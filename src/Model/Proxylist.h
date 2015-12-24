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

#ifndef GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#define GRUB_CUSTOMIZER_PROXYLIST_INCLUDED
#include <list>
#include <sstream>
#include "Proxy.h"
#include "../lib/Trait/LoggerAware.h"
#include "../lib/Exception.h"
#include "../lib/ArrayStructure.h"

struct Model_Proxylist_Item {
	std::string labelPathValue;
	std::string labelPathLabel;
	std::string numericPathValue;
	std::string numericPathLabel;
};
struct Model_Proxylist : public std::list<Model_Proxy>, public Trait_LoggerAware {
	std::list<Model_Proxy> trash; //removed proxies
	std::list<Model_Proxy*> getProxiesByScript(Model_Script const& script);
	std::list<const Model_Proxy*> getProxiesByScript(Model_Script const& script) const;
	std::list<Model_Rule*> getForeignRules();
	void sync_all(bool deleteInvalidRules = true, bool expand = true, Model_Script* relatedScript = NULL, std::map<std::string, Model_Script*> scriptMap = std::map<std::string, Model_Script*>()); //relatedScript = NULL: sync all proxies, otherwise only sync proxies wich target the given Script
	void unsync_all();
	bool proxyRequired(Model_Script const& script) const;
	void deleteAllProxyscriptFiles();
	static bool compare_proxies(Model_Proxy const& a, Model_Proxy const& b);
	void sort();
	void deleteProxy(Model_Proxy* proxyPointer);
	void clearTrash();
	std::list<Model_Proxylist_Item> generateEntryTitleList() const;
	std::list<std::string> getToplevelEntryTitles() const;
	static std::list<Model_Proxylist_Item> generateEntryTitleList(std::list<Model_Rule> const& parent, std::string const& labelPathPrefix, std::string const& numericPathPrefix, std::string const& numericPathLabelPrefix, int* offset = NULL);
	Model_Proxy* getProxyByRule(Model_Rule* rule, std::list<Model_Rule> const& list, Model_Proxy& parentProxy);
	Model_Proxy* getProxyByRule(Model_Rule* rule);
	std::list<Model_Rule>::iterator moveRuleToNewProxy(Model_Rule& rule, int direction, Model_Script* dataSource = NULL);
	std::list<Model_Rule>::iterator getNextVisibleRule(Model_Rule* base, int direction);
	std::list<Model_Rule>::iterator getNextVisibleRule(std::list<Model_Rule>::iterator base, int direction);
	std::list<Model_Proxy>::iterator getIter(Model_Proxy const* proxy);
	void splitProxy(Model_Proxy const* proxyToSplit, Model_Rule const* firstRuleOfPart2, int direction);
	Model_Rule* getVisibleRuleForEntry(Model_Entry const& entry);
	bool hasConflicts() const;
	bool hasProxy(Model_Proxy* proxy);
	operator ArrayStructure() const;
};

#endif
