// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: Parse.cpp - JSON parser test application
// author: Karl-Mihkel Ott

#include <cvar/JSONUnserializer.h>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 2)
        std::cout << "Usage: Parse <file>\n";

    std::ifstream stream(argv[1]);
    CVar::JSONUnserializer unserializer(stream);
    return 0;
}
