#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "error.h"

using namespace std;
namespace fs = std::filesystem;

string readFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printUsage()
{
    cout << "Usage:" << endl;
    cout << "  nova <file.nova>                # Compile and run" << endl;
    cout << "  nova <file.nova> <output.c>     # Compile to C file" << endl;
    cout << "  nova --help                     # Show this help" << endl;
}

bool compileNovaToC(const string &inputFile, const string &outputFile)
{
    string input = readFile(inputFile);

    Parser parser(input);
    auto program = parser.parse();

    if (errorReporter.hadError())
    {
        errorReporter.printErrors();
        return false;
    }

    SemanticAnalyzer analyzer;
    analyzer.analyze(program);

    if (errorReporter.hadError())
    {
        errorReporter.printErrors();
        return false;
    }

    ofstream output(outputFile);
    if (!output.is_open())
    {
        cerr << "Error: Could not open output file " << outputFile << endl;
        return false;
    }

    CodeGenerator generator(output);
    generator.generate(program);
    output.close();

    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    string firstArg = argv[1];

    if (firstArg == "--help" || firstArg == "-h")
    {
        printUsage();
        return 0;
    }

    if (!fs::exists(firstArg))
    {
        cerr << "Error: File not found: " << firstArg << endl;
        return 1;
    }

    if (firstArg.size() < 5 || firstArg.substr(firstArg.size() - 5) != ".nova")
    {
        cerr << "Error: Input file must have .nova extension" << endl;
        return 1;
    }

    if (argc == 2)
    {

        string tempCFile = "/tmp/nova_temp.c";
        string tempExe = "/tmp/nova_temp_exe";

        cout << "Compiling " << firstArg << "..." << endl;

        if (!compileNovaToC(firstArg, tempCFile))
        {
            return 1;
        }

        string compileCmd = "gcc -o " + tempExe + " " + tempCFile + " 2>/dev/null";
        int compileResult = system(compileCmd.c_str());

        if (compileResult != 0)
        {
            cerr << "Error: Failed to compile generated C code" << endl;
            remove(tempCFile.c_str());
            return 1;
        }

        int runResult = system(tempExe.c_str());

        remove(tempCFile.c_str());
        remove(tempExe.c_str());

        return WEXITSTATUS(runResult);
    }
    else if (argc == 3)
    {

        string inputFile = argv[1];
        string outputFile = argv[2];

        if (compileNovaToC(inputFile, outputFile))
        {
            cout << "Successfully compiled " << inputFile << " to " << outputFile << endl;
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        printUsage();
        return 1;
    }
}