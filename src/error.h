#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Error
{
public:
    string message;
    int line;
    int column;

    Error(const string &message, int line = -1, int column = -1)
        : message(message), line(line), column(column) {}
};

class ErrorReporter
{
private:
    vector<Error> errors;
    bool hasError = false;

public:
    void reportError(const string &message, int line = -1, int column = -1)
    {
        errors.push_back(Error(message, line, column));
        hasError = true;
    }

    bool hadError() const { return hasError; }

    void printErrors() const
    {
        for (const auto &error : errors)
        {
            cerr << "Error";
            if (error.line != -1)
            {
                cerr << " at line " << error.line;
                if (error.column != -1)
                {
                    cerr << ", column " << error.column;
                }
            }
            cerr << ": " << error.message << endl;
        }
    }

    void clear()
    {
        errors.clear();
        hasError = false;
    }
};

extern ErrorReporter errorReporter;

#endif 