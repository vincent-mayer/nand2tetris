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
            'local': '@LCL',
            'argument': '@ARG',
            'this': '@THIS',
            'that': '@THAT',
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
        lines += "@SP\n"
        lines += "M=M+1\n"
        return lines

    def _decrement_and_pop(self, dest="D") -> str:
        lines = ""
        lines += "@SP\n"
        lines += "AM=M-1\n"
        lines += f"{dest}=M\n"  # D = *SP
        return lines

    def write_arithmetic(self, command: str) -> None:
        lines = ""
        if command == "add":
            lines += self._decrement_and_pop(dest="D")
            lines += self._decrement_and_pop(dest="A")
            lines += "D=A+D\n"
            lines += self._push_and_increment()
        elif command == "sub":
            lines += self._decrement_and_pop(dest="D")
            lines += self._decrement_and_pop(dest="A")
            lines += "D=A-D\n"
            lines += self._push_and_increment()
        elif command == "and":
            lines += self._decrement_and_pop(dest="D")
            lines += self._decrement_and_pop(dest="A")
            lines += "D=A&D\n"
            lines += self._push_and_increment()
        elif command == "or":
            lines += self._decrement_and_pop(dest="D")
            lines += self._decrement_and_pop(dest="A")
            lines += "D=A|D\n"
            lines += self._push_and_increment()
        elif command == "not":
            lines += "@SP\n"
            lines += "A=M-1\n"
            lines += "M=!M\n"
        elif command == "neg":
            lines += "@SP\n"
            lines += "A=M-1\n"
            lines += "M=-M\n"
        elif command in ["gt", "lt", "eq"]:
            lines += self._decrement_and_pop(dest="D")
            lines += self._decrement_and_pop(dest="A")
            lines += "D=A-D\n"
            lines += f"@TRUE{self.bool_count}\n"
            lines += f"D;J{command.upper()}\n"
            lines += "@SP"
            lines += "A=M"
            lines += "M=0"
            lines += f"@ENDBOOL{self.bool_count}"
            lines += "0;JMP"
            lines += f"(TRUE{self.bool_count})"
            lines += "@SP"
            lines += "A=M"
            lines += "M=-1"
            lines += f"(ENDBOOL{self.bool_count})"
            self.bool_count += 1

        else:
            raise ArgumentTypeError(f"Invalid arithmetic operation: {command}")
        self.f.write(lines)

    def write_push_pop(self, cmd_type: CmdType, segment: str, index: int) -> None:
        base_address = self.segment_to_base_address[segment]
        lines = ""
        if isinstance(base_address, int):
            base_address = str(base_address)

        if cmd_type == CmdType.C_PUSH:
            if segment != "constant":
                # addr = base + i
                lines += "@" + base_address + "\n"
                lines += "D=A" + "\n"
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
            lines += "@SP" + "\n"
            lines += "M=M+1" + "\n"
        elif cmd_type == CmdType.C_POP:
            # Put base + offset into D
            lines += "@" + str(base_address) + "\n"
            lines += "D=A" + "\n"
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
