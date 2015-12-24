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

#ifndef MOUNT_TABLE_INCLUDED
#define MOUNT_TABLE_INCLUDED
#include <list>
#include <cstdio>
#include <string>
#include <cstdlib>
#include "../lib/Trait/LoggerAware.h"
#include "../lib/Exception.h"


struct Model_MountTable_Mountpoint {
	std::string device, mountpoint, fileSystem, options, dump, pass;
	bool isMounted;
	bool isValid(std::string const& prefix = "", bool isRoot = false) const;
	operator bool() const;
	Model_MountTable_Mountpoint(std::string const& mountpoint = "", bool isMounted = false);
	Model_MountTable_Mountpoint(std::string const& device, std::string const& mountpoint, std::string const& options, bool isMounted = false);
	void mount();
	void umount();
	bool isLiveCdFs();
};

class Model_MountTable : public std::list<Model_MountTable_Mountpoint>, public Trait_LoggerAware {
	bool loaded;
	public:
	Model_MountTable(FILE* source, std::string const& rootDirectory, bool default_isMounted_flag = false);
	Model_MountTable(std::string const& rootDirectory);
	Model_MountTable();
	void sync(Model_MountTable const& mtab);
	bool isLoaded() const;
	operator bool() const;
	void loadData(FILE* source, std::string const& prefix, bool default_isMounted_flag = false);
	void loadData(std::string const& rootDirectory);
	void clear(std::string const& prefix);
	Model_MountTable_Mountpoint getEntryByMountpoint(std::string const& mountPoint) const;
	Model_MountTable_Mountpoint& getEntryRefByMountpoint(std::string const& mountPoint);
	Model_MountTable_Mountpoint& add(Model_MountTable_Mountpoint const& mpToAdd);
	void remove(Model_MountTable_Mountpoint const& mountpoint);
	void umountAll(std::string const& prefix);
	void mountRootFs(std::string const& device, std::string const& mountpoint);
	operator std::string() const;
};

class Model_MountTable_Connection {
protected:
	Model_MountTable* mountTable;
public:
	Model_MountTable_Connection() : mountTable(NULL) {}

	void setMountTable(Model_MountTable& mountTable){
		this->mountTable = &mountTable;
	}
};

#endif

