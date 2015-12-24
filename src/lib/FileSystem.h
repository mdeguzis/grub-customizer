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

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include "Exception.h"
#include <fstream>
#include <list>
#include <algorithm>

class FileSystem {
public:
	void copy(std::string const& srcPath, std::string const& destPath, bool recursive = false, std::list<std::string> ignoreList = std::list<std::string>());
};

#endif /* FILESYSTEM_H_ */
