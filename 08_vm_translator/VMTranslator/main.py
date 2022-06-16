import sys
import shutil
import os
from pathlib import Path
from VMTranslator import Parser, CodeWriter
from VMTranslator import CommandType as CmdType
from typing import List


def build_output_path(input_path: str) -> str:
    if ".vm" in input_path:
        output_path = input_path.replace(".vm", ".asm")
    else:
        dir_name = input_path.split("/")[-1]
        output_path = input_path + "/" + dir_name + ".asm"
    return output_path


def parse_files(file_path_or_dir: str) -> List[str]:
    ret_list = []
    if ".vm" in file_path_or_dir:
        ret_list.append(file_path_or_dir)
    else:
        path_to_dir = str(Path(file_path_or_dir).resolve())
        files = os.listdir(path_to_dir)
        for f in files:
            if ".vm" in f:
                ret_list.append(os.path.join(path_to_dir, f))
    return ret_list


def copy_asm_to_test(orig_save_path: str) -> str:
    new_save_path = "/home/vincent/Documents/courses/nand2tetris/projects/08" + \
        orig_save_path
    new_save_path = new_save_path.replace("tests", "")
    if os.path.exists(new_save_path):
        os.remove(new_save_path)
    orig_save_path = str(Path(orig_save_path).resolve())
    shutil.copy2(orig_save_path, new_save_path)


if __name__ == "__main__":
    arg = sys.argv[1]

    path_to_output_file = build_output_path(arg)
    vm_files = parse_files(arg)
    writer = CodeWriter(path_to_output_file)
    writer.write_init()
    # Either translate individual vm file or all vm files in dir.
    for file in vm_files:
        parser = Parser(file)
        writer.set_file_name(file)

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

        parser.close()

    writer.close()
    copy_asm_to_test(path_to_output_file)
