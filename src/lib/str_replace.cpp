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

#include "str_replace.h"

std::string str_replace(const std::string &search, const std::string &replace, std::string subject) {
	size_t pos = 0;
	while (pos < subject.length() && (pos = subject.find(search, pos)) != -1){
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string escapeXml(std::string const& input) {
	return str_replace("<", "&lt;", str_replace("&", "&amp;", input));
}
