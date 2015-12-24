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

#include "csv.h"

CsvReader::CsvReader(FILE* file)
	: _file(file) {}


std::list<std::string> CsvReader::_parseRow() {
	std::list<std::string> dataRow;
	int c = 0;
	std::string field;
	bool quotesFound = false;
	bool inString = false;
	while ((c = fgetc(this->_file)) != EOF) {
		if (c == '"') {
			if (!inString) {
				inString = true;
			} else {
				if (quotesFound) { // double quotes
					field += '"';
					quotesFound = false;
				} else {
					quotesFound = true;
				}
			}
		} else if ((c == ';' || c == '\n') && (!inString || quotesFound)) {
			quotesFound = false;
			inString = false;
			dataRow.push_back(field);
			field = "";
			if (c == '\n') {
				break;
			}
		} else if (inString) {
			field += char(c);
			quotesFound = false;
		}
	}

	return dataRow;
}

std::map<std::string, std::string> CsvReader::read() {
	if (this->_keys.size() == 0) {
		this->_keys = this->_parseRow();
	}
	std::list<std::string> values = this->_parseRow();

	std::map<std::string, std::string> result;
	for (std::list<std::string>::iterator keyIter = this->_keys.begin(), valueIter = values.begin(); keyIter != this->_keys.end() && valueIter != values.end(); keyIter++, valueIter++) {
		result[*keyIter] = *valueIter;
	}

	return result;
}

CsvWriter::CsvWriter(FILE* file)
	: _file(file) {}


void CsvWriter::_writeValue(std::string const& value) {
	fputs(("\"" + str_replace("\"", "\"\"", value) + "\"").c_str(), this->_file);
}

void CsvWriter::write(std::map<std::string, std::string> const& data) {
	if (this->_keys.size() == 0) {
		for (std::map<std::string, std::string>::const_iterator cellIter = data.begin(); cellIter != data.end(); cellIter++) {
			this->_keys.push_back(cellIter->first);

			if (cellIter != data.begin()) {
				fputc(';', this->_file);
			}
			this->_writeValue(cellIter->first);
		}
		fputc('\n', this->_file);
	}

	for (std::list<std::string>::iterator keyIter = this->_keys.begin(); keyIter != this->_keys.end(); keyIter++) {
		std::string value;
		try {
			value = data.at(*keyIter);
		} catch (std::out_of_range const& e) {
			value = "";
		}
		if (keyIter != this->_keys.begin()) {
			fputc(';', this->_file);
		}
		this->_writeValue(value);
	}
	fputc('\n', this->_file);
}
