// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: JSONSerializer.h - JSON serializer class implementation
// author: Karl-Mihkel Ott

#include <cvar/JSONSerializer.h>
#include <stack>
#include <algorithm>
#include <iomanip>

namespace cvar {

    JSONSerializer::JSONSerializer(std::ostream& _stream, std::unordered_map<String, Value>& _root) :
        ISerializer(_stream, _root) {}


    void JSONSerializer::Serialize(bool _bBeautified) {
        if (_bBeautified)
            _SerializeBeautified();
        else _SerializeCompact();
    }

    void JSONSerializer::_SerializeCompact() {
        m_stream << '{';
        std::stack<std::pair<std::unordered_map<String, Value>*, std::unordered_map<String, Value>::iterator>> stckObjects;
        stckObjects.push(std::make_pair(&m_root, m_root.begin()));

        while (!stckObjects.empty()) {
            BEGIN:
            auto& top = stckObjects.top();

            for (auto it = top.second; it != top.first->end(); it++) {
                m_stream << '\"' << it->first << "\":";
                
                switch(it->second.index()) {
                    case Type_Int:
                        m_stream << std::get<Type_Int>(it->second);
                        break;

                    case Type_Float:
                        m_stream << std::get<Type_Float>(it->second);
                        break;

                    case Type_Bool:
                        m_stream << (std::get<Type_Bool>(it->second) ? "true" : "false");
                        break;

                    case Type_String:
                        m_stream << '\"' << std::get<Type_String>(it->second) << '\"';
                        break;

                    case Type_List:
                        m_stream << std::get<Type_List>(it->second);
                        break;

                    case Type_Object:
                        {
                            m_stream << "{";
                            auto& obj = std::get<Type_Object>(it->second)->GetContents();
                            it++;
                            top.second = it;
                            stckObjects.push(std::make_pair(&obj, obj.begin()));
                            goto BEGIN;
                        }
                        break;

                    default:
                        break;
                }

                if (std::next(it) != top.first->end())
                    m_stream << ",";
            }

            stckObjects.pop();
            
            if (stckObjects.size() >= 1 && stckObjects.top().second != stckObjects.top().first->end())
                m_stream << "},";
            else m_stream << '}';
        }
    }


    void JSONSerializer::_SerializeBeautified() {
        m_stream << "{\n";
        std::size_t uNTabs = 1;
        
        std::stack<std::pair<std::unordered_map<String, Value>*, std::unordered_map<String, Value>::iterator>> stckObjects;
        stckObjects.push(std::make_pair(&m_root, m_root.begin()));

        while (!stckObjects.empty()) {
            BEGIN:
            auto& top = stckObjects.top();

            for (auto it = top.second; it != top.first->end(); it++) {
                m_stream << std::setw(uNTabs) << std::setfill('\t') << "" << '\"' << it->first << "\": ";

                switch (it->second.index()) {
                    case Type_Int:
                        m_stream << std::get<Type_Int>(it->second);
                        break;

                    case Type_Float:
                        m_stream << std::get<Type_Float>(it->second);
                        break;

                    case Type_Bool:
                        m_stream << (std::get<Type_Bool>(it->second) ? "true" : "false");
                        break;

                    case Type_String:
                        m_stream << '\"' << std::get<Type_String>(it->second) << '\"';
                        break;

                    case Type_List:
                        m_stream << std::get<Type_List>(it->second);
                        break;

                    case Type_Object:
                        {
                            m_stream << "{\n";
                            uNTabs++;
                            auto& obj = std::get<Type_Object>(it->second)->GetContents();
                            it++;
                            top.second = it;
                            stckObjects.push(std::make_pair(&obj, obj.begin()));
                            goto BEGIN;
                        }
                        break;

                    default:
                        break;
                }

                if (std::next(it) == top.first->end())
                    m_stream << '\n';
                else m_stream << ",\n";
            }
            
            stckObjects.pop();
            uNTabs--;

            if (stckObjects.size() >= 1 && stckObjects.top().second != stckObjects.top().first->end())
                m_stream << std::setw(uNTabs) << std::setfill('\t') << "" << "},\n";
            else m_stream << std::setw(uNTabs) << std::setfill('\t') << "" << "}\n";

        }
    }
}
