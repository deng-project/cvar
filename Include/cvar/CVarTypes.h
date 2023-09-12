// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: CVarTypes.h - CVar types definition header file
// author: Karl-Mihkel Ott

#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cvar/SID.h>

namespace cvar {

    class String {
        private:
            std::string m_string;
            hash_t m_hshString = 0;  

        public:
            String() = default;
            String(const std::string& _str) :
                m_string(_str),
                m_hshString(RUNTIME_CRC(m_string)) {}
            String(const char* _szString) :
                m_string(_szString),
                m_hshString(RUNTIME_CRC(m_string)) {}
            String(const String&) = default;
            String(String&&) = default;

            operator std::string&() { return m_string; }
            operator const std::string& () const { return m_string; }

            inline bool operator==(const String& _str) const {
                return m_hshString == _str.GetHash();
            }

            inline void operator=(const std::string& _str) {
                m_string = _str;
                m_hshString = RUNTIME_CRC(m_string);
            }

            inline void operator=(const char* _szString) {
                m_string = _szString;
                m_hshString = RUNTIME_CRC(m_string);
            }

            inline void operator=(const String& _str) {
                m_string = _str.GetSTDString();
                m_hshString = _str.GetHash();
            }

            inline const std::string& GetSTDString() const { return m_string; }
            inline const hash_t GetHash() const { return m_hshString; }
    };

    inline std::ostream& operator<<(std::ostream& _stream, const String& _str) {
        _stream << _str.GetSTDString();
        return _stream;
    }
}

// HACK: create std::hash<String> definitions so that the hash wouldn't get calculated
template <>
struct std::hash<cvar::String> {
    inline std::size_t operator()(const cvar::String& _str) const {
        return _str.GetHash();
    }
};

namespace cvar {
    typedef int32_t Int;
    typedef float Float;
    typedef bool Bool;

    class List;
    class Object;
    typedef std::variant<std::monostate, Int, Float, Bool, String, std::shared_ptr<List>, std::shared_ptr<Object>> ListItem;

    class List {
        private:
            std::vector<ListItem> m_items;

        public:
            List() = default;
            List(const List& _list) :
                m_items(_list.m_items) {}
            List(List&& _list) :
                m_items(std::move(_list.m_items)) {}
            List(std::initializer_list<ListItem> _initList) {
                m_items.reserve(_initList.size());
                for (auto it = _initList.begin(); it != _initList.end(); it++) {
                    m_items.push_back(*it);
                }
            }

            template <typename T>
            inline void PushBack(const T& _val) { m_items.push_back(_val); }
            inline std::size_t Size() const { return m_items.size(); }
            inline auto Begin() const { return m_items.begin(); };
            inline auto End() const { return m_items.end(); };
            inline auto ReverseBegin() const { return m_items.rbegin(); }
            inline auto ReverseEnd() const { return m_items.rend(); }
    };

    typedef std::variant<std::monostate, Int, Float, Bool, String, List, std::shared_ptr<Object>> Value;

    class Object {
        private:
            using _Contents = std::unordered_map<String, Value>;
            _Contents m_contents;

        public:
            Object() = default;

            inline void PushNode(const String& _key, const Value& _val) {
                m_contents.emplace(std::make_pair(_key, _val));
            }

            inline _Contents& GetContents() { return m_contents; }
            inline const _Contents& GetContents() const { return m_contents; }
    };

    enum Type : size_t {
        Type_None,
        Type_Int,
        Type_Float,
        Type_Bool,
        Type_String,
        Type_List,
        Type_Object
    };

    // list and object stream serializers
    std::ostream& operator<<(std::ostream& _stream, List& _list);
    std::ostream& operator<<(std::ostream& _stream, Object& _obj);
}
