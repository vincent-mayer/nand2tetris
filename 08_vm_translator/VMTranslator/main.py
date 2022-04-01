import sys
import shutil
import os
from pathlib import Path
from VMTranslator import Parser, CodeWriter
from VMTranslator import CommandType as CmdType


def build_output_path(input_path: str) -> str:
    output_path = input_path.replace(".vm", ".asm")
    if "input/" in output_path:
        output_path = output_path.replace("input/", "output/")
    return output_path


def copy_asm_to_test(orig_save_path: str) -> str:
    new_save_path = "/home/vincent/Documents/courses/nand2tetris/projects/08" + \
        orig_save_path
    new_save_path = new_save_path.replace("tests", "")
    if os.path.exists(new_save_path):
        os.remove(new_save_path)
    orig_save_path = str(Path(orig_save_path).resolve())
    shutil.move(orig_save_path, new_save_path)


if __name__ == "__main__":
    arg = sys.argv[1]

    # Either translate individual vm file or all vm files in dir.
    if arg.endswith(".vm"):
        parser = Parser(arg)
        writer = CodeWriter(build_output_path(arg))

        writer.set_file_name(arg)
        writer.write_init()

        while parser.has_more_commands():
            parser.advance()
            cmd_type = parser.command_type()

            if cmd_type in [CmdType.C_PUSH, CmdType.C_POP]:
                writer.write_push_pop(cmd_type, parser.arg1(), parser.arg2())
            elif cmd_type in [CmdType.C_ARITHMETIC]:
                writer.write_arithmetic(parser.arg1())
            elif cmd_type == CmdType.C_LABEL:
                writer.write_label(parser.arg1())
            elif cmd_type == CmdType.C_GOTO:
                writer.write_goto(parser.arg1())
            elif cmd_type == CmdType.C_IF:
                writer.write_if(parser.arg1())
            elif cmd_type == CmdType.C_FUNCTION:
                writer.write_function(parser.arg1(), parser.arg2())
            elif cmd_type == CmdType.C_RETURN:
                writer.write_return()
            elif cmd_type == CmdType.C_CALL:
                writer.write_call(parser.arg1(), parser.arg2())

        copy_asm_to_test(build_output_path(arg))

        parser.close()
        writer.close()

    else:
        pass
