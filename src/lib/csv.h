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

#ifndef CSV_H_
#define CSV_H_

#include <list>
#include <map>
#include <string>
#include <cstdio>
#include <stdexcept>
#include "str_replace.h"

class CsvReader {
	FILE* _file;
	std::list<std::string> _keys;
	std::list<std::string> _parseRow();
public:
	CsvReader(FILE* file);
	std::map<std::string, std::string> read();
};

class CsvWriter {
	FILE* _file;
	std::list<std::string> _keys;
	void _writeValue(std::string const& value);
public:
	CsvWriter(FILE* file);
	void write(std::map<std::string, std::string> const& data);
};


#endif /* CSV_H_ */
