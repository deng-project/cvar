// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: Parse.cpp - JSON parser test application
// author: Karl-Mihkel Ott

#include <cvar/JSONUnserializer.h>
#include <cvar/JSONSerializer.h>
#include <cvar/CVarSystem.h>
#include <cvar/SerializerExceptions.h>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 2)
        std::cout << "Usage: Parse <file>\n";

    cvar::CVarSystem& cvarSyst = cvar::CVarSystem::GetInstance();
    try {
        cvarSyst.Unserialize<cvar::JSONUnserializer>(argv[1]);
    }
    catch (const cvar::SyntaxErrorException& e) {
        std::cerr << "[SyntaxErrorException] " << e.what() << '\n';
    }
    catch (const cvar::UnexpectedEOFException& e) {
        std::cerr << "[UnexpectedEOFException] " << e.what() << '\n';
    }

    cvar::JSONSerializer serializer(std::cout, cvarSyst.GetRoot());
    serializer.Serialize();
    return 0;
}
