// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONUnserializer.cpp - JSON unserializer class implementation
// author: Karl-Mihkel Ott

#include <iostream>
#include <cstring>
#include <cvar/JSONUnserializer.h>

namespace CVar {

    std::ostream& operator<<(std::ostream& _stream, std::variant<char, String, Float, Int, Bool, JSONNull>& _token) {
        switch (_token.index()) {
            case JSONTokenIndex_Char:
                _stream << std::get<char>(_token);
                break;

            case JSONTokenIndex_String:
                _stream << std::get<String>(_token);
                break;

            case JSONTokenIndex_Float:
                _stream << std::get<Float>(_token);
                break;

            case JSONTokenIndex_Int:
                _stream << std::get<Int>(_token);
                break;

            case JSONTokenIndex_Bool:
                _stream << (std::get<Bool>(_token) ? "true" : "false");
                break;

            case JSONTokenIndex_JSONNull:
                _stream << "null";
                break;

            default:
                break;
        }

        return _stream;
    }

    
    JSONUnserializer::JSONUnserializer(std::istream& _stream, std::ostream& _errStream) :
        IUnserializer(_stream, _errStream)
    {
        _Lex();
        _Parse();
    }


    std::variant<std::monostate, String> JSONUnserializer::_LexString() {
        std::variant<std::monostate, String> jsonString = std::monostate{};
        char cQuot = m_stream.peek();

        if (cQuot == '"' || cQuot == '\'') {
            static_cast<void>(m_stream.get());
            std::string jsonStdString;

            while(m_stream.peek() != -1 && m_stream.peek() != cQuot)
                jsonStdString += m_stream.get();

            jsonString = String(jsonStdString);

            if (!m_stream.eof())
                static_cast<void>(m_stream.get());
        }

        return jsonString;
    }


    std::variant<std::monostate, Int> JSONUnserializer::_LexInt() {
        std::variant<std::monostate, Int> jsonInt = std::monostate{};
        
        std::string sJsonInt;
        while (!m_stream.eof() && ((m_stream.peek() <= '9' && m_stream.peek() >= '0') || m_stream.peek() == '-'))
            sJsonInt += m_stream.get();

        if (m_stream.peek() == '.')
            return jsonInt;
        else if (!sJsonInt.empty())
            jsonInt = static_cast<Int>(std::stoi(sJsonInt));

        return jsonInt;
    }


    std::variant<std::monostate, Float> JSONUnserializer::_LexFloat() {
        std::variant<std::monostate, Float> jsonFloat = std::monostate{};

        std::string sJsonFloat;
        while (!m_stream.eof() && ((m_stream.peek() <= '9' && m_stream.peek() >= '0') || m_stream.peek() == '.' || m_stream.peek() == '-'))
            sJsonFloat += m_stream.get();

        if (!sJsonFloat.empty())
            jsonFloat = static_cast<Float>(std::stof(sJsonFloat));

        return jsonFloat;
    }


    std::variant<std::monostate, Bool> JSONUnserializer::_LexBool() {
        char buf[sizeof("false")] = {};

        // true
        size_t i = 0;
        for (i = 0; i < sizeof("true")-1 && m_stream.peek() != -1; i++)
            buf[i] = static_cast<char>(m_stream.get());

        if (!std::strcmp(buf, "true"))
            return true;

        for (size_t j = 0; j < i; j++)
            m_stream.unget();
        m_stream.clear();

        // false
        for (i = 0; i < sizeof("false")-1 && m_stream.peek() != -1; i++)
            buf[i] = static_cast<char>(m_stream.get());

        if (!std::strcmp(buf, "false"))
            return false;

        for (size_t j = 0; j < i; j++)
            m_stream.unget();
        m_stream.clear();

        return std::monostate{};
    }


    std::variant<std::monostate, JSONNull> JSONUnserializer::_LexNull() {
        char buf[sizeof("null")] = {};

        size_t i = 0;
        for (i = 0; i < sizeof("null")-1 && m_stream.peek() != -1; i++)
            buf[i] = m_stream.get();

        if (!std::strcmp(buf, "null"))
            return JSONNull{};

        for (size_t j = 0; j < i; j++)
            m_stream.unget();
        m_stream.clear();

        return std::monostate{};
    }


    bool JSONUnserializer::_Contains(char _c, const char* _szFilter, size_t _uSize) {
        for (size_t i = 0; i < _uSize; i++)
            if (_szFilter[i] == _c) return true;

        return false;
    }


    void JSONUnserializer::_Lex() {
        while (m_stream.peek() != -1) {
            std::variant<std::monostate, String> jsonString = _LexString();
            if (jsonString.index()) {
                m_qTokens.emplace(std::get<String>(jsonString), m_uLineCounter);
                continue;
            }

            std::variant<std::monostate, Int> jsonInt = _LexInt();
            if (jsonInt.index()) {
                m_qTokens.emplace(std::get<Int>(jsonInt), m_uLineCounter);
                continue;
            }

            std::variant<std::monostate, Float> jsonFloat = _LexFloat();
            if (jsonFloat.index()) {
                m_qTokens.emplace(std::get<Float>(jsonFloat), m_uLineCounter);
                continue;
            }

            std::variant<std::monostate, Bool> jsonBool = _LexBool();
            if (jsonBool.index()) {
                m_qTokens.emplace(std::get<Bool>(jsonBool), m_uLineCounter);
                continue;
            }

            std::variant<std::monostate, JSONNull> jsonNull = _LexNull();
            if (jsonNull.index()) {
                m_qTokens.emplace(JSONNull{}, m_uLineCounter);
                continue;
            }

            if (_Contains(m_stream.peek(), m_szJsonSyntax, sizeof(m_szJsonSyntax)))
                m_qTokens.emplace(static_cast<char>(m_stream.get()), m_uLineCounter);
            else if (_Contains(m_stream.peek(), m_szJsonWhitespace, sizeof(m_szJsonWhitespace))) {
                if (static_cast<char>(m_stream.peek()) == '\n')
                    m_uLineCounter++;
                static_cast<void>(m_stream.get());
            }
            else {
                m_errStream << "Unexpected symbol '" << static_cast<char>(m_stream.peek()) << "' at line " << m_uLineCounter << '\n';
                break;
            }
        }
    }


    List JSONUnserializer::_ParseList() {
        List rootList;
        std::stack<List*> stckList;
        stckList.push(&rootList);

        bool bInitialDecl = false;
        while (!m_qTokens.empty() && !stckList.empty()) {
            JSONToken* pToken = &m_qTokens.front();
            List* pCurrentList = stckList.top();

            // check for initial list declaration
            if (!bInitialDecl && pToken->token.index() == JSONTokenIndex_Char && std::get<char>(pToken->token) == '[') {
                m_qTokens.pop();
                if (_EofError()) return rootList;
                pToken = &m_qTokens.front();
                bInitialDecl = true;
            } else if (!bInitialDecl) {
                m_errStream << "Unexpected identifier '" << pToken->token << "' at line " << pToken->uLine << '\n';
                m_qTokens.pop();
                return rootList;
            }

            // check for end statement
            if (pToken->token.index() == JSONTokenIndex_Char && std::get<char>(pToken->token) == '}') {
                m_qTokens.pop();
                stckList.pop();

                if (!stckList.empty()) {
                    if (_EofError()) return List();
                    pToken = &m_qTokens.front();

                    if (pToken->token.index() != JSONTokenIndex_Char) {
                        m_errStream << "Expected comma before line " << pToken->uLine << '\n';
                        return List();
                    } 
                    else if (std::get<char>(pToken->token) == ',') {
                        m_qTokens.pop();
                    }
                }

                continue;
            }

            // expect a value
            switch (pToken->token.index()) {
                case JSONTokenIndex_Char:
                    if (std::get<char>(pToken->token) == '[') {
                        pCurrentList->PushBack(std::make_shared<List>());
                        stckList.push(std::get<std::shared_ptr<List>>(*pCurrentList->ReverseBegin()).get());
                    }
                    else if (std::get<char>(pToken->token) == '{') {
                        pCurrentList->PushBack(std::make_shared<Object>());
                        _ParseObject(&std::get<std::shared_ptr<Object>>(*pCurrentList->ReverseBegin()).get()->GetContents());
                    }
                    else if (std::get<char>(pToken->token) == ']') {
                        stckList.pop();
                        continue;
                    }
                    break;

                case JSONTokenIndex_String:
                    pCurrentList->PushBack(std::get<String>(pToken->token));
                    break;

                case JSONTokenIndex_Float:
                    pCurrentList->PushBack(std::get<Float>(pToken->token));
                    break;

                case JSONTokenIndex_Int:
                    pCurrentList->PushBack(std::get<Int>(pToken->token));
                    break;

                case JSONTokenIndex_Bool:
                    pCurrentList->PushBack(std::get<Bool>(pToken->token));
                    break;

                case JSONTokenIndex_JSONNull:
                    pCurrentList->PushBack<Int>(0);
                    break;

                default:
                    break;
            }

            // expect a continuation or end of list statement
            m_qTokens.pop();
            if (_EofError()) return rootList;
            pToken = &m_qTokens.front();

            if (pToken->token.index() != JSONTokenIndex_Char) {
                m_errStream << "Unexpected token '" << pToken->token << "' at line " << pToken->uLine << '\n';
                m_qTokens.pop();
                return List();
            }

            if (std::get<char>(pToken->token) == ',') {
                m_qTokens.pop();
            }
            else if (std::get<char>(pToken->token) == ']') {
                m_qTokens.pop();
                stckList.pop();

                if (!stckList.empty()) {
                    if (_EofError()) return List();
                    pToken = &m_qTokens.front();

                    if (pToken->token.index() != JSONTokenIndex_Char) {
                        m_errStream << "Expected comma before line " << pToken->uLine << '\n';
                        return List();
                    } 
                    else if (std::get<char>(pToken->token) == ',') {
                        m_qTokens.pop();
                    }
                }
            }
            else {
                m_errStream << "Unexpected identifier '" << pToken->token << "' at line " << pToken->uLine << '\n';
                m_qTokens.pop();
                return List();
            }
        }

        return rootList;
    }


    void JSONUnserializer::_ParseObject(std::unordered_map<String, Value>* _pRootObject) {
        // optimization: using a stack for recursive objects instead of actual recursion 
        std::stack<std::unordered_map<String, Value>*> stckObjects;
        stckObjects.push(_pRootObject);

        bool bInitialDecl = false;

        while (!m_qTokens.empty() && !stckObjects.empty()) {
            JSONToken* pToken = &m_qTokens.front();
            auto pObject = stckObjects.top();

            // check for initial object declaration
            if (!bInitialDecl && pToken->token.index() == JSONTokenIndex_Char && std::get<char>(pToken->token) == '{') {
                m_qTokens.pop();
                if (_EofError()) return;
                pToken = &m_qTokens.front();
                bInitialDecl = true;
            } else if (!bInitialDecl) {
                m_errStream << "Unexpected identifier '" << pToken->token << "' at line " << pToken->uLine << '\n';
                m_qTokens.pop();
                return;
            }

            // check for end statement
            if (pToken->token.index() == JSONTokenIndex_Char && std::get<char>(pToken->token) == '}') {
                m_qTokens.pop();
                stckObjects.pop();

                if (!stckObjects.empty()) {
                    if (_EofError()) return;
                    pToken = &m_qTokens.front();

                    if (pToken->token.index() != JSONTokenIndex_Char) {
                        m_errStream << "Expected comma before line " << pToken->uLine << '\n';
                        return;
                    } 
                    else if (std::get<char>(pToken->token) == ',') {
                        m_qTokens.pop();
                    }
                }
                continue;
            }

            // expect a string key
            pToken = &m_qTokens.front();

            String sKey;
            if (pToken->token.index() != JSONTokenIndex_String) {
                m_errStream << "Unexpected identifier '" << pToken->token << "' at line " << pToken->uLine << 
                                ".\nExpected a json key!\n";
                m_qTokens.pop();
                return;
            } else {
                sKey = std::get<String>(pToken->token);
            }

            // expect a colon separator
            m_qTokens.pop();
            if (_EofError()) return;
            pToken = &m_qTokens.front();

            if (pToken->token.index() != JSONTokenIndex_Char || std::get<char>(pToken->token) != ':') {
                m_errStream << "Unexpected identifier '" << pToken->token << "' at line " << pToken->uLine <<
                               ".\nExpected a color (':')!\n";
                m_qTokens.pop();
                return;
            }

            // expect a value
            m_qTokens.pop();
            if (_EofError()) return;
            pToken = &m_qTokens.front();

            switch (pToken->token.index()) {
                case JSONTokenIndex_Char: 
                    // recursive object
                    if (std::get<char>(pToken->token) == '{') {
                        pObject->insert(std::make_pair(sKey, std::make_shared<Object>()));
                        stckObjects.push(&std::get<std::shared_ptr<Object>>(pObject->find(sKey)->second).get()->GetContents());
                        bInitialDecl = false;
                        continue;
                    } 
                    // list of objects
                    else if (std::get<char>(pToken->token) == '[') {
                        pObject->insert(std::make_pair(sKey, _ParseList()));
                    }
                    break;

                case JSONTokenIndex_String:
                    pObject->insert(std::make_pair(sKey, std::get<String>(pToken->token)));
                    m_qTokens.pop();
                    break;

                case JSONTokenIndex_Float:
                    pObject->insert(std::make_pair(sKey, std::get<Float>(pToken->token)));
                    m_qTokens.pop();
                    break;

                case JSONTokenIndex_Int:
                    pObject->insert(std::make_pair(sKey, std::get<Int>(pToken->token)));
                    m_qTokens.pop();
                    break;

                case JSONTokenIndex_Bool:
                    pObject->insert(std::make_pair(sKey, std::get<Bool>(pToken->token)));
                    m_qTokens.pop();
                    break;

                default:
                    m_qTokens.pop();
                    break;
            }

            if (_EofError()) return;
            pToken = &m_qTokens.front();

            // expect either a continuation identifier ',' or object end statement '}'
            if (pToken->token.index() != JSONTokenIndex_Char) {
                m_errStream << "Unexpected token '" << pToken->token << "' at line " << pToken->uLine << '\n';
                m_qTokens.pop();
                return;
            }

            if (std::get<char>(pToken->token) == ',') {
                m_qTokens.pop();
            }
            else if (std::get<char>(pToken->token) == '}') {
                m_qTokens.pop();
                stckObjects.pop();

                if (!stckObjects.empty()) {
                    if (_EofError()) return;
                    pToken = &m_qTokens.front();

                    if (pToken->token.index() != JSONTokenIndex_Char) {
                        m_errStream << "Expected comma before line " << pToken->uLine << '\n';
                        return;
                    } 
                    else if (std::get<char>(pToken->token) == ',') {
                        m_qTokens.pop();
                    }
                }
            }
            else {
                m_errStream << "Unexpected identifier '" << pToken->token << "' at line " << pToken->uLine << '\n';
                m_qTokens.pop();
                return;
            }
        }
    }
    

    void JSONUnserializer::_Parse() {
        bool bIsRoot = false;

        while (!m_qTokens.empty()) {
            JSONToken* pToken = &m_qTokens.front();

            if (!bIsRoot && (pToken->token.index() != JSONTokenIndex_Char || std::get<char>(pToken->token) != '{')) {
                m_errStream << "Root must be an object\n";
                return;
            } else if (!bIsRoot){
                bIsRoot = true;
                continue;
            }

            _ParseObject(&m_root);
        }
    }
}
