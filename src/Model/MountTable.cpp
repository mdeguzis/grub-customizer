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

#include "MountTable.h"

Model_MountTable_Mountpoint::Model_MountTable_Mountpoint(std::string const& mountpoint, bool isMounted) : isMounted(isMounted), mountpoint(mountpoint) {}

Model_MountTable_Mountpoint::Model_MountTable_Mountpoint(std::string const& device, std::string const& mountpoint, std::string const& options, bool isMounted)
	: device(device), mountpoint(mountpoint), isMounted(isMounted), options(options)
{
}

bool Model_MountTable_Mountpoint::isValid(std::string const& prefix, bool isRoot) const {
	return device != "" && (mountpoint != prefix || isRoot) && mountpoint != prefix+"none" && fileSystem != "" && options != "" && dump != "" && pass != "";
}
Model_MountTable_Mountpoint::operator bool() const {
	return this->isValid();
}

void Model_MountTable_Mountpoint::mount(){
	if (!isMounted){
		int res = system(("mount '"+device+"' '"+mountpoint+"'"+(options != "" ? " -o '"+options+"'" : "")).c_str());
		if (res != 0)
			throw MountException("mount failed", __FILE__, __LINE__);

		this->isMounted = true;
	}
}
void Model_MountTable_Mountpoint::umount(){
	if (isMounted){
		int res = system(("umount '"+mountpoint+"'").c_str());
		if (res != 0)
			throw UMountException("umount failed", __FILE__, __LINE__);

		this->isMounted = false;
	}
}

bool Model_MountTable_Mountpoint::isLiveCdFs(){
	return this->fileSystem == "aufs" | this->fileSystem == "overlayfs";
}

Model_MountTable::Model_MountTable(FILE* source, std::string const& prefix, bool default_isMounted_flag)
	: loaded(false)
{
	this->loadData(source, prefix, default_isMounted_flag);
}

Model_MountTable::Model_MountTable(std::string const& rootDirectory)
	: loaded(false)
{
	this->loadData(rootDirectory);
}

Model_MountTable::Model_MountTable() : loaded(false) {}

void Model_MountTable::loadData(FILE* source, std::string const& prefix, bool default_isMounted_flag){
	int c;
	int rowEntryPos = 0;
	bool isComment = false;
	bool isBeginOfRow = true;
	char previousChar = 0;
	Model_MountTable_Mountpoint newMp(prefix, default_isMounted_flag);
	while ((c = fgetc(source)) != EOF){
		if (isBeginOfRow && c == '#')
			isComment = true;
		else if (c == '\n'){
			bool isRoot = newMp.mountpoint == prefix + "/";
			if (newMp.mountpoint[newMp.mountpoint.length()-1] == '/')
				newMp.mountpoint = newMp.mountpoint.substr(0, newMp.mountpoint.length()-1);

			if (newMp.isValid(prefix, isRoot)){
				this->remove(newMp);
				this->push_back(newMp);
			}

			newMp = Model_MountTable_Mountpoint(prefix, default_isMounted_flag);
			rowEntryPos = 0;
			isBeginOfRow = true;
			isComment = false;
		}
		else if (!isComment) {
			if (c == ' ' || c == '\t'){
				if (previousChar != ' ' && previousChar != '\t')
					rowEntryPos++;
				isBeginOfRow = false;
			}
			else {
				switch (rowEntryPos){
					case 0:	newMp.device += char(c); break;
					case 1: newMp.mountpoint += char(c); break;
					case 2: newMp.fileSystem += char(c); break;
					case 3: newMp.options += char(c); break;
					case 4: newMp.dump += char(c); break;
					case 5: newMp.pass += char(c); break;
				}
				isBeginOfRow = false;
			}
		}
		previousChar = c;
	}
	if (newMp.mountpoint[newMp.mountpoint.length()-1] == '/')
		newMp.mountpoint = newMp.mountpoint.substr(0, newMp.mountpoint.length()-1);

	if (newMp.isValid(prefix)){
		this->remove(newMp);
		this->push_back(newMp);
	}

	loaded = true;
}

void Model_MountTable::loadData(std::string const& rootDirectory){
	FILE* fstabFile = fopen((rootDirectory+"/etc/fstab").c_str(), "r");
	if (fstabFile != NULL){
		this->loadData(fstabFile, rootDirectory);
		Model_MountTable mtab;
		FILE* mtabfile = fopen("/etc/mtab", "r"); //use global mtab - the local one is unmanaged
		if (mtabfile){
			mtab = Model_MountTable(mtabfile, "", true);
			fclose(mtabfile);
		}
		this->sync(mtab);
		fclose(fstabFile);
	}
}

void Model_MountTable::clear(std::string const& prefix){
	Model_MountTable::iterator iter = this->begin();
	while (iter != this->end()){
		if (iter->mountpoint.substr(0, prefix.length()) == prefix){
			this->erase(iter);
			iter = this->begin();
		}
		else
			iter++;
	}
	loaded = false;
}

void Model_MountTable::sync(Model_MountTable const& mtab){
	for (Model_MountTable::const_iterator iter = mtab.begin(); iter != mtab.end(); iter++){
		this->add(*iter);
	}
}

bool Model_MountTable::isLoaded() const {
	return loaded;
}

Model_MountTable::operator bool() const {
	return isLoaded();
}

Model_MountTable_Mountpoint& Model_MountTable::getEntryRefByMountpoint(std::string const& mountPoint) {
	for (std::list<Model_MountTable_Mountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountPoint)
			return *iter;
	}
	throw MountpointNotFoundException("mountpoint not found", __FILE__, __LINE__);
}

Model_MountTable_Mountpoint Model_MountTable::getEntryByMountpoint(std::string const& mountPoint) const {
	for (std::list<Model_MountTable_Mountpoint>::const_iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountPoint)
			return *iter;
	}
	return Model_MountTable_Mountpoint();
}

Model_MountTable_Mountpoint& Model_MountTable::add(Model_MountTable_Mountpoint const& mpToAdd){
	this->remove(mpToAdd); //remove existing mountpoints with the same directory
	this->push_back(mpToAdd);
	return this->back();
}

void Model_MountTable::remove(Model_MountTable_Mountpoint const& mountpoint){
	for (std::list<Model_MountTable_Mountpoint>::iterator iter = this->begin(); iter != this->end(); iter++){
		if (iter->mountpoint == mountpoint.mountpoint){
			this->erase(iter);
			break;
		}
	}
}

void Model_MountTable::umountAll(std::string const& prefix){
	for (Model_MountTable::reverse_iterator iter = this->rbegin(); iter != this->rend(); iter++){
		if (iter->mountpoint.substr(0, prefix.length()) == prefix && iter->mountpoint != prefix && iter->isMounted){
			iter->umount();
		}
	}

	this->getEntryRefByMountpoint(prefix).umount();
}

void Model_MountTable::mountRootFs(std::string const& device, std::string const& mountpoint){
	this->add(Model_MountTable_Mountpoint(device, mountpoint, "")).mount();
	this->loadData(mountpoint);
	FILE* fstab = fopen((mountpoint + "/etc/fstab").c_str(), "r");
	if (fstab){
		fclose(fstab); //opening of fstab is just a test

		try {
			this->add(Model_MountTable_Mountpoint("/proc", mountpoint + "/proc", "bind")).mount();
			this->add(Model_MountTable_Mountpoint("/sys", mountpoint + "/sys", "bind")).mount();
			this->add(Model_MountTable_Mountpoint("/dev", mountpoint + "/dev", "bind")).mount();
		}
		//errors while mounting any of this partitions may not be a problem
		catch (SystemException const& e){}
		catch (MountpointNotFoundException const& e){}
	}
	else
		throw MissingFstabException("fstab is required but was not found", __FILE__, __LINE__);
	this->loaded = true;
}

Model_MountTable::operator std::string() const {
	std::string result;
	for (Model_MountTable::const_iterator iter = this->begin(); iter != this->end(); iter++){
		result += std::string("[") + (iter->isMounted ? "x" : " ")  + "] " + iter->device + " " + iter->mountpoint + "\n";
	}
	return result;
}
