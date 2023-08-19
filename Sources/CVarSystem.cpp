// CVar: Console variable systems support library
// license: Apache, see LICENCE file
// file: CVarSystem.h - CVar system class implementation file
// author: Karl-Mihkel Ott

#include <cvar/CVarSystem.h>
#include <queue>

namespace CVar {

    std::vector<String> CVarSystem::_HashKeyWords(const std::string& _key) {
		std::queue<std::string> qKeyWords;

		size_t uPos = 0;
		size_t uBeginPos = 0;

		const char cDelim = '.';
		while ((uPos = _key.find(cDelim, uBeginPos)) != std::string::npos) {
			qKeyWords.push(_key.substr(uBeginPos, uPos - uBeginPos));
			uBeginPos = uPos + 1;
		}

		if (uBeginPos != _key.size())
			qKeyWords.push(_key.substr(uBeginPos));

		std::vector<String> hashes;
		hashes.reserve(qKeyWords.size());

		while (!qKeyWords.empty()) {
			hashes.emplace_back(qKeyWords.front());
			qKeyWords.pop();
		}

		return hashes;
	}


	Value* CVarSystem::_FindNode(const std::string& _key) {
		auto hashes = _HashKeyWords(_key);
		Value* pNode = nullptr;
		std::unordered_map<String, Value>* pNodeTable = &m_root;

		for (size_t i = 0; i < hashes.size(); i++) {
			auto itNode = pNodeTable->find(hashes[i]);
			if (itNode != pNodeTable->end()) {
				pNode = &itNode->second;
				auto pObject = std::get_if<std::shared_ptr<Object>>(pNode);
				
				if (i != hashes.size() - 1 && !pObject)
					return nullptr;
				else if (i != hashes.size() - 1) 
					pNodeTable = &pObject->get()->GetContents();
			}
			else 
				return nullptr;
		}

		return pNode;
	}


	CVarSystem& CVarSystem::GetInstance() {
		static CVarSystem system;
		return system;
	}
}
