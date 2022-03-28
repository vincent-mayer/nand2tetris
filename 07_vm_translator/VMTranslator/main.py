import sys
from VMTranslator import Parser, CodeWriter
from VMTranslator import CommandType as CmdType


def build_output_path(input_path: str) -> str:
    output_path = input_path.replace(".vm", ".asm")
    if "input/" in output_path:
        output_path = output_path.replace("input/", "output/")
    return output_path


if __name__ == "__main__":
    arg = sys.argv[1]

    # Either translate individual vm file or all vm files in dir.
    if arg.endswith(".vm"):
        parser = Parser(arg)
        writer = CodeWriter(build_output_path(arg))

        while parser.has_more_commands():
            parser.advance()
            cmd_type = parser.command_type()
            arg1 = parser.arg1()

            if cmd_type in [CmdType.C_PUSH, CmdType.C_POP]:
                arg2 = parser.arg2()
                writer.write_push_pop(cmd_type, arg1, arg2)
            elif cmd_type in [CmdType.C_ARITHMETIC]:
                writer.write_arithmetic(arg1)

        parser.close()
        writer.close()

    else:
        pass
