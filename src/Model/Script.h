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

#ifndef GRUB_CUSTOMIZER_SCRIPT_INCLUDED
#define GRUB_CUSTOMIZER_SCRIPT_INCLUDED
#include <string>
#include <list>
#include <cstdlib>
#include <sys/stat.h>
#include "Entry.h"
#include <unistd.h>
#include "../Model/EntryPathFollower.h"
#include "../lib/Trait/LoggerAware.h"
#include "../lib/md5.h"
#include "../config.h"
#include "../lib/Exception.h"
#include "../lib/ArrayStructure.h"
#include "../lib/Type.h"

struct Model_Script : public Model_EntryPathFollower, public Trait_LoggerAware, public Script {
	std::string name, fileName;
	bool isCustomScript;
	Model_Entry root;
	Model_Script(std::string const& name, std::string const& fileName);
	bool isModified(Model_Entry* parent = NULL);
	std::list<Model_Entry>& entries();
	std::list<Model_Entry> const& entries() const;
	bool isInScriptDir(std::string const& cfg_dir) const;
	Model_Entry* getEntryByPath(std::list<std::string> const& path);
	Model_Entry* getEntryByName(std::string const& name, std::list<Model_Entry>& parentList);
	Model_Entry* getEntryByHash(std::string const& hash, std::list<Model_Entry>& parentList);
	Model_Entry* getPlaintextEntry();
	void moveToBasedir(std::string const& cfg_dir); //moves the file from any location to grub.d and adds the prefix PS_ (proxified Script) or DS_ (default script)
	bool moveFile(std::string const& newPath, short int permissions = -1);
	std::list<std::string> buildPath(Model_Entry const& entry, Model_Entry const* parent) const;
	std::list<std::string> buildPath(Model_Entry const& entry) const;
	std::string buildPathString(Model_Entry const& entry, bool withOtherEntriesPlaceholder = false) const;
	bool hasEntry(Model_Entry const& entry, Model_Entry const * parent = NULL) const;
	void deleteEntry(Model_Entry const& entry, Model_Entry* parent = NULL);
	bool deleteFile();
	operator ArrayStructure() const;
	static Model_Script& fromPtr(Script* script);
	static Model_Script const& fromPtr(Script const* script);
	static int extractIndexFromPath(std::string const& path, std::string const& cfgDirPath);
};

#endif
