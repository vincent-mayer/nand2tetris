from argparse import ArgumentTypeError
from typing import Union
from pathlib import Path
from VMTranslator import CommandType as CmdType


class CodeWriter:

    def __init__(self, path_to_asm: Union[str, Path]) -> None:
        path_to_asm = path_to_asm if isinstance(path_to_asm, Path) else Path(path_to_asm)
        self.f = open(str(path_to_asm.resolve()), "w")
        self.segment_to_base_address = {
            'local': 'LCL',
            'argument': 'ARG',
            'this': 'THIS',
            'that': 'THAT',
            'static': 16,
            'temp': 5,
            'pointer': 3,
            'constant': -1
        }
        self.bool_count = 0
        self.call_count = 0
        self.curr_file = ""

    def set_file_name(self, vm_file_name: str) -> None:
        self.curr_file = vm_file_name.split("/")[-1].replace(".vm", "")
        self.f.write('//////\n')
        self.f.write(f'// {self.curr_file}\n')

    def _push_and_increment(self) -> str:
        lines = ""
        lines += "@SP\n"
        lines += "A=M\n"
        lines += "M=D\n"
        lines += self._increment_sp()
        return lines

    def _increment_sp(self) -> str:
        lines = ""
        lines += "@SP\n"
        lines += "M=M+1\n"
        return lines

    def _decrement_sp(self) -> str:
        lines = ""
        lines += "@SP\n"
        lines += "M=M-1\n"
        lines += "A=M\n"
        return lines

    def _decrement_and_pop(self, dest="D") -> str:
        lines = ""
        lines += "@SP\n"
        lines += "AM=M-1\n"
        lines += f"{dest}=M\n"  # D = *SP
        return lines

    def write_init(self) -> None:
        lines = ""
        lines += "@256\n"
        lines += "D=A\n"
        lines += "@SP\n"
        lines += "M=D\n"
        self.f.write(lines)
        self.write_call("Sys.init", 0)

    def write_arithmetic(self, command: str) -> None:
        lines = ""
        # Unitary command
        if command in ["not", "neg"]:
            lines += self._decrement_sp()
        else:
            lines += self._decrement_and_pop(dest="D")
            lines += self._decrement_and_pop(dest="A")

        # Add line for command
        if command == "add":
            lines += "D=A+D\n"
        elif command == "sub":
            lines += "D=A-D\n"
        elif command == "and":
            lines += "D=A&D\n"
        elif command == "or":
            lines += "D=A|D\n"
        elif command == "not":
            lines += "M=!M\n"
        elif command == "neg":
            lines += "M=-M\n"
        elif command in ["gt", "lt", "eq"]:
            lines += "D=A-D\n"
            lines += f"@TRUE{self.bool_count}\n"
            lines += f"D;J{command.upper()}\n"
            lines += "@SP\n"
            lines += "A=M\n"
            lines += "M=0\n"
            lines += f"@ENDBOOL{self.bool_count}\n"
            lines += "0;JMP\n"
            lines += f"(TRUE{self.bool_count})\n"
            lines += "@SP\n"
            lines += "A=M\n"
            lines += "M=-1\n"
            lines += f"(ENDBOOL{self.bool_count})\n"
            self.bool_count += 1
        else:
            raise ArgumentTypeError(f"Invalid arithmetic operation: {command}")

        if command in ["gt", "lt", "eq", "neg", "not"]:
            lines += self._increment_sp()
        else:
            lines += self._push_and_increment()

        self.f.write(lines)

    def _resolve_address(self, segment: str, index: int) -> str:
        lines = ""
        base_address = self.segment_to_base_address[segment]
        if segment == "constant":
            lines += "@" + str(index) + "\n"
        elif segment == "static":
            lines += "@" + self.curr_file + "." + str(index) + "\n"
        elif segment in ["pointer", "temp"]:
            lines += "@R" + str(base_address + index) + "\n"
        elif segment in ["local", "argument", "this", "that"]:
            lines += "@" + base_address + "\n"
            lines += "D=M\n"
            lines += "@" + str(index) + "\n"
            lines += "A=D+A\n"
        else:
            raise ValueError(f"Invalid segment: {segment}")

        return lines

    def write_push_pop(self, cmd_type: CmdType, segment: str, index: int) -> None:
        lines = ""
        lines += self._resolve_address(segment, index)
        if cmd_type == CmdType.C_PUSH:
            if segment == "constant":
                lines += "D=A\n"
            else:
                lines += "D=M\n"
            lines += self._push_and_increment()
        elif cmd_type == CmdType.C_POP:
            lines += "D=A\n"
            # save target address in r13
            lines += "@R13\n"
            lines += "M=D\n"
            # SP--, D = *SP
            lines += self._decrement_and_pop(dest="D")
            # *addr = R13 = *SP
            lines += "@R13\n"  # R13 must hold target address
            lines += "A=M\n"
            lines += "M=D\n"
        else:
            raise ValueError(f"Invalid command type: {cmd_type}!")
        self.f.write(lines)

    def write_label(self, label: str) -> None:
        self.f.write(f"({self.curr_file}${label})\n")

    def write_goto(self, label: str) -> None:
        self.f.write(f"@{self.curr_file}${label}\n")
        self.f.write("0;JMP\n")

    def write_if(self, label: str) -> None:
        lines = ""
        lines += self._decrement_and_pop("D")
        lines += f"@{self.curr_file}${label}\n"
        lines += "D;JNE\n"
        self.f.write(lines)

    def write_call(self, f_name: str, n_args: int) -> None:
        ret_label = f"{self.curr_file}${f_name}RET{self.call_count}"
        self.call_count += 1
        lines = ""
        # push return-address
        lines += f"@{ret_label}\n"
        lines += "D=A\n"
        lines += self._push_and_increment()
        # push "LCL", "ARG", "THIS", "THAT"
        for label in ["LCL", "ARG", "THIS", "THAT"]:
            lines += f"@{label}\n"
            lines += "D=M\n"
            lines += self._push_and_increment()
        # LCL = SP
        lines += "@SP\n"
        lines += "D=M\n"
        lines += "@LCL\n"
        lines += "M=D\n"
        # ARG = SP-(n+5)
        lines += f"@{n_args+5}\n"
        lines += "D=D-A\n"
        lines += "@ARG\n"
        lines += "M=D\n"

        # goto f
        lines += f"@{f_name}\n"
        lines += "0;JMP\n"
        lines += "(" + ret_label + ")\n"

        self.f.write(lines)

    def write_return(self) -> None:
        lines = ""
        # FRAME (R14) = LCL
        lines += "@LCL\n"
        lines += "D=M\n"
        lines += "@R14\n"
        lines += "M=D\n"
        # RET (R15) = *(FRAME-5)
        lines += "@5\n"
        lines += "A=D-A\n"
        lines += "D=M\n"  # D=*(FRAME-5)
        lines += "@R15\n"
        lines += "M=D\n"  # R15=D=(FRAME-5)
        # *ARG = pop()
        lines += self._decrement_and_pop(dest="D")
        lines += "@ARG\n"
        lines += "A=M\n"
        lines += "M=D\n"
        # SP = ARG+1
        lines += "@ARG\n"
        lines += "D=M+1\n"
        lines += "@SP\n"
        lines += "M=D\n"
        # THAT/THIS/ARG/LCL = *(FRAME-i+1)
        for i, label in enumerate(["THAT", "THIS", "ARG", "LCL"]):
            lines += f"@{i+1}\n"
            lines += "D=A\n"
            lines += "@R14\n"
            lines += "A=M-D\n"
            lines += "D=M\n"
            lines += f"@{label}\n"
            lines += "M=D\n"
        # goto RET
        lines += "@R15\n"
        lines += "A=M\n"
        lines += "0;JMP\n"
        # write
        self.f.write(lines)

    def write_function(self, label: str, n_locals: int) -> None:
        lines = ""
        lines += f"({label})\n"
        for _ in range(n_locals):
            lines += "D=0\n"
            lines += self._push_and_increment()
        self.f.write(lines)

    def close(self):
        self.f.close()
