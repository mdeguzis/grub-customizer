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

#include "SmartFileHandle.h"

Model_SmartFileHandle::Model_SmartFileHandle()
	: type(TYPE_STRING), proc_or_file(NULL)
{
}

char Model_SmartFileHandle::getChar() {
	if (type == TYPE_STRING) {
		if (this->string.size()) {
			int c = (int) this->string[0];
			this->string = this->string.substr(1);
			return c;
		} else {
			throw EndOfFileException("end of file", __FILE__, __LINE__);
		}
	} else {
		int c = fgetc(this->proc_or_file);
		if (c != EOF)
			return c;
		else
			throw EndOfFileException("end of file", __FILE__, __LINE__);
	}
}
std::string Model_SmartFileHandle::getRow() {
	std::string result;
	int c;
	try {
		while ((c = this->getChar()) != EOF && c != '\n'){
			result += c;
		}
	} catch (EndOfFileException const& e) {
		if (result == "") {
			throw e;
		}
	}
	return result;
}
std::string Model_SmartFileHandle::getAll() {
	std::string result;
	int c;
	try {
		while ((c = this->getChar()) != EOF){
			result += c;
		}
	} catch (EndOfFileException const& e) {
		if (result == "") {
			throw e;
		}
	}
	return result;
}


void Model_SmartFileHandle::open(std::string const& cmd_or_file, std::string const& mode, Type type) {
	if (this->proc_or_file || this->string != "")
		throw HandleNotClosedException("handle not closed - cannot open", __FILE__, __LINE__);

	this->proc_or_file = NULL;
	this->string = "";

	switch (type) {
		case TYPE_STRING:
			this->string = cmd_or_file;
			break;
		case TYPE_COMMAND:
			this->proc_or_file = popen(cmd_or_file.c_str(), mode.c_str());
			break;
		case TYPE_FILE:
			this->proc_or_file = fopen(cmd_or_file.c_str(), mode.c_str());
			break;
		default:
			throw LogicException("unexpected type given");
	}

	if (this->proc_or_file || type == TYPE_STRING)
		this->type = type;
	else
		throw FileReadException("Cannot read the file/cmd: " + cmd_or_file, __FILE__, __LINE__);
}
void Model_SmartFileHandle::close() {
	if (this->type != TYPE_STRING && !this->proc_or_file)
		throw HandleNotOpenedException("handle not opened - cannot close", __FILE__, __LINE__);

	switch (type) {
		case TYPE_STRING:
			this->string = "";
			break;
		case TYPE_COMMAND:
			pclose(this->proc_or_file);
			break;
		case TYPE_FILE:
			fclose(this->proc_or_file);
			break;
		default:
			throw LogicException("unexpected type given");
	}
}
