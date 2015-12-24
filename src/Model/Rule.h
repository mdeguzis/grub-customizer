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

#ifndef GRUB_CUSTOMIZER_RULE_INCLUDED
#define GRUB_CUSTOMIZER_RULE_INCLUDED
#include <string>
#include "Entry.h"
#include "../Model/EntryPathBuilder.h"
#include "../Model/EntryPathFollower.h"
#include <ostream>
#include "../lib/md5.h"
#include "../lib/ArrayStructure.h"
#include "../lib/Type.h"

struct Model_Rule : public Rule {
	Model_Entry* dataSource; //assigned when using RuleType::OTHER_ENTRIES_PLACEHOLDER
	std::string outputName;
	std::string __idHash; //should only be used by sync()!
	std::list<std::string> __idpath; //should only be used by sync()!
	std::string __sourceScriptPath; //should only be used by sync()!
	bool isVisible;
	std::list<Model_Rule> subRules;
	enum RuleType {
		NORMAL, OTHER_ENTRIES_PLACEHOLDER, PLAINTEXT, SUBMENU
	} type;
	Model_Rule(RuleType type, std::list<std::string> path, std::string outputName, bool isVisible);
	Model_Rule(RuleType type, std::list<std::string> path, bool isVisible);
	Model_Rule(Model_Entry& source, bool isVisible, Model_EntryPathFollower& pathFollower, std::list<std::list<std::string> > const& pathesToIgnore = std::list<std::list<std::string> >(), std::list<std::string> const& currentPath = std::list<std::string>()); //generate rule for given entry
	Model_Rule();
	std::string toString(Model_EntryPathBilder const& pathBuilder);
	bool hasRealSubrules() const;
	void print(std::ostream& out) const;
	std::string getEntryName() const;
	void setVisibility(bool isVisible);
	operator ArrayStructure() const;
	static Model_Rule& fromPtr(Rule* rule);
	static Model_Rule const& fromPtr(Rule const* rule);
};

#endif
