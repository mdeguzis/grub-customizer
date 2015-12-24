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

#include "Rule.h"

Model_Rule::Model_Rule(Model_Entry& source, bool isVisible, Model_EntryPathFollower& pathFollower, std::list<std::list<std::string> > const& pathesToIgnore, std::list<std::string> const& currentPath) //generate rule for given entry. __idname is only required for re-syncing (soft-reload)
	: type(source.type == Model_Entry::PLAINTEXT ? Model_Rule::PLAINTEXT : (source.type == Model_Entry::SUBMENU ? Model_Rule::SUBMENU : Model_Rule::NORMAL)), isVisible(isVisible), __idpath(currentPath), outputName(source.name), dataSource(source.type == Model_Entry::SUBMENU ? NULL : &source)
{
	if (source.type == Model_Entry::SUBMENU) {
		Model_Rule placeholder(Model_Rule::OTHER_ENTRIES_PLACEHOLDER, currentPath, "*", this->isVisible);
		placeholder.dataSource = pathFollower.getEntryByPath(currentPath);
		this->subRules.push_front(placeholder);
	}
	for (std::list<Model_Entry>::iterator iter = source.subEntries.begin(); iter != source.subEntries.end(); iter++) {
		std::list<std::string> currentPath_in_loop = currentPath;
		currentPath_in_loop.push_back(iter->name);

		//find out if currentPath is on the blacklist
		bool currentPath_in_loop_is_blacklisted = false;
		for (std::list<std::list<std::string> >::const_iterator pti_iter = pathesToIgnore.begin(); pti_iter != pathesToIgnore.end(); pti_iter++) {
			if (*pti_iter == currentPath_in_loop) {
				currentPath_in_loop_is_blacklisted = true;
				break;
			}
		}

		//add this entry as rule if not blacklisted
		if (!currentPath_in_loop_is_blacklisted){
			this->subRules.push_back(Model_Rule(*iter, isVisible, pathFollower, pathesToIgnore, currentPath_in_loop));
		}
	}
}

std::string Model_Rule::toString(Model_EntryPathBilder const& pathBuilder){
	std::string result = isVisible ? "+" : "-";
	if (type == Model_Rule::PLAINTEXT) {
		result += "#text";
	} else if (dataSource) {
		result += pathBuilder.buildPathString(*dataSource, this->type == OTHER_ENTRIES_PLACEHOLDER);
		if (this->dataSource->content.size() && this->type != Model_Rule::OTHER_ENTRIES_PLACEHOLDER) {
			result += "~" + md5(this->dataSource->content) + "~";
		}
	} else if (type == Model_Rule::SUBMENU) {
		result += "'SUBMENU'"; // dummy data source
	} else {
		result += "???";
	}
	if (type == Model_Rule::SUBMENU || (type == Model_Rule::NORMAL && dataSource && dataSource->name != outputName)) {
		result += " as '"+str_replace("'", "''", outputName)+"'";
	}

	if (this->dataSource) {
		std::string sourceScriptPath = pathBuilder.buildScriptPath(*this->dataSource);
		if (sourceScriptPath != "") {
			result += " from '" + sourceScriptPath + "'";
		}
	}

	if (type == Model_Rule::SUBMENU && this->subRules.size() > 0) {
		result += "{";
		for (std::list<Model_Rule>::iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
			if (iter != this->subRules.begin())
				result += ", ";
			result += iter->toString(pathBuilder);
		}
		result += "}";
	}
	return result;
}

Model_Rule::Model_Rule(RuleType type, std::list<std::string> path, std::string outputName, bool isVisible)
	: type(type), isVisible(isVisible), __idpath(path), outputName(outputName), dataSource(NULL)
{}

Model_Rule::Model_Rule(RuleType type, std::list<std::string> path, bool isVisible)
	: type(type), isVisible(isVisible), __idpath(path), outputName(path.back()), dataSource(NULL)
{}

Model_Rule::Model_Rule()
	: type(Model_Rule::NORMAL), isVisible(false), dataSource(NULL)
{}

std::string Model_Rule::getEntryName() const {
	if (this->dataSource)
		return this->dataSource->name;
	else
		return "?";
}

bool Model_Rule::hasRealSubrules() const {
	for (std::list<Model_Rule>::const_iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
		if (iter->isVisible && ((iter->type == Model_Rule::NORMAL && iter->dataSource) || (iter->type == Model_Rule::SUBMENU && iter->hasRealSubrules()))) {
			return true;
		}
	}
	return false;
}


void Model_Rule::print(std::ostream& out) const {
	if (this->isVisible) {
		if (this->type == Model_Rule::PLAINTEXT && this->dataSource) {
			out << this->dataSource->content;
		} else if (this->type == Model_Rule::NORMAL && this->dataSource) {
			out << "menuentry";
			out << " \"" << this->outputName << "\"" << this->dataSource->extension << "{\n";
			out << this->dataSource->content;
			out << "}\n";
		} else if (this->type == Model_Rule::SUBMENU && this->hasRealSubrules()) {
			out << "submenu" << " \"" << this->outputName << "\"" << "{\n";
			for (std::list<Model_Rule>::const_iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
				iter->print(out);
			}
			out << "}\n";
		}
	}
}

void Model_Rule::setVisibility(bool isVisible) {
	this->isVisible = isVisible;
	for (std::list<Model_Rule>::iterator iter = this->subRules.begin(); iter != this->subRules.end(); iter++) {
		iter->setVisibility(isVisible);
	}
}

Model_Rule::operator ArrayStructure() const {
	ArrayStructure result;

	result["dataSource"] = this->dataSource;
	result["outputName"] = this->outputName;
	result["__idHash"] = this->__idHash;
	result["__idpath"] = ArrayStructure(this->__idpath);
	result["__sourceScriptPath"] = this->__sourceScriptPath;
	result["isVisible"] = this->isVisible;
	result["subRules"].isArray = true;
	int i = 0;
	for (std::list<Model_Rule>::const_iterator subRuleIter = this->subRules.begin(); subRuleIter != this->subRules.end(); subRuleIter++) {
		result["subRules"][i] = ArrayStructure(*subRuleIter);
		i++;
	}
	result["type"] = this->type;

	return result;
}

Model_Rule& Model_Rule::fromPtr(Rule* rule) {
	if (rule != NULL) {
		try {
			return dynamic_cast<Model_Rule&>(*rule);
		} catch (std::bad_cast const& e) {
		}
	}
	throw BadCastException("Model_Rule::fromPtr failed");
}

Model_Rule const& Model_Rule::fromPtr(Rule const* rule) {
	if (rule != NULL) {
		try {
			return dynamic_cast<Model_Rule const&>(*rule);
		} catch (std::bad_cast const& e) {
		}
	}
	throw BadCastException("Model_Rule::fromPtr [const] failed");
}
