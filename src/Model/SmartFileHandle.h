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

#ifndef SMARTFILEHANDLE_H_
#define SMARTFILEHANDLE_H_
#include <cstdio>
#include <string>
#include "../lib/Exception.h"

class Model_SmartFileHandle {
public:
	enum Type {
		TYPE_FILE,
		TYPE_COMMAND,
		TYPE_STRING
	};
private:
	FILE* proc_or_file;
	Model_SmartFileHandle::Type type;
	std::string string; // content for TYPE_STRING
public:
	Model_SmartFileHandle();
	char getChar();
	std::string getRow();
	std::string getAll();
	void open(std::string const& cmd_or_file, std::string const& mode, Type type);
	void close();
};

#endif
