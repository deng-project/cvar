// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: Parse.cpp - JSON parser test application
// author: Karl-Mihkel Ott

#include <cvar/JSONUnserializer.h>
#include <cvar/JSONSerializer.h>
#include <cvar/CVarSystem.h>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 2)
        std::cout << "Usage: Parse <file>\n";

    CVar::CVarSystem& cvarSyst = CVar::CVarSystem::GetInstance();
    try {
        cvarSyst.Unserialize<CVar::JSONUnserializer>(argv[1]);
    }
    catch (const CVar::SyntaxErrorException& e) {
        std::cerr << "[SyntaxErrorException] " << e.what() << '\n';
    }
    catch (const CVar::UnexpectedEOFException& e) {
        std::cerr << "[UnexpectedEOFException] " << e.what() << '\n';
    }
    cvarSyst.Serialize<CVar::JSONSerializer>(std::string(argv[1]) + ".copy");
    return 0;
}
