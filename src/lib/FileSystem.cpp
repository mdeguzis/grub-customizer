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

#include "FileSystem.h"

void FileSystem::copy(std::string const& srcPath, std::string const& destPath, bool recursive, std::list<std::string> ignoreList) {
	if (std::find(ignoreList.begin(), ignoreList.end(), srcPath) != ignoreList.end()) {
		return;
	}
	struct stat fileProperties;
	stat(srcPath.c_str(), &fileProperties);

	if (S_ISDIR(fileProperties.st_mode)) {
		if (!recursive) {
			throw LogicException(srcPath + " is an directory but copying shouldn't be recursive (recursive=false)", __FILE__, __LINE__);
		}
		DIR* dir = opendir(srcPath.c_str());
		if (dir) {
			struct dirent *entry;
			mkdir(destPath.c_str(), fileProperties.st_mode);
			while ((entry = readdir(dir))) {
				if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
					continue;
				}
				this->copy(srcPath + "/" + entry->d_name, destPath + "/" + entry->d_name, recursive, ignoreList);
			}
			closedir(dir);
		} else {
			throw FileReadException("cannot read directory: " + srcPath, __FILE__, __LINE__);
		}
	} else {
		std::ifstream src(srcPath.c_str(), std::ios::binary);
		std::ofstream dst(destPath.c_str(), std::ios::binary);
		dst << src.rdbuf();
	}
}
