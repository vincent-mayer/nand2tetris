from distutils.cmd import Command
import os
from pathlib import Path
from typing import Union
from xml.dom import InvalidAccessErr
from VMTranslator import CommandType as CmdType

ARTHM_CMDS = ["add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"]


class Parser:

    def __init__(self, path_to_file: Union[str, Path]) -> None:
        path_to_file = path_to_file if isinstance(path_to_file,
                                                  Path) else Path(path_to_file)
        self.f = open(str(path_to_file.resolve(strict=True)), "r")
        self.eof = os.fstat(self.f.fileno()).st_size
        self.cmd = ""
        self.cmd_clean = ""

    def _ignore_line(self, line: str) -> bool:
        if line.startswith("//") or line.startswith("\n"):
            return True
        else:
            return False

    def has_more_commands(self):
        return self.f.tell() != self.eof

    def command_type(self) -> CmdType:
        if "push" in self.cmd:
            cmd_type = CmdType.C_PUSH
        elif "pop" in self.cmd:
            cmd_type = CmdType.C_POP
        elif self.cmd_clean in ARTHM_CMDS:
            cmd_type = CmdType.C_ARITHMETIC
        elif "label" in self.cmd:
            cmd_type = CmdType.C_LABEL
        elif "goto" in self.cmd:
            cmd_type = CmdType.C_GOTO
        elif "if-goto" in self.cmd:
            cmd_type = CmdType.C_IF
        elif "return" in self.cmd:
            cmd_type = CmdType.C_RETURN
        elif "function" in self.cmd:
            cmd_type = CmdType.C_FUNCTION
        elif "call" in self.cmd:
            cmd_type = CmdType.C_CALL
        else:
            raise InvalidAccessErr("Wrong command parsed from .vm file!")
        return cmd_type

    def arg1(self) -> str:
        cmd_type = self.command_type()
        assert (cmd_type != CmdType.C_RETURN)

        if cmd_type == CmdType.C_ARITHMETIC:
            arg1 = self.cmd_clean
        else:
            arg1 = self.cmd.split(" ")[1]
        return arg1

    def arg2(self) -> int:
        cmd_type = self.command_type()
        assert (cmd_type
                in [CmdType.C_PUSH, CmdType.C_POP, CmdType.C_FUNCTION, CmdType.C_CALL])
        return int(self.cmd.split(" ")[-1])

    def advance(self) -> None:
        while (self.has_more_commands()):
            line = self.f.readline()
            if self._ignore_line(line):
                continue
            else:
                print(line)
                break
        if "//" in line:
            line = line.split("//")[0]
        self.cmd = line.replace("\n", "")
        self.cmd_clean = self.cmd.replace(" ", "")
        print(self.cmd)
        print(self.cmd_clean)

    def close(self):
        self.f.close()
