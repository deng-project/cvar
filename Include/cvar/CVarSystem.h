// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: CVarSystem.h - CVar system class header file
// author: Karl-Mihkel Ott

#pragma once

#include <cvar/Api.h>
#include <cvar/CVarTypes.h>
#include <fstream>

namespace cvar {

    class CVAR_API CVarSystem {
        private:
            std::unordered_map<String, Value> m_root;

        private:
            CVarSystem() = default;
            std::vector<String> _HashKeyWords(const std::string& _key);
            Value* _FindNode(const std::string& _key);

        public:
            static CVarSystem& GetInstance();

            template <typename T>
            void Serialize(const std::string& _sFileName, bool bBeautified = true) {
                std::ofstream stream(_sFileName);
                T serializer(stream, m_root);
                serializer.Serialize(bBeautified);
                stream.close();
            }


            template <typename T>
            void Unserialize(const std::string& _sFileName) {
                std::ifstream stream(_sFileName);
                T unserializer(stream);
                m_root = std::move(unserializer.Get());
            }

            inline auto& GetRoot() {
                return m_root;
            }

            inline Value* GetValue(const std::string& _key) {
                Value* pDesc = _FindNode(_key);
                if (pDesc)
                    return pDesc;
                return nullptr;
            }

            template <typename T>
            inline T* Get(const std::string& _key) {
                Value* pDesc = _FindNode(_key);
                if (pDesc)
                    return std::get_if<T>(pDesc);
                return nullptr;
            }

            template <typename T>
            bool Set(const String& _key, const T& _val) {
                std::vector<String> cvarStrings = _HashKeyWords(_key);
                std::unordered_map<String, Value>* pTable = &m_root;

                for (size_t i = 0; i < cvarStrings.size() - 1; i++) {
                    if (pTable->find(cvarStrings[i]) == pTable->end()) {
                        pTable->insert(std::make_pair(cvarStrings[i], std::make_shared<Object>()));
                    }
                    auto pObject = std::get_if<std::shared_ptr<Object>>(&pTable->find(cvarStrings[i])->second);
                    if (!pObject) 
                        return false;
                    pTable = &pObject->get()->GetContents();
                }

                pTable->insert(std::make_pair(cvarStrings.back(), _val));
                return true;
            }
    };
}
