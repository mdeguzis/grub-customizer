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

#ifndef GRUB_CUSTOMIZER_ENTRY_INCLUDED
#define GRUB_CUSTOMIZER_ENTRY_INCLUDED
#include <cstdio>
#include <string>
#include <list>
#include "../lib/Trait/LoggerAware.h"
#include "../lib/str_replace.h"
#include "../lib/ArrayStructure.h"
#include "../lib/trim.h"
#include "../lib/Type.h"

struct Model_Entry_Row {
	Model_Entry_Row(FILE* sourceFile);
	Model_Entry_Row();
	std::string text;
	bool eof;
	bool is_loaded;
	operator bool();
};

struct Model_Entry : public Trait_LoggerAware, public Entry {
	enum EntryType {
		MENUENTRY,
		SUBMENU,
		SCRIPT_ROOT,
		PLAINTEXT
	} type;
	bool isValid, isModified;
	std::string name, extension, content;
	char quote;
	std::list<Model_Entry> subEntries;
	Model_Entry();
	Model_Entry(std::string name, std::string extension, std::string content = "", EntryType type = MENUENTRY);
	Model_Entry(FILE* sourceFile, Model_Entry_Row firstRow = Model_Entry_Row(), Logger* logger = NULL, std::string* plaintextBuffer = NULL);
private:
	void readSubmenu(FILE* sourceFile, Model_Entry_Row firstRow);
	void readMenuEntry(FILE* sourceFile, Model_Entry_Row firstRow);
public:
	std::list<Model_Entry>& getSubEntries();
	operator bool() const;
	operator ArrayStructure() const;
	static Model_Entry& fromPtr(Entry* entry);
	static Model_Entry const& fromPtr(Entry const* entry);
};

#endif
