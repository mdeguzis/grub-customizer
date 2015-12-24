/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "Proxy.h"

Model_Proxy::Model_Proxy()
	: dataSource(NULL), permissions(0755), index(90)
{	
}

Model_Proxy::Model_Proxy(Model_Script& dataSource, bool activateRules)
	: dataSource(&dataSource), permissions(0755), index(90)
{
	rules.push_back(Model_Rule(Model_Rule::OTHER_ENTRIES_PLACEHOLDER, std::list<std::string>(), "*", activateRules));
	sync(true, true);
}

std::list<Model_Rule> Model_Proxy::parseRuleString(const char** ruleString, std::string const& cfgDirPrefix) {
	std::list<Model_Rule> rules;

	bool inString = false, inAlias = false, inHash = false, inFromClause = false;
	std::string name;
	std::string hash;
	std::list<std::string> path;
	bool visible = false;
	const char* iter = NULL;
	for (iter = *ruleString; *iter && (*iter != '}' || inString || inAlias || inFromClause); iter++) {
		if (!inString && *iter == '+') {
			visible = true;
		} else if (!inString && *iter == '-') {
			visible = false;
		} else if (*iter == '\'' && iter[1] != '\'') {
			inString = !inString;
			if (iter[1] != '/') {
				if (!inString){
					if (inAlias) {
						rules.back().outputName = name;
					} else if (inFromClause) {
						rules.back().__sourceScriptPath = cfgDirPrefix + name;
					} else {
						path.push_back(name);
						rules.push_back(Model_Rule(Model_Rule::NORMAL, path, visible));
						path.clear();
					}
					inAlias = false;
					inFromClause = false;
				}
				name = "";
			}
		} else if (!inString && *iter == '*') {
			rules.push_back(Model_Rule(Model_Rule::OTHER_ENTRIES_PLACEHOLDER, path, "*", visible));
			path.clear();
		} else if (!inString && *iter == '#' && *++iter == 't' && *++iter == 'e' && *++iter == 'x' && *++iter == 't') {
			path.push_back("#text");
			rules.push_back(Model_Rule(Model_Rule::PLAINTEXT, path, "#text", visible));
			path.clear();
			name = "";
		} else if (inString) {
			name += *iter;
			if (*iter == '\'')
				iter++;
		} else if (inHash && *iter != '~') {
			hash += *iter;
		} else if (!inString && !inAlias && !inFromClause && *iter == 'a' && *++iter == 's') {
			inAlias = true;
		} else if (!inString && !inAlias && !inFromClause && *iter == 'f' && *++iter == 'r' && *++iter == 'o' && *++iter == 'm') {
			inFromClause = true;
		} else if (!inString && !inAlias && !inFromClause && *iter == '/') {
			path.push_back(name);
			name = "";
		} else if (!inString && !inAlias && !inFromClause && *iter == '{') {
			iter++;
			rules.back().subRules = Model_Proxy::parseRuleString(&iter, cfgDirPrefix);
			rules.back().type = Model_Rule::SUBMENU;
		} else if (!inString && *iter == '~') {
			inHash = !inHash;
			if (!inHash) {
				rules.back().__idHash = hash;
				hash = "";
			}
		}
	}
	*ruleString = iter;
	return rules;
}

void Model_Proxy::importRuleString(const char* ruleString, std::string const& cfgDirPrefix){
	rules = Model_Proxy::parseRuleString(&ruleString, cfgDirPrefix);
}

Model_Rule* Model_Proxy::getRuleByEntry(Model_Entry const& entry, std::list<Model_Rule>& list, Model_Rule::RuleType ruletype) {
	for (std::list<Model_Rule>::iterator rule_iter = list.begin(); rule_iter != list.end(); rule_iter++){
		if (&entry == rule_iter->dataSource && rule_iter->type == ruletype)
			return &*rule_iter;
		else {
			Model_Rule* result = this->getRuleByEntry(entry, rule_iter->subRules, ruletype);
			if (result)
				return result;
		}
	}
	return NULL;
}

std::list<Model_Rule*> Model_Proxy::getForeignRules(Model_Rule* parent) {
	assert(this->dataSource != NULL);

	std::list<Model_Rule*> result;
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;

	for (std::list<Model_Rule>::iterator ruleIter = list.begin(); ruleIter != list.end(); ruleIter++) {
		if (ruleIter->dataSource && !this->dataSource->hasEntry(*ruleIter->dataSource)) {
			result.push_back(&*ruleIter);
		}
		if (ruleIter->subRules.size()) {
			std::list<Model_Rule*> subResult = this->getForeignRules(&*ruleIter);
			result.splice(result.end(), subResult);
		}
	}
	return result;
}

void Model_Proxy::unsync(Model_Rule* parent) {
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		iter->dataSource = NULL;
		if (iter->subRules.size()) {
			this->unsync(&*iter);
		}
	}
}

bool Model_Proxy::sync(bool deleteInvalidRules, bool expand, std::map<std::string, Model_Script*> scriptMap){
	if (this->dataSource){
		this->sync_connectExisting(NULL, scriptMap);
		this->sync_connectExistingByHash(NULL, scriptMap);
		if (expand) {
			this->sync_add_placeholders(NULL, scriptMap);
			this->sync_expand(scriptMap);
		}

		if (deleteInvalidRules)
			this->sync_cleanup(NULL, scriptMap);

		return true;
	}
	else
		return false;
}

void Model_Proxy::sync_connectExisting(Model_Rule* parent, std::map<std::string, Model_Script*> scriptMap) {
	assert(this->dataSource != NULL);
	if (parent == NULL) {
		this->__idPathList.clear();
		this->__idPathList_OtherEntriesPlaceHolders.clear();
	}
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->type != Model_Rule::SUBMENU) { // don't sync submenu entries
			std::list<std::string> path = iter->__idpath;

			Model_Script* script = NULL;
			if (iter->__sourceScriptPath == "") { // main dataSource
				script = this->dataSource;
			} else if (scriptMap.size()) {
				assert(scriptMap.find(iter->__sourceScriptPath) != scriptMap.end()); // expecting that the script exists on the map
				script = scriptMap[iter->__sourceScriptPath];
			} else {
				continue; // don't sync foreign entries if scriptMap is empty
			}

			if (iter->type != Model_Rule::OTHER_ENTRIES_PLACEHOLDER) {
				this->__idPathList[script].push_back(path);
			} else {
				this->__idPathList_OtherEntriesPlaceHolders[script].push_back(path);
			}

			iter->dataSource = script->getEntryByPath(path);

		} else if (iter->subRules.size()) {
			this->sync_connectExisting(&*iter, scriptMap);
		}
	}
}

void Model_Proxy::sync_connectExistingByHash(Model_Rule* parent, std::map<std::string, Model_Script*> scriptMap) {
	assert(this->dataSource != NULL);
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource == NULL && iter->__idHash != "") {
			Model_Script* script = NULL;
			if (iter->__sourceScriptPath == "") {
				script = this->dataSource;
			} else if (scriptMap.size()) {
				assert(scriptMap.find(iter->__sourceScriptPath) != scriptMap.end()); // expecting that the script exists on the map
				script = scriptMap[iter->__sourceScriptPath];
			} else {
				continue; // don't sync foreign entries if scriptMap is empty
			}
			iter->dataSource = script->getEntryByHash(iter->__idHash, script->entries());
			if (iter->dataSource) {
				this->__idPathList[script].push_back(script->buildPath(*iter->dataSource));
			}
		}
		if (iter->subRules.size()) {
			this->sync_connectExistingByHash(&*iter, scriptMap);
		}
	}
}

void Model_Proxy::sync_add_placeholders(Model_Rule* parent, std::map<std::string, Model_Script*> scriptMap) {
	assert(parent == NULL || parent->dataSource != NULL);

	std::list<std::string> path = parent ? this->dataSource->buildPath(*parent->dataSource) : std::list<std::string>();
	//find out if currentPath is on the blacklist
	bool eop_is_blacklisted = false;

	for (std::list<std::list<std::string> >::const_iterator pti_iter = this->__idPathList_OtherEntriesPlaceHolders[this->dataSource].begin(); pti_iter != this->__idPathList_OtherEntriesPlaceHolders[this->dataSource].end(); pti_iter++) {
		if (*pti_iter == path) {
			eop_is_blacklisted = true;
			break;
		}
	}

	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;
	if (!eop_is_blacklisted) {
		Model_Rule newRule(Model_Rule::OTHER_ENTRIES_PLACEHOLDER, parent ? this->dataSource->buildPath(*parent->dataSource) : std::list<std::string>(), "*", true);
		newRule.dataSource = this->dataSource->getEntryByPath(path);
		list.push_front(newRule);
		this->__idPathList_OtherEntriesPlaceHolders[this->dataSource].push_back(path);
	}

	//sub entries (recursion)
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource && iter->type == Model_Rule::SUBMENU) {
			this->sync_add_placeholders(&*iter);
		}
	}
}


void Model_Proxy::sync_expand(std::map<std::string, Model_Script*> scriptMap) {
	assert(this->dataSource != NULL);
	for (std::map<Model_Script*, std::list<std::list<std::string> > >::iterator scriptIter = this->__idPathList_OtherEntriesPlaceHolders.begin(); scriptIter != this->__idPathList_OtherEntriesPlaceHolders.end(); scriptIter++) {
		for (std::list<std::list<std::string> >::iterator oepPathIter = this->__idPathList_OtherEntriesPlaceHolders[scriptIter->first].begin(); oepPathIter != this->__idPathList_OtherEntriesPlaceHolders[scriptIter->first].end(); oepPathIter++) {
			Model_Entry* dataSource = scriptIter->first->getEntryByPath(*oepPathIter);
			if (dataSource) {
				Model_Rule* oep = this->getRuleByEntry(*dataSource, this->rules, Model_Rule::OTHER_ENTRIES_PLACEHOLDER);
				assert(oep != NULL);
				Model_Rule* parentRule = this->getParentRule(oep);
				std::list<Model_Rule>& dataTarget = parentRule ? parentRule->subRules : this->rules;

				std::list<Model_Rule>::iterator dataTargetIter = dataTarget.begin();
				while (dataTargetIter != dataTarget.end()
					&& !(dataTargetIter->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER
						&& dataTargetIter->__idpath == *oepPathIter
						&& ((dataTargetIter->__sourceScriptPath != ""
								&& scriptMap.size()
								&& scriptMap[dataTargetIter->__sourceScriptPath] == scriptIter->first)
							|| (dataTargetIter->__sourceScriptPath == ""
								&& scriptIter->first == this->dataSource)
							)
						)
					) {
					dataTargetIter++;
				}
				std::list<Model_Rule> newRules;
				for (std::list<Model_Entry>::iterator iter = dataSource->subEntries.begin(); iter != dataSource->subEntries.end(); iter++){
					Model_Rule* relatedRule = this->getRuleByEntry(*iter, this->rules, Model_Rule::NORMAL);
					Model_Rule* relatedRulePt = this->getRuleByEntry(*iter, this->rules, Model_Rule::PLAINTEXT);
					Model_Rule* relatedRuleOep = this->getRuleByEntry(*iter, this->rules, Model_Rule::OTHER_ENTRIES_PLACEHOLDER);
					if (!relatedRule && !relatedRuleOep && !relatedRulePt){
						newRules.push_back(Model_Rule(*iter, dataTargetIter->isVisible, *scriptIter->first, this->__idPathList[scriptIter->first], scriptIter->first->buildPath(*iter))); //generate rule for given entry
					}
				}
				dataTargetIter++;
				dataTarget.splice(dataTargetIter, newRules);
			}
		}
	}
}

void Model_Proxy::sync_cleanup(Model_Rule* parent, std::map<std::string, Model_Script*> scriptMap) {
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;

	bool done = false;
	do {
		bool listModified = false;
		for (std::list<Model_Rule>::iterator iter = list.begin(); !listModified && iter != list.end(); iter++) {
			if (!((iter->type == Model_Rule::NORMAL && iter->dataSource) ||
				  (iter->type == Model_Rule::SUBMENU && iter->subRules.size()) ||
				  (iter->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER && iter->dataSource) ||
				  (iter->type == Model_Rule::PLAINTEXT && iter->dataSource))) {
				if (iter->__sourceScriptPath == "" || scriptMap.size()) {
					list.erase(iter);
					listModified = true; //after ereasing something we have to create a new iterator
				}
			} else { //check contents
				this->sync_cleanup(&*iter, scriptMap);
			}
		}

		if (!listModified)
			done = true;
	} while (!done);
}

bool Model_Proxy::isModified(Model_Rule const* parentRule, Model_Entry const* parentEntry) const {
	assert(this->dataSource != NULL);
	bool result = false;

	std::list<Model_Rule> const& rlist = parentRule ? parentRule->subRules : this->rules;
	std::list<Model_Entry> const& elist = parentEntry ? parentEntry->subEntries : this->dataSource->entries();
	if (rlist.size()-1 == elist.size()){ //rules contains the other entries placeholder, so there is one more entry
		std::list<Model_Rule>::const_iterator ruleIter = rlist.begin();
		std::list<Model_Entry>::const_iterator entryIter = elist.begin();
		if (ruleIter->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER){ //the first element is the OTHER_ENTRIES_PLACEHOLDER by default.
			result = !ruleIter->isVisible; //If not visible, it's modified…
			ruleIter++;
		} else {
			result = true;
		}
		while (!result && ruleIter != rlist.end() && entryIter != elist.end()){
			// type compare
			if ((ruleIter->type == Model_Rule::NORMAL && entryIter->type != Model_Entry::MENUENTRY)
				|| (ruleIter->type == Model_Rule::PLAINTEXT && entryIter->type != Model_Entry::PLAINTEXT)
				|| (ruleIter->type == Model_Rule::SUBMENU && entryIter->type != Model_Entry::SUBMENU)) {
				result = true;
			} else if (ruleIter->outputName != entryIter->name || !ruleIter->isVisible) { // data compare
				result = true;
			} else if (ruleIter->type == Model_Rule::SUBMENU) { // submenu check
				result = this->isModified(&*ruleIter, &*entryIter);
			}

			ruleIter++;
			entryIter++;
		}
	} else {
		result = true;
	}
	return result;
}

bool Model_Proxy::deleteFile(){
	assert(Model_ProxyScriptData::is_proxyscript(this->fileName));
	int success = unlink(this->fileName.c_str());
	if (success == 0){
		this->fileName = "";
		return true;
	}
	else
		return false;
}

std::list<std::string> Model_Proxy::getScriptList(std::map<Model_Entry const*, Model_Script const*> const& entrySourceMap, std::map<Model_Script const*, std::string> const& scriptTargetMap) const {
	std::map<std::string, Nothing> uniqueList; // the pointer (value) is just a dummy
	for (std::map<Model_Entry const*, Model_Script const*>::const_iterator iter = entrySourceMap.begin(); iter != entrySourceMap.end(); iter++) {
		uniqueList[scriptTargetMap.find(iter->second)->second] = Nothing();
	}
	std::list<std::string> result;
	result.push_back(scriptTargetMap.find(this->dataSource)->second); // the own script must be the first entry
	for (std::map<std::string, Nothing>::iterator iter = uniqueList.begin(); iter != uniqueList.end(); iter++) {
		result.push_back(iter->first);
	}
	return result;
}

bool Model_Proxy::generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix, std::map<Model_Entry const*, Model_Script const*> entrySourceMap, std::map<Model_Script const*, std::string> const& scriptTargetMap){
	if (this->dataSource){
		FILE* proxyFile = fopen(path.c_str(), "w");
		if (proxyFile){
			this->fileName = path;
			fputs("#!/bin/sh\n#THIS IS A GRUB PROXY SCRIPT\n", proxyFile);
			std::list<std::string> scripts = this->getScriptList(entrySourceMap, scriptTargetMap);
			if (scripts.size() == 1) { // single script
				fputs(("'"+this->dataSource->fileName.substr(cfg_dir_prefix_length)+"'").c_str(), proxyFile);
			} else { // multi script
				fputs("sh -c '", proxyFile);
				for (std::list<std::string>::iterator scriptIter = scripts.begin(); scriptIter != scripts.end(); scriptIter++) {
					fputs(("echo \"### BEGIN "+(*scriptIter).substr(cfg_dir_prefix_length)+" ###\";\n").c_str(), proxyFile);
					fputs(("\""+(*scriptIter).substr(cfg_dir_prefix_length)+"\";\n").c_str(), proxyFile);
					fputs(("echo \"### END "+(*scriptIter).substr(cfg_dir_prefix_length)+" ###\";").c_str(), proxyFile);
					if (&*scriptIter != &scripts.back()) {
						fputs("\n", proxyFile);
					}
				}
				fputs("'", proxyFile);
			}
			fputs((" | "+cfg_dir_noprefix+"/bin/grubcfg_proxy \"").c_str(), proxyFile);
			for (std::list<Model_Rule>::iterator ruleIter = this->rules.begin(); ruleIter != this->rules.end(); ruleIter++){
				assert(this->dataSource != NULL);
				Model_EntryPathBuilderImpl entryPathBuilder(*this->dataSource);
				entryPathBuilder.setScriptTargetMap(scriptTargetMap);
				entryPathBuilder.setEntrySourceMap(entrySourceMap);
				entryPathBuilder.setPrefixLength(cfg_dir_prefix_length);
				fputs((ruleIter->toString(entryPathBuilder)+"\n").c_str(), proxyFile); //write rule
			}
			fputs("\"", proxyFile);
			if (scripts.size() > 1) {
				fputs(" multi", proxyFile);
			}
			fclose(proxyFile);
			chmod(path.c_str(), this->permissions);
			return true;
		}
	}
	return false;
}

bool Model_Proxy::isExecutable() const {
	return this->permissions & 0111;
}


void Model_Proxy::set_isExecutable(bool value){
	if (value)
		permissions |= 0111;
	else
		permissions &= ~0111;
}

std::string Model_Proxy::getScriptName(){
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}


Model_Rule& Model_Proxy::moveRule(Model_Rule* rule, int direction) {
	std::list<Model_Rule>& ruleList = this->getRuleList(this->getParentRule(rule));

	std::list<Model_Rule>::iterator el = this->getListIterator(*rule, ruleList);
	Model_Rule* parent = NULL;
	try {
		parent = this->getParentRule(rule);
	} catch (ItemNotFoundException const& e) {} // leave parent in NULL state
	std::list<Model_Rule>& sourceList = this->getRuleList(parent);


	std::list<Model_Rule>::iterator next = this->rules.end();

	bool needToGoUp = false;
	try {
		next = this->getNextVisibleRule(el, direction);
	} catch (NoMoveTargetException const& e) {
		if (&sourceList != &this->rules) {
			needToGoUp = true;
		} else {
			throw e;
		}
	}

	try {
		parent = this->getParentRule(&*next);
	} catch (ItemNotFoundException const& e) {} // leave parent in NULL state
	std::list<Model_Rule>* targetList = &this->getRuleList(parent);

	Model_Rule* newRule = rule;

	std::list<Model_Rule>::iterator insertPos = next;

	if (needToGoUp) {
		Model_Rule* parentSubmenu = this->getParentRule(rule);
		targetList = &this->getRuleList(this->getParentRule(parentSubmenu));
		insertPos = this->getListIterator(*parentSubmenu, *targetList);
		if (direction == 1) {
			insertPos++;
		}
	} else if (next->type == Model_Rule::SUBMENU) {
		targetList = &next->subRules;
		insertPos = direction == -1 ? targetList->end() : targetList->begin();
	} else {
		if (direction == 1) {
			insertPos++;
		}
	}

	if (targetList == &this->rules && rule->dataSource && !this->ruleIsFromOwnScript(*rule)) {
		throw MustBeProxyException("this rule must be placed into a new proxy instead of moving internally", __FILE__, __LINE__);
	}

	newRule = &*targetList->insert(insertPos, *rule);

	if (ruleList.size() == 1) {
		// delete parent list if empty
		Model_Rule* parent = this->getParentRule(rule);
		std::list<Model_Rule>& parentOfParent = this->getRuleList(this->getParentRule(this->getParentRule(rule)));
		parentOfParent.erase(this->getListIterator(*parent, parentOfParent));
	} else {
		ruleList.erase(this->getListIterator(*rule, ruleList));
	}


	return *newRule;
}

void Model_Proxy::merge(Model_Proxy const& foreignProxy, int direction) {
	if (direction == -1) {
		for (std::list<Model_Rule>::const_iterator iter = foreignProxy.rules.begin(); iter != foreignProxy.rules.end(); iter++) {
			if (iter->isVisible) {
				this->removeEquivalentRules(*iter);
				this->rules.push_back(*iter);
			}
		}
	} else {
		for (std::list<Model_Rule>::const_reverse_iterator iter = foreignProxy.rules.rbegin(); iter != foreignProxy.rules.rend(); iter++) {
			if (iter->isVisible) {
				this->removeEquivalentRules(*iter);
				this->rules.push_front(*iter);
			}
		}
	}
}

std::list<Model_Rule>::iterator Model_Proxy::getNextVisibleRule(std::list<Model_Rule>::iterator base, int direction) {
	assert(direction == -1 || direction == 1);
	Model_Rule* parent = NULL;
	try {
		parent = this->getParentRule(&*base);
	} catch (ItemNotFoundException const& e) {} // leave parent in NULL state
	std::list<Model_Rule>* list = &this->getRuleList(parent);

	do {
		if (direction == 1) {
			base++;
		} else {
			base--;
		}
	} while (!base->isVisible && base != list->end());
	if (base == list->end()) {
		throw NoMoveTargetException("no move target found inside of this proxy", __FILE__, __LINE__);
	}
	return base;
}

Model_Rule* Model_Proxy::splitSubmenu(Model_Rule* position) {
	Model_Rule* parent = this->getParentRule(position);

	// search items before and after the submenu
	std::list<Model_Rule> rulesBefore;
	std::list<Model_Rule> rulesAfter;

	bool isBehindChildtem = false;
	for (std::list<Model_Rule>::iterator iter = parent->subRules.begin(); iter != parent->subRules.end(); iter++) {
		if (&*iter != position) {
			if (!isBehindChildtem) {
				rulesBefore.push_back(*iter);
			} else {
				rulesAfter.push_back(*iter);
			}
		} else {
			rulesAfter.push_back(*iter);
			isBehindChildtem = true;
		}
	}
	Model_Rule oldSubmenu = *parent;
	oldSubmenu.subRules.clear();

	std::list<Model_Rule>* list = NULL;
	Model_Rule* parentRule = this->getParentRule(parent);
	if (parentRule) {
		list = &parentRule->subRules;
	} else {
		list = &this->rules;
	}
	assert(list != NULL);
	// add the rules before and/or after to new submenus
	if (rulesBefore.size()) {
		Model_Rule newSubmenu = oldSubmenu;
		newSubmenu.subRules = rulesBefore;
		list->insert(this->getListIterator(*parent, *list), newSubmenu);
	}


	Model_Rule newSubmenu = oldSubmenu;
	newSubmenu.subRules = rulesAfter;
	std::list<Model_Rule>::iterator iter = this->getListIterator(*parent, *list);
	iter++;
	std::list<Model_Rule>::iterator insertPos = list->insert(iter, newSubmenu);

	// remove the submenu
	list->erase(this->getListIterator(*parent, *list));

	return &insertPos->subRules.front();
}

Model_Rule* Model_Proxy::createSubmenu(Model_Rule* position) {
	std::list<Model_Rule>& list = this->getRuleList(this->getParentRule(position));
	std::list<Model_Rule>::iterator posIter = this->getListIterator(*position, list);

	std::list<Model_Rule>::iterator insertPos = list.insert(posIter, Model_Rule(Model_Rule::SUBMENU, std::list<std::string>(), "", true));

	return &*insertPos;
}

bool Model_Proxy::ruleIsFromOwnScript(Model_Rule const& rule) const {
	assert(this->dataSource != NULL);
	assert(rule.dataSource != NULL);
	if (this->dataSource->hasEntry(*rule.dataSource)) {
		return true;
	} else {
		return false;
	}
}

Model_Rule* Model_Proxy::getParentRule(Model_Rule* child, Model_Rule* root) {
	std::list<Model_Rule>& list = root ? root->subRules : this->rules;
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (&*iter == child)
			return root;
		else if (iter->subRules.size()) {
			Model_Rule* parentRule = NULL;
			try {
				parentRule = this->getParentRule(child, &*iter);
			} catch (ItemNotFoundException const& e) {
				// do nothing
			}
			if (parentRule) {
				return parentRule;
			}
		}
	}
	throw ItemNotFoundException("specified rule not found", __FILE__, __LINE__);
}

std::list<Model_Rule>::iterator Model_Proxy::getListIterator(Model_Rule const& needle, std::list<Model_Rule>& haystack) {
	for (std::list<Model_Rule>::iterator iter = haystack.begin(); iter != haystack.end(); iter++) {
		if (&*iter == &needle)
			return iter;
	}

	throw ItemNotFoundException("specified rule not found", __FILE__, __LINE__);
}

std::list<Model_Rule>& Model_Proxy::getRuleList(Model_Rule* parentElement) {
	if (parentElement)
		return parentElement->subRules;
	else
		return this->rules;
}

void Model_Proxy::adjustIterator(std::list<Model_Rule>::iterator& iter, int adjustment) {
	if (adjustment > 0) {
		for (int i = 0; i < adjustment; i++) {
			iter++;
		}
	} else {
		for (int i = 0; i > adjustment; i--) {
			iter--;
		}
	}
}

void Model_Proxy::removeForeignChildRules(Model_Rule& parent) {
	bool loopRestartRequired = false;
	do { // required to restart the loop after an entry has been removed
		loopRestartRequired = false;
		for (std::list<Model_Rule>::iterator iter = parent.subRules.begin(); iter != parent.subRules.end(); iter++) {
			if (iter->dataSource) {
				if (!this->ruleIsFromOwnScript(*iter)) {
					parent.subRules.erase(iter);
					loopRestartRequired = true;
					break;
				}
			} else if (iter->subRules.size()) {
				this->removeForeignChildRules(*iter);
				if (iter->subRules.size() == 0) { // if this submenu is empty now, remove it
					parent.subRules.erase(iter);
					loopRestartRequired = true;
					break;
				}
			}
		}
	} while (loopRestartRequired);
}

void Model_Proxy::removeEquivalentRules(Model_Rule const& base) {
	if (base.dataSource) {
		Model_Rule* eqRule = this->getRuleByEntry(*base.dataSource, this->rules, base.type);
		if (eqRule) {
			this->removeRule(eqRule);
		}
	} else if (base.subRules.size()) {
		for (std::list<Model_Rule>::const_iterator iter = base.subRules.begin(); iter != base.subRules.end(); iter++) {
			this->removeEquivalentRules(*iter);
		}
	}
}

void Model_Proxy::removeRule(Model_Rule* rule) {
	assert(rule != NULL);
	Model_Rule* parent = NULL;
	int rlist_size = 0;
	do {
		try {
			parent = this->getParentRule(rule);
		} catch (ItemNotFoundException const& e) {
			parent = NULL;
		}
		std::list<Model_Rule>& rlist = this->getRuleList(parent);
		std::list<Model_Rule>::iterator iter = this->getListIterator(*rule, rlist);
		rlist.erase(iter);

		rule = parent; // go one step up to remove this rule if empty
		rlist_size = rlist.size();
	} while (rlist_size == 0 && parent != NULL); // delete all the empty submenus above
}

bool Model_Proxy::hasVisibleRules(Model_Rule const* parent) const {
	std::list<Model_Rule> const& list = parent ? parent->subRules : this->rules;
	for (std::list<Model_Rule>::const_iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->isVisible) {
			if (iter->type == Model_Rule::SUBMENU) {
				bool has = this->hasVisibleRules(&*iter);
				if (has) {
					return true;
				}
			} else {
				return true;
			}
		}
	}
	return false;
}

Model_Rule* Model_Proxy::getVisibleRuleForEntry(Model_Entry const& entry, Model_Rule* parent) {
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->isVisible && iter->dataSource == &entry) {
			return &*iter;
		}
		Model_Rule* subResult = this->getVisibleRuleForEntry(entry, &*iter);
		if (subResult) {
			return subResult;
		}
	}
	return NULL;
}

std::list<Model_Rule*> Model_Proxy::getVisibleRulesByType(Model_Rule::RuleType type, Model_Rule* parent) {
	std::list<Model_Rule*> result;
	std::list<Model_Rule>& list = parent ? parent->subRules : this->rules;

	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->isVisible) {
			if (iter->type == type) {
				result.push_back(&*iter);
			}
			if (iter->subRules.size()) {
				std::list<Model_Rule*> subResult = this->getVisibleRulesByType(type, &*iter);
				result.splice(result.end(), subResult);
			}
		}
	}

	return result;
}

Model_Proxy::operator ArrayStructure() const {
	ArrayStructure result;
	result["rules"].isArray = true;
	int ruleIterPos = 0;
	for (std::list<Model_Rule>::const_iterator ruleIter = this->rules.begin(); ruleIter != this->rules.end(); ruleIter++) {
		result["rules"][ruleIterPos] = *ruleIter;
		ruleIterPos++;
	}
	result["index"] = this->index;
	result["permissions"] = this->permissions;
	result["fileName"] = this->fileName;
	result["dataSource"] = this->dataSource;
	result["__idPathList"].isArray = true;
	{
		for (std::map<Model_Script*, std::list<std::list<std::string> > >::const_iterator iter = this->__idPathList.begin(); iter != this->__idPathList.end(); iter++) {
			result["__idPathList"]["k"] = iter->first;
			int i = 0;
			for (std::list<std::list<std::string> >::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
				result["__idPathList"]["v"][i] = ArrayStructure(*iter2);
				i++;
			}
		}
	}
	result["__idPathList_OtherEntriesPlaceHolders"].isArray = true;
	{
		for (std::map<Model_Script*, std::list<std::list<std::string> > >::const_iterator iter = this->__idPathList_OtherEntriesPlaceHolders.begin(); iter != this->__idPathList_OtherEntriesPlaceHolders.end(); iter++) {
			result["__idPathList_OtherEntriesPlaceHolders"]["k"] = iter->first;
			int i = 0;
			for (std::list<std::list<std::string> >::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
				result["__idPathList_OtherEntriesPlaceHolders"]["v"][i] = ArrayStructure(*iter2);
				i++;
			}
		}
	}

	return result;
}


Model_Proxy& Model_Proxy::fromPtr(Proxy* proxy) {
	if (proxy != NULL) {
		try {
			return dynamic_cast<Model_Proxy&>(*proxy);
		} catch (std::bad_cast const& e) {
		}
	}
	throw BadCastException("Model_Proxy::fromPtr failed");
}
Model_Proxy const& Model_Proxy::fromPtr(Proxy const* proxy) {
	if (proxy != NULL) {
		try {
			return dynamic_cast<Model_Proxy const&>(*proxy);
		} catch (std::bad_cast const& e) {
		}
	}
	throw BadCastException("Model_Proxy::fromPtr [const] failed");
}

