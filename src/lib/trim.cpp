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

#include "trim.h"

std::string ltrim(std::string string, std::string const& chars) {
	int first = string.find_first_not_of(chars);
	if (first != -1) {
		return string.substr(first);
	} else {
		return "";
	}
}

std::string rtrim(std::string string, std::string const& chars) {
	string = std::string(string.rbegin(), string.rend());
	string = ltrim(string, chars);
	string = std::string(string.rbegin(), string.rend());
	return string;
}

std::string trim(std::string string, std::string const& chars) {
	return rtrim(ltrim(string, chars));
}
