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

#include "SettingsStore.h"

Model_SettingsStore_Row::Model_SettingsStore_Row() : isActive(true), hasExportPrefix(false), isSetting(true) {}

void Model_SettingsStore_Row::validate(){
	isActive = false;
	hasExportPrefix = false;
	isSetting = false;

	// trim name and value
	name = name.substr(name.find_first_not_of(' ') == -1 ? 0 : name.find_first_not_of(' ')); // ltrim
	name = name.substr(0, name.find_last_not_of(' ') + 1); // rtrim
	value = value.substr(value.find_first_not_of(' ') == -1 ? 0 : value.find_first_not_of(' ')); // ltrim
	value = value.substr(0, value.find_last_not_of(' ') + 1); // rtrim

	if (name != "" && value != "" && (name.length() < 2 || name.substr(0,2) != "# ")){
		isSetting = true;
		if (name[0] != '#')
			isActive = true;
		else
			name = name.substr(1);

		if (name.length() > 7 && name.substr(0,7) == "export "){
			hasExportPrefix = true;
			int pos = name.find_first_not_of(' ', 7);
			name = name.substr(pos != -1 ? pos : 7); // try to use trimmed value, but use pos 7 if there's an error
		}
	}
	else {
		name = "";
		value = "";
	}
}

std::string Model_SettingsStore_Row::getOutput(){
	if (isSetting) {
		if (name != "") {
			return (isActive ? "" : "#")+std::string(hasExportPrefix ? "export " : "")+name+"=\""+value+"\""+(comment != "" ? " #"+comment : "");
		} else {
			return "#UNNAMED_OPTION=\""+value+"\""; //unnamed options would destroy the grub confuguration
		}
	} else {
		return plaintext;
	}
}


Model_SettingsStore::Model_SettingsStore(FILE* source) {
	if (source) {
		this->load(source);
	}
}


std::list<Model_SettingsStore_Row>::iterator Model_SettingsStore::begin(bool jumpOverPlaintext){
	std::list<Model_SettingsStore_Row>::iterator iter = settings.begin();
	if (!iter->isSetting && jumpOverPlaintext)
		iter_to_next_setting(iter);
	return iter;
}
std::list<Model_SettingsStore_Row>::iterator Model_SettingsStore::end(){
	return settings.end();
}
void Model_SettingsStore::iter_to_next_setting(std::list<Model_SettingsStore_Row>::iterator& iter){
	iter++;
	while (iter != settings.end()){
		if (iter->isSetting)
			break;
		else
			iter++;
	}
}
std::string Model_SettingsStore::getValue(std::string const& name){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name)
			return iter->value;
	}
	return "";
}

bool Model_SettingsStore::setValue(std::string const& name, std::string const& value){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){
			if (iter->value != value){ //only set when the new value is really new
				iter->value = value;
			}
			return true;
		}
	}

	settings.push_back(Model_SettingsStore_Row());
	settings.back().name = name;
	settings.back().value = value;
	settings.back().validate();

	return false;
}

std::string Model_SettingsStore::addNewItem(){
	Model_SettingsStore_Row newRow;
	newRow.name = "";
	settings.push_back(newRow);
	return newRow.name;
}

void Model_SettingsStore::removeItem(std::string const& name){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (iter->name == name){
			settings.erase(iter);
			break; //must break because the iterator is invalid now!
		}
	}
}

void Model_SettingsStore::renameItem(std::string const& old_name, std::string const& new_name){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (iter->name == old_name){
			iter->name = new_name;
			break; //must break because the iterator is invalid now!
		}
	}
}

bool Model_SettingsStore::isActive(std::string const& name, bool checkValueToo){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name)
			return iter->isActive && (!checkValueToo || iter->value != "false");
	}
	return false;
}
bool Model_SettingsStore::setIsActive(std::string const& name, bool value){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){
			if (iter->isActive != value){
				iter->isActive = value;
			}
			return true;
		}
	}
	return false;
}

bool Model_SettingsStore::setIsExport(std::string const& name, bool isExport){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){
			if (iter->hasExportPrefix != isExport){
				iter->hasExportPrefix = isExport;
			}
			return true;
		}
	}
	return false;
}

void Model_SettingsStore::clear(){
	this->settings.clear();
}

void Model_SettingsStore::load(FILE* source) {
	std::string row;
	int c;
	int step = 0; //0: name parsing, 1: value parsing
	bool inQuotes = false;
	char quoteChar;
	settings.push_back(Model_SettingsStore_Row());
	while ((c = fgetc(source)) != EOF){
		if (c == '\n'){
			settings.back().validate();
			settings.push_back(Model_SettingsStore_Row());
			inQuotes = false;
			step = 0;
		}
		else {
			settings.back().plaintext += c;
			if (c == '"' && (!inQuotes || quoteChar == c)){
				inQuotes = !inQuotes;
				quoteChar = '"';
			}
			else if (c == '\'' && (!inQuotes || quoteChar == c)){
				inQuotes = !inQuotes;
				quoteChar = '\'';
			}
			else {
				if (step == 0 && c == '=' && !inQuotes)
					step = 1;
				else if (step == 0)
					settings.back().name += c;
				else if (step == 1)
					settings.back().value += c;
			}
		}
	}
	if (settings.size() > 0)
		settings.back().validate();
}

void Model_SettingsStore::save(FILE* target) {
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(false); iter != this->end(); iter++){
		if (iter != this->begin(false)) {
			fputs("\n", target);
		}
		fputs((iter->getOutput()).c_str(), target);
	}
}
