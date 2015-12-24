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

#include "Repository.h"

void Model_Repository::load(std::string const& directory, bool is_proxifiedScript_dir){
	DIR* dir = opendir(directory.c_str());
	if (dir){
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(dir))) {
			stat((directory+"/"+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				bool scriptAdded = false;
				if (!is_proxifiedScript_dir && !Model_ProxyScriptData::is_proxyscript(directory+"/"+entry->d_name) && std::string(entry->d_name).length() >= 4 && entry->d_name[0] >= '0' && entry->d_name[0] <= '9' && entry->d_name[1] >= '0' && entry->d_name[1] <= '9' && entry->d_name[2] == '_'){
					this->push_back(Model_Script(std::string(entry->d_name).substr(3), directory+"/"+entry->d_name));
					scriptAdded = true;
				} else if (is_proxifiedScript_dir) {
					this->push_back(Model_Script(Model_PscriptnameTranslator::decode(entry->d_name), directory+"/"+entry->d_name));
					scriptAdded = true;
				}
				if (scriptAdded && this->hasLogger()) {
					this->back().setLogger(this->getLogger());
				}
			}
		}
		closedir(dir);
	}
}

Model_Script* Model_Repository::getScriptByFilename(std::string const& fileName, bool createScriptIfNotFound) {
	Model_Script* result = NULL;
	for (Model_Repository::iterator iter = this->begin(); iter != this->end() && !result; iter++){
		if (iter->fileName == fileName)
			result = &*iter;
	}
	if (result == NULL && createScriptIfNotFound){
		this->push_back(Model_Script("noname", fileName));
		result = &this->back();

		if (this->hasLogger()) {
			this->back().setLogger(this->getLogger());
		}
	}
	return result;
}

Model_Script* Model_Repository::getScriptByName(std::string const& name) {
	Model_Script* result = NULL;
	for (Model_Repository::iterator iter = this->begin(); iter != this->end() && !result; iter++){
		if (iter->name == name)
			result = &*iter;
	}
	return result;
}

Model_Script* Model_Repository::getScriptByEntry(Model_Entry const& entry) {
	for (std::list<Model_Script>::iterator iter = this->begin(); iter != this->end(); iter++) {
		if (iter->hasEntry(entry)) {
			return &*iter;
		}
	}
	return NULL;
}

Model_Script const* Model_Repository::getScriptByEntry(Model_Entry const& entry) const {
	for (std::list<Model_Script>::const_iterator iter = this->begin(); iter != this->end(); iter++) {
		if (iter->hasEntry(entry)) {
			return &*iter;
		}
	}
	return NULL;
}

Model_Script* Model_Repository::getCustomScript() {
	for (std::list<Model_Script>::iterator iter = this->begin(); iter != this->end(); iter++) {
		if (iter->isCustomScript) {
			return &*iter;
		}
	}
	return NULL;
}

Model_Script* Model_Repository::getNthScript(int pos){
	Model_Script* result = NULL;
	int i = 0;
	for (Model_Repository::iterator iter = this->begin(); result == NULL && iter != this->end(); iter++){
		if (i == pos)
			result = &*iter;
		i++;
	}
	return result;
}

void Model_Repository::deleteAllEntries(bool preserveModifiedScripts){
	for (Model_Repository::iterator iter = this->begin(); iter != this->end(); iter++){
		if (preserveModifiedScripts && iter->isModified()) {
			continue;
		}
		iter->entries().clear();
	}
}

Model_Script* Model_Repository::createScript(std::string const& name, std::string const& fileName, std::string const& content) {
	assert_filepath_empty(fileName, __FILE__, __LINE__);
	FILE* script = fopen(fileName.c_str(), "w");
	if (script) {
		fputs(content.c_str(), script);
		fclose(script);

		Model_Script newScript(name, fileName);
		this->push_back(newScript);
		return &this->back();
	}
	return NULL;
}

void Model_Repository::createScript(Model_Script const& script, std::string const& content) {
	assert_filepath_empty(script.fileName, __FILE__, __LINE__);
	FILE* scriptFile = fopen(script.fileName.c_str(), "w");
	if (scriptFile) {
		fputs(content.c_str(), scriptFile);
		fclose(scriptFile);
	} else {
		throw FileSaveException("cannot open file for saving: " + script.fileName, __FILE__, __LINE__);
	}
}

/**
 * returns a list of all scripts associated by its fileNames
 */
std::map<std::string, Model_Script*> Model_Repository::getScriptPathMap() {
	std::map<std::string, Model_Script*> map;
	for (Model_Repository::iterator iter = this->begin(); iter != this->end(); iter++){
		map[iter->fileName] = &*iter;
	}
	for (Model_Repository::iterator iter = this->trash.begin(); iter != this->trash.end(); iter++){
		map[iter->fileName] = &*iter;
	}
	return map;
}

void Model_Repository::removeScript(Model_Script const& script) {
	for (std::list<Model_Script>::iterator scriptIter = this->begin(); scriptIter != this->end(); scriptIter++) {
		if (&*scriptIter == &script) {
			this->trash.push_back(*scriptIter);
			this->erase(scriptIter);
			return;
		}
	}
}

void Model_Repository::clearTrash() {
	for (std::list<Model_Script>::iterator scriptIter = this->trash.begin(); scriptIter != this->trash.end(); scriptIter++) {
		scriptIter->deleteFile();
	}
	this->trash.clear();
}

Model_Repository::operator ArrayStructure() const {
	ArrayStructure result;
	result["(items)"].isArray = true;
	int i = 0;
	for (std::list<Model_Script>::const_iterator iter = this->begin(); iter != this->end(); iter++) {
		result["(items)"][i] = ArrayStructure(*iter);
		i++;
	}
	return result;
}




