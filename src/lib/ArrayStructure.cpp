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

#include "ArrayStructure.h"

ArrayStructureItem::ArrayStructureItem(std::string const& value) : isArray(false), value(value), isString(true) {}

std::string const& ArrayStructureItem::operator=(std::string const& value) {
	this->isArray = false;
	this->isString = true;
	return this->value = value;
}

std::string const& ArrayStructureItem::operator=(double const& value) {
	std::ostringstream str;
	str << value;
	this->isArray = false;
	this->isString = false;
	this->value = str.str();
	return this->value;
}

std::string const& ArrayStructureItem::operator=(bool const& value) {
	this->isArray = false;
	this->isString = false;
	this->value = value ? "true" : "false";
	return this->value;
}

std::string const& ArrayStructureItem::operator=(void const* value) {
	this->isArray = false;
	this->isString = false;
	if (value == NULL) {
		this->value = "NULL";
	} else {
		std::ostringstream str;
		str << value;
		this->value = str.str();
	}
	return this->value;
}

std::string const& ArrayStructureItem::operator=(char const* value) {
	this->isArray = false;
	this->isString = true;
	return this->value = value;
}

std::string const& ArrayStructureItem::operator=(int value) {
	std::ostringstream str;
	str << value;
	this->isArray = false;
	this->isString = false;
	this->value = str.str();
	return this->value;
}

std::map<std::string, ArrayStructureItem> const& ArrayStructureItem::operator=(std::map<std::string, ArrayStructureItem> const& value) {
	this->isArray = true;
	this->isString = false;
	this->subItems = value;
	return value;
}

ArrayStructureItem& ArrayStructureItem::operator[] (std::string const& key) {
	this->isArray = true;
	this->isString = false;
	return this->subItems[key];
}

ArrayStructureItem& ArrayStructureItem::operator[] (int const& key) {
	std::ostringstream str;
	str << key;
	ArrayStructureItem newItem;
	this->isArray = true;
	this->isString = false;
	return this->subItems[str.str()];
}

ArrayStructure::ArrayStructure() {}

ArrayStructure::ArrayStructure(std::list<std::string> const& source) {
	int i = 0;
	for (std::list<std::string>::const_iterator iter = source.begin(); iter != source.end(); iter++) {
		std::ostringstream str;
		str << i;
		ArrayStructureItem newItem;
		newItem.value = *iter;
		(*this)[str.str()] = newItem;
		i++;
	}
}

ArrayStructure::ArrayStructure(std::map<std::string, std::string> const& source) {
	for (std::map<std::string, std::string>::const_iterator iter = source.begin(); iter != source.end(); iter++) {
		(*this)[iter->first] = iter->second;
	}
}

void var_dump(std::map<std::string, ArrayStructureItem> const& data, std::string prefix, unsigned int indent) {
	if (prefix[0] == '!') {
		indent = 2;
	}

	if (data.size() == 0) {
		for (unsigned int i = 0; i < indent; i++) {
			std::cout << " ";
		}
		std::cout << "(empty)" << std::endl;
	}
	for (std::map<std::string, ArrayStructureItem>::const_iterator iter = data.begin(); iter != data.end(); iter++) {
		std::string indentStr = "";
		for (unsigned int i = 0; i < indent; i++) {
			indentStr += " ";
		}
		if (prefix[0] != '!') {
			std::cout << indentStr;
		}

		std::string key = prefix + iter->first;


		if (!iter->second.isArray) {
			std::string val = iter->second.value;
			if (iter->second.isString) {
				val = "\"" + val + "\"";
			}

			std::cout << "\"" << key << "\"" << " : " << str_replace("\n", "\n" + indentStr, val) << std::endl;
		} else {
			std::cout << "\"" << key << "\"" << " : Array(" << iter->second.subItems.size() << ")" << (iter->second.value != "" ? " [" + iter->second.value + "]" : "") << std::endl;
			if (iter->second.subItems.size()) {
				var_dump(iter->second.subItems, key[0] == '!' ? key + "." : "", indent + 2);
			}
		}
	}
}
