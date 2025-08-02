#include <argparse/argparse.hpp>


int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("zeta-cli"); 

    program.add_argument("-p", "--precision")
    .help("calculation precision in bits")
    .scan<'i', int>()
    .default_value(512);

    program.add_argument("-m", "--method")
    .help("chooses solving method");

    program.add_argument("-m", "--matrix")
    .help("input matrix size for system")
    .scan<'i', int>()
    .nargs(1)
    .required()
    .default_value(50);

    program.add_argument("-s", "--save")
    .help("saves solution in default saves dir")
    .default_value(false)
    .implicit_value(true);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    return 0;   
}