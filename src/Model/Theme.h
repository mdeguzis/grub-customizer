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

#ifndef MODEL_THEME_H_
#define MODEL_THEME_H_
#include <list>
#include "ThemeFile.h"
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include "../lib/Exception.h"
#include <archive.h>
#include <archive_entry.h>
#include <map>
#include "../lib/assert.h"

struct Model_Theme {
	std::string directory;
	std::string zipFile;
	std::list<Model_ThemeFile> files;
	std::string name;
	bool isModified;

	Model_Theme(std::string const& directory, std::string const& zipFile, std::string const& name);
	void load(std::string const& directory);
	void sort();
	void loadZipFile(std::string const& zipFile);
	std::string loadFileContent(std::string localFileName);
	std::string loadFileContentExternal(std::string const& externalPath);
	std::string getFullFileName(std::string localFileName);
	Model_ThemeFile& getFile(std::string localFileName);
	Model_ThemeFile& getFileByNewName(std::string localFileName);
	void removeFile(Model_ThemeFile const& file);
	void save(std::string const& baseDirectory);
	void renameFile(std::string const& oldName, std::string const& newName);
	bool hasConflicts(std::string const& localFilename);
	void deleteThemeFiles(std::string const& baseDirectory);
private:
	void removeSubdir();
	std::string extractLocalPath(std::string fullPath);
	std::string loadFileContentFromDirectory(std::string localFileName);
	std::string loadFileContentFromZip(std::string localFileName);
	void writeFile(Model_ThemeFile& file, std::string const& path);
	bool fileExists(std::string const& path);
	bool isDir(std::string const& path);
	void createFilePath(std::string const& path);
	void deleteDirectory(std::string const& path);
};


#endif /* MODEL_THEME_H_ */
