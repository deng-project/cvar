// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONUnserializer.cpp - JSON unserializer class implementation
// author: Karl-Mihkel Ott

#include <iostream>
#include <cstring>
#include <cvar/JSONUnserializer.h>

namespace CVar {
    
    JSONUnserializer::JSONUnserializer(std::istream& _stream) :
        IUnserializer(_stream)
    {
        _Lex();
        // _Parse();
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
        while (!m_stream.eof() && m_stream.peek() <= '9' && m_stream.peek() >= '0')
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
        while (!m_stream.eof() && ((m_stream.peek() <= '9' && m_stream.peek() >= '0') || m_stream.peek() == '.'))
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
        while (!m_stream.eof()) {
            std::variant<std::monostate, String> jsonString = _LexString();
            if (jsonString.index()) {
                m_tokens.emplace_back(std::get<String>(jsonString));
                continue;
            }

            std::variant<std::monostate, Int> jsonInt = _LexInt();
            if (jsonInt.index()) {
                m_tokens.emplace_back(std::get<Int>(jsonInt));
                continue;
            }

            std::variant<std::monostate, Float> jsonFloat = _LexFloat();
            if (jsonFloat.index()) {
                m_tokens.emplace_back(std::get<Float>(jsonFloat));
                continue;
            }

            std::variant<std::monostate, Bool> jsonBool = _LexBool();
            if (jsonBool.index()) {
                m_tokens.emplace_back(std::get<Bool>(jsonBool));
                continue;
            }

            std::variant<std::monostate, JSONNull> jsonNull = _LexNull();
            if (jsonNull.index()) {
                m_tokens.push_back(JSONNull{});
                continue;
            }

            if (_Contains(m_stream.peek(), m_szJsonSyntax, sizeof(m_szJsonSyntax)))
                m_tokens.push_back(static_cast<char>(m_stream.get()));
            else if (_Contains(m_stream.peek(), m_szJsonWhitespace, sizeof(m_szJsonWhitespace)))
                static_cast<void>(m_stream.get());
            else {
                std::cerr << "Unexpected symbol '" << static_cast<char>(m_stream.peek()) << "'\n";
                break;
            }
        }

        // TESTING: print the output array
        std::cout << '[';
        for (auto it = m_tokens.begin(); it != m_tokens.end(); it++) {
            switch (it->index()) { 
                case 0: // char
                    std::cout << '\'' << std::get<char>(*it) << '\'';
                    break;

                case 1: // String
                    std::cout << '"' << std::get<String>(*it) << '"';
                    break;

                case 2: // Float
                    std::cout << std::get<Float>(*it);
                    break;

                case 3: // Int
                    std::cout << std::get<Int>(*it);
                    break;

                case 4: // Bool
                    std::cout << (std::get<Bool>(*it) ? "true" : "false");
                    break;

                case 5: // JSONNull
                    std::cout << "null";
                    break;

                default:
                    break;
            }

            if (std::next(it) == m_tokens.end()) {
                std::cout << "]\n";
            } else {
                std::cout << ", ";
            }
        }
    }
}
