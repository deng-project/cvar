// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: InteractiveConsole.cpp - Interactive CVar console program implementation
// author: Karl-Mihkel Ott

#include <iostream>
#include <sstream>
#include <cvar/CVarSystem.h>
#include <cvar/JSONSerializer.h>

using namespace std;

static string sHelpText = "Standard mode commands are:\n"\
                          "<variable> - outputs variable value if available\n"\
                          "<variable>=<value> - sets variable value\n\n"\
                          "Command options are denoted with ':cmd'\n"\
                          ":cmd save <json|yaml|xml> filename.<ext> [min] - serialize variables to file\n"\
                          ":cmd load <json|yaml|xml> filename.<ext> - unserialize variables from file\n";


void TrimString(string& _str) {
    // front
    size_t i = 0;
    while (_str[i] == ' ' || _str[i] == '\t')
        i++;
    _str = _str.substr(i);

    // back
    while (_str.back() == ' ' || _str.back() == '\t')
        _str.pop_back();
}

// for now only primitive types are supported
CVar::Type DetectValueType(const std::string _sValue) {
    if ((_sValue.front() == '"' && _sValue.back() == '"') ||
        (_sValue.front() == '\'' && _sValue.back() == '\''))
        return CVar::Type_String;
    else if (_sValue == "true" || _sValue == "false") {
        return CVar::Type_Bool;
    } 
    else {
        bool bInt = true;
        for (size_t i = 0; i < _sValue.size(); i++) {
            if (_sValue[i] < '0' && _sValue[i] > '9' && _sValue[i] != '.')
                return CVar::Type_None;
            else if (_sValue[i] == '.')
                bInt = false;
        }

        if (bInt) return CVar::Type_Int;
        else return CVar::Type_Float;
    }
}

void Set(const string& _sLine) {
    // extract the variable name part and value part
    size_t uPosEq = _sLine.find('=');
    string sVarName = _sLine.substr(0, uPosEq);
    string sValue = _sLine.substr(uPosEq + 1);

    TrimString(sVarName);
    TrimString(sValue);

    auto type = DetectValueType(sValue);
    CVar::CVarSystem& cvarSyst = CVar::CVarSystem::GetInstance();

    switch (type) {
        case CVar::Type_String:
            cvarSyst.Set<CVar::String>(sVarName, sValue.substr(1, sValue.size()-2));
            break;

        case CVar::Type_Bool:
            cvarSyst.Set<CVar::Bool>(sVarName, (sValue == "true" ? true : false));
            break;

        case CVar::Type_Int:
            cvarSyst.Set<CVar::Int>(sVarName, static_cast<CVar::Int>(std::stoi(sValue)));
            break;

        case CVar::Type_Float:
            cvarSyst.Set<CVar::Float>(sVarName, static_cast<CVar::Float>(std::stof(sValue)));
            break;

        default:
            cout << "Could not determine type for value for '" << sValue << "'\n";
            break;
    }
}


string Get(const string& _sLine) {
    CVar::CVarSystem& cvarSyst = CVar::CVarSystem::GetInstance();

    CVar::Value* pValue = cvarSyst.GetValue(_sLine);
    stringstream ss;
    if (!pValue) 
        ss << "Invalid variable '" << _sLine << "'";
    else {
        switch (pValue->index()) {
            case CVar::Type_Int:
                ss << std::get<CVar::Type_Int>(*pValue);
                break;

            case CVar::Type_Float:
                ss << std::get<CVar::Type_Float>(*pValue);
                break;

            case CVar::Type_Bool:
                ss << (std::get<CVar::Type_Bool>(*pValue) ? "true" : "false");
                break;
            
            case CVar::Type_String:
                ss << std::get<CVar::Type_String>(*pValue);
                break;

            case CVar::Type_List:
                ss << std::get<CVar::Type_List>(*pValue);
                break;

            case CVar::Type_Object:
                {
                    auto pObject = std::get<CVar::Type_Object>(*pValue);
                    ss << *(pObject.get());
                }
                break;

            default:
                ss << "Invalid variable '" << _sLine << "'";
                break;
        }
    }

    return ss.str();
}

void Cmd(const string& _sLine) {
    stringstream ss(_sLine);
    std::vector<string> words;

    string word;
    while (ss >> word)
        words.push_back(word);

    if (words.size() < 4) {
        cout << sHelpText;
        return;
    } else if (words[1] == "save") {
        if (words[2] == "json") {
            CVar::CVarSystem& cvarSyst = CVar::CVarSystem::GetInstance();

            // check if minified json should be used
            if (words.size() == 5 && words.back() == "min") {
                cvarSyst.Serialize<CVar::JSONSerializer>(words[3], false);
            } else {
                cvarSyst.Serialize<CVar::JSONSerializer>(words[3], true);
            }
            cout << "Serialized to '" << words[2] << "'\n";
        } else if (words[2] == "yaml") {
            cout << "Yaml serializer is not yet implemented :(\n";
            return;
        } else if (words[2] == "xml") {
            cout << "XML serializer is not yet implemented :(\n";
            return;
        }
    } 
    else if (words[1] == "load") {
        cout << std::uppercase << words[2] << "unserializer is not yet implemented :(\n";
        return;
    }
}

enum class ActionType {
    Set,
    Get,
    Cmd
};


inline ActionType FindActionType(const string& _sLine) {
    if (_sLine.find('=') != string::npos) {
        return ActionType::Set;
    }
    else if (_sLine.find(":cmd ") == 0)
        return ActionType::Cmd;
    else return ActionType::Get;
}

int main(void) {
    cout << "Welcome to interactive CVar console!\n"\
            "Type <variable> to see value\n"\
            "Type <variable>=<value> to set a value\n"\
            "Type help for more options\n";

    string sPrompt = "InteractiveConsole > ";

    while (true) {
        cout << sPrompt;
        cout.flush();

        string sLine;
        getline(cin, sLine);

        if (sLine == "exit" || sLine == "quit")
            break;
        else if (sLine == "help")
            cout << sHelpText;

        ActionType actType = FindActionType(sLine);

        switch (actType) {
            case ActionType::Set:
                Set(sLine);
                break;

            case ActionType::Get:
                cout << Get(sLine) << '\n';
                break;

            case ActionType::Cmd:
                Cmd(sLine);
                break;
        }
    }
    return 0;
}
