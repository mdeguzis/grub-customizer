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

#ifndef ARRAYSTRUCTURE_H_
#define ARRAYSTRUCTURE_H_
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <iostream>
#include "str_replace.h"

struct ArrayStructureItem {
public:
	ArrayStructureItem(std::string const& value = "");
	bool isArray;
	bool isString;
	std::string const& operator=(std::string const& value);
	std::string const& operator=(double const& value);
	std::string const& operator=(bool const& value);
	std::string const& operator=(void const* value);
	std::string const& operator=(char const* value);
	std::string const& operator=(int value);
	std::map<std::string, ArrayStructureItem> const& operator=(std::map<std::string, ArrayStructureItem> const& value);
	ArrayStructureItem& operator[] (std::string const& key);
	ArrayStructureItem& operator[] (int const& key);
	std::string value;
	std::map<std::string, ArrayStructureItem> subItems;
};

/**
 * used to dump objects implementing a cast to ArrayStructure
 *
 * Example of usage:
 * 	ArrayStructure test;
 *  test["x"] = "Model_Proxy";
 *  test["x"]["y"];
 *  test["x"]["z"] = (Foo*)NULL;
 *  test["x"]["a"] = 10.4;
 *  test["x"]["bool1"] = true;
 *  test["x"]["bool2"] = false;
 *  test["a"] = "eins";
 *  test["null-test"] = (Foo*)NULL;

 *  ArrayStructure subStructure;
 *  subStructure["BLUBB"] = true;
 *  test["SUB"] = subStructure;

 *  var_dump(test);
 */

class ArrayStructure : public std::map<std::string, ArrayStructureItem> {
public:
	ArrayStructure();
	ArrayStructure(std::list<std::string> const& source);
	ArrayStructure(std::map<std::string, std::string> const& source);
};

// prefix "!" = use prefixes
void var_dump(std::map<std::string, ArrayStructureItem> const& data, std::string prefix = "", unsigned int indent = 0);

#endif /* ARRAYSTRUCTURE_H_ */
