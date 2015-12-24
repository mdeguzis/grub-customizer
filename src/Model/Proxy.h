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

#ifndef GRUB_CUSTOMIZER_PROXY_INCLUDED
#define GRUB_CUSTOMIZER_PROXY_INCLUDED
#include "Rule.h"
#include "Script.h"
#include <sys/stat.h>
#include "../lib/assert.h"
#include <unistd.h>
#include <map>
#include "EntryPathBuilderImpl.h"
#include "../lib/Exception.h"
#include "../lib/ArrayStructure.h"
#include "ProxyScriptData.h"
#include "../lib/Type.h"

struct Model_Proxy : public Proxy {
	std::list<Model_Rule> rules;
	int index;
	short int permissions;
	std::string fileName; //may be the same as Script::fileName
	Model_Script* dataSource;
	std::map<Model_Script*, std::list<std::list<std::string> > > __idPathList; //to be used by sync();
	std::map<Model_Script*, std::list<std::list<std::string> > > __idPathList_OtherEntriesPlaceHolders; //to be used by sync();
	Model_Proxy();
	Model_Proxy(Model_Script& dataSource, bool activateRules = true);
	bool isExecutable() const;
	void set_isExecutable(bool value);
	static std::list<Model_Rule> parseRuleString(const char** ruleString, std::string const& cfgDirPrefix);
	void importRuleString(const char* ruleString, std::string const& cfgDirPrefix);
	Model_Rule* getRuleByEntry(Model_Entry const& entry, std::list<Model_Rule>& list, Model_Rule::RuleType ruletype);
	std::list<Model_Rule*> getForeignRules(Model_Rule* parent = NULL);
	void unsync(Model_Rule* parent = NULL);
	bool sync(bool deleteInvalidRules = true, bool expand = true, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>());
	void sync_connectExisting(Model_Rule* parent = NULL, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>());
	void sync_connectExistingByHash(Model_Rule* parent = NULL, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>());
	void sync_add_placeholders(Model_Rule* parent = NULL, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>());
	void sync_expand(std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>());
	void sync_cleanup(Model_Rule* parent = NULL, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>());
	bool isModified(Model_Rule const* parentRule = NULL, Model_Entry const* parentEntry = NULL) const;
	bool deleteFile();
	std::list<std::string> getScriptList(std::map<Model_Entry const*, Model_Script const*> const& entrySourceMap, std::map<Model_Script const*, std::string> const& scriptTargetMap) const;
	bool generateFile(std::string const& path, int cfg_dir_prefix_length, std::string const& cfg_dir_noprefix, std::map<Model_Entry const*, Model_Script const*> ruleSourceMap, std::map<Model_Script const*, std::string> const& scriptTargetMap); //before running this function, the realted script file must be saved!
	std::string getScriptName();
	Model_Rule& moveRule(Model_Rule* rule, int direction);
	void merge(Model_Proxy const& foreignProxy, int direction);
	std::list<Model_Rule>::iterator getNextVisibleRule(std::list<Model_Rule>::iterator base, int direction);
	Model_Rule* splitSubmenu(Model_Rule* position);
	Model_Rule* createSubmenu(Model_Rule* position);
	bool ruleIsFromOwnScript(Model_Rule const& rule) const;
	void removeForeignChildRules(Model_Rule& parent);
	void removeEquivalentRules(Model_Rule const& base);
	void removeRule(Model_Rule* rule);
	std::list<Model_Rule>::iterator getListIterator(Model_Rule const& needle, std::list<Model_Rule>& haystack);
	Model_Rule* getParentRule(Model_Rule* child, Model_Rule* root = NULL);
	std::list<Model_Rule>& getRuleList(Model_Rule* parentElement);
	bool hasVisibleRules(Model_Rule const* parent = NULL) const;
	Model_Rule* getVisibleRuleForEntry(Model_Entry const& entry, Model_Rule* parent = NULL);
	std::list<Model_Rule*> getVisibleRulesByType(Model_Rule::RuleType type, Model_Rule* parent = NULL);
	operator ArrayStructure() const;
	static Model_Proxy& fromPtr(Proxy* proxy);
	static Model_Proxy const& fromPtr(Proxy const* proxy);
private:
	static void adjustIterator(std::list<Model_Rule>::iterator& iter, int adjustment);
};

#endif
