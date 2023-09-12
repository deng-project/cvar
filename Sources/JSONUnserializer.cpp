// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONUnserializer.cpp - JSON unserializer class implementation
// author: Karl-Mihkel Ott

#include <iostream>
#include <cstring>
#include <sstream>
#include <cvar/JSONUnserializer.h>

namespace cvar {

    std::ostream& operator<<(std::ostream& _stream, std::variant<std::monostate, char, String, Float, Int, Bool, JSONNull>& _token) {
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

    
    JSONUnserializer::JSONUnserializer(std::istream& _stream) :
        IUnserializer(_stream)
    {
        _Parse();
    }


    std::optional<JSONNull> JSONUnserializer::_TokenizeNull() {
        char buf[sizeof("null")] = {};

        size_t i = 0;
        for (i = 0; i < sizeof("null")-1 && m_stream.peek() != -1; i++)
            buf[i] = m_stream.get();

        if (!std::strcmp(buf, "null"))
            return JSONNull{};

        for (size_t j = 0; j < i; j++)
            m_stream.unget();
        m_stream.clear();

        return std::nullopt;
    }


    bool JSONUnserializer::_NextToken() {
        m_token.token = std::monostate{};
        m_token.uLine = m_uLineCounter;

        if (m_stream.peek() == -1)
            return false;

        while (m_stream.peek() != -1 && _Contains(m_stream.peek(), m_szJsonWhitespace, sizeof(m_szJsonWhitespace))) {
            if (static_cast<char>(m_stream.peek()) == '\n')
                m_uLineCounter++;
            static_cast<void>(m_stream.get());
        }

        if (m_stream.peek() == -1)
            return false;

        if (_TryValueTokenization(_TokenizeString<String>()) || _TryValueTokenization(_TokenizeInt()) ||
            _TryValueTokenization(_TokenizeFloat()) || _TryValueTokenization(_TokenizeBool()) ||
            _TryValueTokenization(_TokenizeNull())) 
        {
            return true;
        }

        if (_Contains(m_stream.peek(), m_szJsonSyntax, sizeof(m_szJsonSyntax))) {
            m_token.token = static_cast<char>(m_stream.get());
            m_token.uLine = m_uLineCounter;
            return true;
        }
        else {
            std::stringstream ss;
            ss << "Unexpected symbol '" << static_cast<char>(m_stream.peek()) << "' at line " << m_uLineCounter;
            throw SyntaxErrorException(ss.str());
            return false;
        }
    }


    void JSONUnserializer::_NextTokenEx() {
        if (!_NextToken())
            throw UnexpectedEOFException("No more tokens to validate JSON syntax");
    }


    List JSONUnserializer::_ParseList() {
        List rootList;

        // pair specification:
        // first - pointer to list
        // second - boolean flag to indicate if the current value shall be continuation to some previous value
        std::stack<std::pair<List*, bool>> stckLists;
        stckLists.push(std::make_pair(&rootList, false));

        while (!stckLists.empty() && _NextToken()) {
            auto pList = stckLists.top().first;
            bool& bIsContinuation = stckLists.top().second;

            // check for list end statement
            if (m_token.token.index() == JSONTokenIndex_Char && std::get<char>(m_token.token) == ']') {
                stckLists.pop();
                continue;
            }

            // check if the current value is a continuation to some other value
            // if so then expect a comma ',' as the current token
            if (bIsContinuation && (m_token.token.index() == JSONTokenIndex_Char && std::get<char>(m_token.token) == ','))
                _NextTokenEx();
            else if (bIsContinuation) {
                std::stringstream ss;
                ss << "Expected a comma separator at line " << m_token.uLine;
                throw SyntaxErrorException(ss.str());
            } else {
                bIsContinuation = true;
            }

            // expect some kind of <valuetype>
            switch (m_token.token.index()) {
                case JSONTokenIndex_Char:
                    // recursive list (array)
                    if (std::get<char>(m_token.token) == '[') {
                        pList->PushBack(std::make_shared<List>());
                        stckLists.push(std::make_pair(std::get<std::shared_ptr<List>>(*pList->ReverseBegin()).get(), false));
                        continue;
                    }
                    // object inside a list
                    else if (std::get<char>(m_token.token) == '{') {
                        pList->PushBack(std::make_shared<Object>());
                        _ParseObject(&std::get<std::shared_ptr<Object>>(*pList->ReverseBegin()).get()->GetContents());
                    }
                    // error otherwise
                    else {
                        std::stringstream ss;
                        ss << "Unexpected identifier '" << std::get<char>(m_token.token) << "'. Expected a valuetype instead.";
                        throw SyntaxErrorException(ss.str());
                    }
                    break;

                case JSONTokenIndex_String:
                    pList->PushBack(std::get<String>(m_token.token));
                    break;

                case JSONTokenIndex_Float:
                    pList->PushBack(std::get<Float>(m_token.token));
                    break;

                case JSONTokenIndex_Int:
                    pList->PushBack(std::get<Int>(m_token.token));
                    break;

                case JSONTokenIndex_Bool:
                    pList->PushBack(std::get<Bool>(m_token.token));
                    break;

                default:
                    break;
            }
        }

        return rootList;
    }


    void JSONUnserializer::_ParseObject(std::unordered_map<String, Value>* _pRootObject) {
        // optimization: using a stack for recursive objects instead of actual recursion 
        // pair specification:
        // first - pointer to object (aka unordered_map)
        // second - boolean flag to indicate if the current value shall be a continuation to some previous value
        std::stack<std::pair<std::unordered_map<String, Value>*, bool>> stckObjects;
        stckObjects.push(std::make_pair(_pRootObject, false));

        while (!stckObjects.empty() && _NextToken()) {
            auto pObject = stckObjects.top().first;
            bool& bIsContinuation = stckObjects.top().second;

            // check for end statement
            if (m_token.token.index() == JSONTokenIndex_Char && std::get<char>(m_token.token) == '}') {
                stckObjects.pop();
                continue;
            }

            // check if the current value is a continuation to some other value 
            // if so then expect a comma ',' as the current token
            if (bIsContinuation && (m_token.token.index() == JSONTokenIndex_Char && std::get<char>(m_token.token) == ',')) {
                _NextTokenEx();
            } else if (bIsContinuation) {
                std::stringstream ss;
                ss << "Expected a comma separator at line " << m_token.uLine;
                throw SyntaxErrorException(ss.str());
            } else {
                bIsContinuation = true;
            }

            // expect a string key
            String sKey;
            if (m_token.token.index() != JSONTokenIndex_String) {
                std::stringstream ss;
                ss << "Unexpected identifier '" << m_token.token << "' at line " << m_token.uLine << 
                                ". Expected a json key!";
                throw SyntaxErrorException(ss.str());
            } else {
                sKey = std::get<String>(m_token.token);
            }

            // expect a colon separator
            _NextTokenEx();

            if (m_token.token.index() != JSONTokenIndex_Char || std::get<char>(m_token.token) != ':') {
                std::stringstream ss;
                ss << "Unexpected identifier '" << m_token.token << "' at line " << m_token.uLine << ". Expected a separator color (':')!";
                throw SyntaxErrorException(ss.str());
            }

            // expect a value
            _NextTokenEx();

            switch (m_token.token.index()) {
                case JSONTokenIndex_Char: 
                    // recursive object
                    if (std::get<char>(m_token.token) == '{') {
                        pObject->insert(std::make_pair(sKey, std::make_shared<Object>()));
                        stckObjects.push(std::make_pair(&std::get<std::shared_ptr<Object>>(pObject->find(sKey)->second).get()->GetContents(), false));
                        continue;
                    } 
                    // array of objects
                    else if (std::get<char>(m_token.token) == '[') {
                        pObject->insert(std::make_pair(sKey, _ParseList()));
                    }
                    // error
                    else {
                        std::stringstream ss;
                        ss << "Unexpected identifier '" << std::get<char>(m_token.token) << "'. Expected a valuetype instead.";
                        throw SyntaxErrorException(ss.str());
                    }
                    break;

                case JSONTokenIndex_String:
                    pObject->insert(std::make_pair(sKey, std::get<String>(m_token.token)));
                    break;

                case JSONTokenIndex_Float:
                    pObject->insert(std::make_pair(sKey, std::get<Float>(m_token.token)));
                    break;

                case JSONTokenIndex_Int:
                    pObject->insert(std::make_pair(sKey, std::get<Int>(m_token.token)));
                    break;

                case JSONTokenIndex_Bool:
                    pObject->insert(std::make_pair(sKey, std::get<Bool>(m_token.token)));
                    break;

                default:
                    break;
            }
        }
    }
    

    void JSONUnserializer::_Parse() {
        // assume empty file is used (no exceptions thrown) 
        if (!_NextToken()) return;
        
        if (m_token.token.index() != JSONTokenIndex_Char || std::get<char>(m_token.token) != '{') {
            throw SyntaxErrorException("Root must always be an object");
        } 

        _ParseObject(&m_root);
    }
}
