#include <JackTokenizer.h>

int main(int argc, char *argv[])
{
    std::string path_in =
        "/home/vincent/repos/nand2tetris/10_compiler_I/test/ArrayTest/Main.jack";
    JackTokenizer tokenizer(path_in);
    while (tokenizer.hasMoreTokens())
    {
        tokenizer.advance();
    }
    return 0;
}