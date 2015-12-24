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

#include "Entry.h"

Model_Entry_Row::Model_Entry_Row(FILE* sourceFile)
	: eof(false), is_loaded(true)
{
	this->eof = true; //will be set to false on the first loop run
	int c;
	while ((c = fgetc(sourceFile)) != EOF){
		this->eof = false;
		if (c != '\n'){
			this->text += char(c);
		}
		else {
			break;
		}
	}
}
Model_Entry_Row::Model_Entry_Row()
	: eof(false), is_loaded(false)
{}
Model_Entry_Row::operator bool(){
	return !eof && is_loaded;
}

Model_Entry::Model_Entry()
	: isValid(false), isModified(false), quote('\''), type(MENUENTRY)
{}

Model_Entry::Model_Entry(std::string name, std::string extension, std::string content, EntryType type)
	: name(name), extension(extension), content(content), isValid(true), type(type), isModified(false), quote('\'')
{}
Model_Entry::Model_Entry(FILE* sourceFile, Model_Entry_Row firstRow, Logger* logger, std::string* plaintextBuffer)
	: isValid(false), type(MENUENTRY), quote('\''), isModified(false)
{
	if (logger) {
		this->setLogger(*logger);
	}
	Model_Entry_Row row;
	while ((row = firstRow) || (row = Model_Entry_Row(sourceFile))){
		std::string rowText = ltrim(row.text);

		if (rowText.substr(0, 10) == "menuentry "){
			this->readMenuEntry(sourceFile, row);
			break;
		} else if (rowText.substr(0, 8) == "submenu ") {
			this->readSubmenu(sourceFile, row);
			break;
		} else {
			if (plaintextBuffer) {
				*plaintextBuffer += row.text + "\r\n";
			}
		}
		firstRow.eof = true; //disable firstRow to read the following config from file
	}
}

/**
 * reading a complete submenu (including subentries)
 */
void Model_Entry::readSubmenu(FILE* sourceFile, Model_Entry_Row firstRow) {
	std::string rowText = ltrim(firstRow.text);
	int endOfEntryName = rowText.find('"', 10);
	if (endOfEntryName == -1)
		endOfEntryName = rowText.find('\'', 10);
	std::string entryName = rowText.substr(9, endOfEntryName-9);

	*this = Model_Entry(entryName, "", "", SUBMENU);
	if (this->logger) {
		this->setLogger(*this->logger);
	}
	Model_Entry_Row row;
	while ((row = Model_Entry_Row(sourceFile))) {
		std::string rowText = ltrim(row.text);

		if (rowText.substr(0, 10) == "menuentry " || rowText.substr(0, 8) == "submenu "){
			this->subEntries.push_back(Model_Entry(sourceFile, row));
		} else if (trim(rowText) == "}") {
			this->isValid = true;
			break; //read only one submenu
		}
	}
}

/**
 * reading a menuentry
 */
void Model_Entry::readMenuEntry(FILE* sourceFile, Model_Entry_Row firstRow) {
	std::string rowText = ltrim(firstRow.text);
	char quote = '"';
	int endOfEntryName = rowText.find('"', 12);
	if (endOfEntryName == -1) {
		endOfEntryName = rowText.find('\'', 12);
		quote = '\'';
	}
	std::string entryName = rowText.substr(11, endOfEntryName-11);

	std::string extension = rowText.substr(endOfEntryName+1, rowText.length()-(endOfEntryName+1)-1);

	*this = Model_Entry(entryName, extension);
	if (this->logger) {
		this->setLogger(*this->logger);
	}
	this->quote = quote;

	// encapsulated menuentries must be ignored. This variable counts the encapsulation level.
	// We're starting inside of a menuentry!
	int depth = 1;


	Model_Entry_Row row;
	while ((row = Model_Entry_Row(sourceFile))){
		std::string rowText = ltrim(row.text);

		if (trim(rowText) == "}" && --depth == 0) {
			this->isValid = true;
			break; //read only one menuentry
		} else {
			if (rowText.substr(0, 10) == "menuentry ") {
				depth++;
			}
			this->content += row.text+"\n";
		}
	}
}

std::list<Model_Entry>& Model_Entry::getSubEntries() {
	return this->subEntries;
}

Model_Entry::operator bool() const {
	return isValid;
}

Model_Entry::operator ArrayStructure() const {
	ArrayStructure result;
	result["type"] = this->type;
	result["isValid"] = this->isValid;
	result["isModified"] = this->isModified;
	result["name"] = this->name;
	result["extension"] = this->extension;
	result["content"] = this->content;
	result["quote"] = this->quote;
	result["subEntries"].isArray = true;
	result["rulepointer"] = this;
	int i = 0;
	for (std::list<Model_Entry>::const_iterator iter = this->subEntries.begin(); iter != this->subEntries.end(); iter++) {
		result["subEntries"][i] = ArrayStructure(*iter);
		i++;
	}

	return result;
}

Model_Entry& Model_Entry::fromPtr(Entry* entry) {
	if (entry != NULL) {
		try {
			return dynamic_cast<Model_Entry&>(*entry);
		} catch (std::bad_cast const& e) {
		}
	}
	throw BadCastException("Model_Entry::fromPtr failed");
}

Model_Entry const& Model_Entry::fromPtr(Entry const* entry) {
	if (entry != NULL) {
		try {
			return dynamic_cast<Model_Entry const&>(*entry);
		} catch (std::bad_cast const& e) {
		}
	}
	throw BadCastException("Model_Entry::fromPtr [const] failed");
}
