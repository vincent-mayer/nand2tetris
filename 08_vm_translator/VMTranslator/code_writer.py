from argparse import ArgumentTypeError
from typing import Union
from pathlib import Path
from VMTranslator import CommandType as CmdType


class CodeWriter:

    def __init__(self, path_to_file: Union[str, Path]) -> None:
        path_to_file = path_to_file if isinstance(path_to_file,
                                                  Path) else Path(path_to_file)
        self.f = open(str(path_to_file.resolve()), "w")
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

    def write_push_pop(self, cmd_type: CmdType, segment: str, index: int) -> None:
        base_address = self.segment_to_base_address[segment]
        lines = ""

        if cmd_type == CmdType.C_PUSH:
            if segment != "constant":
                # addr = base + i
                lines += "@" + str(base_address) + "\n"
                if isinstance(base_address, int):
                    lines += "D=A" + "\n"
                else:
                    lines += "D=M" + "\n"
                lines += "@" + str(index) + "\n"
                lines += "A=D+A" + "\n"
                # D = *addr = *(base_segment + index)
                lines += "D=M" + "\n"
            else:
                lines += "@" + str(index) + "\n"
                lines += "D=A" + "\n"
            # *SP = *addr
            lines += "@SP" + "\n"
            lines += "A=M" + "\n"
            lines += "M=D" + "\n"
            # SP++
            lines += self._increment_sp()
        elif cmd_type == CmdType.C_POP:
            # Put base + offset into D
            lines += "@" + str(base_address) + "\n"
            if isinstance(base_address, int):
                lines += "D=A" + "\n"
            else:
                lines += "D=M" + "\n"
            lines += "@" + str(index) + "\n"
            lines += "D=D+A" + "\n"
            # save target address in r13
            lines += "@R13" + "\n"
            lines += "M=D" + "\n"
            # SP--, D = *SP
            lines += self._decrement_and_pop(dest="D")
            # *addr = R13 = *SP
            lines += "@R13" + "\n"  # R13 must hold target address
            lines += "A=M" + "\n"
            lines += "M=D" + "\n"
        self.f.write(lines)

    def close(self):
        self.f.close()
