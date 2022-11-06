// #include "CompilationEngine.h"
// #include "JackTokenizer.h"
// #include <boost/algorithm/string/replace.hpp>
// #include <filesystem>
// #include <stdexcept>
// #include <string>

// inline auto ends_with(std::string const &value, std::string const ending) -> bool
// {
//     if (ending.size() > value.size())
//         return false;
//     return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
// }

// inline auto create_output_path(std::string in_path) -> std::string
// {
//     auto dot_pos = in_path.find(".");
//     auto out_path = in_path.insert(dot_pos, std::string{"V"});
//     return out_path;
// }

// int main(int argc, char *argv[])
// {
//     if (argc == 1)
//     {
//         throw std::invalid_argument("Please provide a path or filename.");
//     }

//     std::string pathOrDir = std::string(argv[1]);

//     if (ends_with(pathOrDir, std::string{".jack"}))
//     {
//         auto pathOut = create_output_path(pathOrDir);
//         boost::replace_all(pathOut, ".jack", ".xml");
//         auto engine = CompilationEngine(
//             std::move(std::make_unique<JackTokenizer>(pathOrDir)), pathOut);
//     }
//     else
//     {

//         for (const auto &dirEntry :
//              std::filesystem::recursive_directory_iterator(pathOrDir))
//         {
//             std::cout << dirEntry << std::endl;

//             // If not a jack file skip.
//             if (!ends_with(dirEntry.path().string(), std::string{".jack"}))
//                 continue;

//             // Input path
//             auto pathIn = dirEntry.path();

//             // Output path
//             auto pathOut = create_output_path(pathIn.string());
//             boost::replace_all(pathOut, ".jack", ".xml");

//             // Engine
//             auto engine = CompilationEngine(
//                 std::move(std::make_unique<JackTokenizer>(pathIn.string())), pathOut);

//             // Compile.
//             engine.compileClass();
//         }
//     }
//     return 0;
// }