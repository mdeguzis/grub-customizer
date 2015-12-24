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

#include "ThemeManager.h"

Model_ThemeManager::Model_ThemeManager()
	: gotSaveErrors(false)
{}

void Model_ThemeManager::load() {
	this->themes.clear();
	std::string path = this->env->output_config_dir + "/" + "themes";

	DIR* dir = opendir(path.c_str());
	if (dir) {
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(dir))) {
			if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
				continue;
			}
			std::string currentFileName = path + "/" + entry->d_name;
			stat(currentFileName.c_str(), &fileProperties);
			this->themes.push_back(Model_Theme(currentFileName, "", entry->d_name));
		}
		closedir(dir);
	} else {
		throw FileReadException("cannot read the theme directory: " + path);
	}
}

Model_Theme& Model_ThemeManager::getTheme(std::string const& name) {
	for (std::list<Model_Theme>::iterator themeIter = this->themes.begin(); themeIter != this->themes.end(); themeIter++) {
		if (themeIter->name == name) {
			return *themeIter;
		}
	}
	throw ItemNotFoundException("getTheme: Theme not found: " + name, __FILE__, __LINE__);
}

bool Model_ThemeManager::themeExists(std::string const& name) {
	try {
		this->getTheme(name);
		return true;
	} catch (ItemNotFoundException const& e) {
	}
	return false;
}

std::string Model_ThemeManager::extractThemeName(std::string const& indexFile) {
	std::string themePath = this->env->output_config_dir + "/themes";
	if (indexFile.substr(0, themePath.size()) != themePath) {
		throw InvalidStringFormatException("theme index file path must contain '" + themePath + "' given path: '" + indexFile + "'", __FILE__, __LINE__);
	}
	int slashPos = indexFile.find('/', themePath.size() + 1);
	if (slashPos == -1) {
		throw InvalidStringFormatException("theme index file path incomplete", __FILE__, __LINE__);
	}

	int themeNameSize = slashPos - themePath.size() - 1;

	return indexFile.substr(themePath.size() + 1, themeNameSize);
}

std::string Model_ThemeManager::addThemePackage(std::string const& fileName) {
	int lastSlashPos = fileName.find_last_of('/');
	std::string name = lastSlashPos != -1 ? fileName.substr(lastSlashPos + 1) : fileName;
	int firstDotPos = name.find_first_of('.');
	name = firstDotPos != -1 ? name.substr(0, firstDotPos) : name;
	while (this->themeExists(name)) {
		name += "-";
	}
	this->themes.push_back(Model_Theme("", fileName, name));
	return name;
}

void Model_ThemeManager::removeTheme(Model_Theme const& theme) {
	for (std::list<Model_Theme>::iterator themeIter = this->themes.begin(); themeIter != this->themes.end(); themeIter++) {
		if (&*themeIter == &theme) {
			if (themeIter->directory != "") {
				this->removedThemes.push_back(*themeIter);
			}
			this->themes.erase(themeIter);
			break;
		}
	}
}

void Model_ThemeManager::save() {
	this->saveErrors = "";
	this->gotSaveErrors = false;

	std::string dirName = this->env->output_config_dir + "/themes";
	mkdir(dirName.c_str(), 0755);
	for (std::list<Model_Theme>::iterator themeIter = this->removedThemes.begin(); themeIter != this->removedThemes.end(); themeIter++) {
		themeIter->deleteThemeFiles(dirName);
	}
	this->removedThemes.clear();

	bool themesSaved = false;
	for (std::list<Model_Theme>::iterator themeIter = this->themes.begin(); themeIter != this->themes.end(); themeIter++) {
		if (themeIter->isModified) {
			try {
				themeIter->save(dirName);
			} catch (Exception const& e) {
				this->saveErrors += e + "\n";
				this->gotSaveErrors = true;
			}
			themesSaved = true;
		}
	}
	if (themesSaved) {
		this->load();
	}
}

std::string Model_ThemeManager::getThemePath() {
	return this->env->output_config_dir + "/themes";
}

bool Model_ThemeManager::hasSaveErrors() {
	return this->gotSaveErrors;
}

std::string Model_ThemeManager::getSaveErrors() {
	return this->saveErrors;
}
