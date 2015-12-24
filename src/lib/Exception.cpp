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

#include "Exception.h"

Exception::Exception(std::string const& message, std::string const& file, int line)
	: _message(message), _file(file), _line(line)
{
}

Exception::operator std::string() const {
	std::ostringstream out;
	out << "exception '" << typeid(*this).name()
	    << "'\n with message '" << this->_message;
	if (this->_file != "") {
		out << "'\n in " << this->_file << ":" << this->_line;
	}
	return out.str();
}

std::string Exception::getMessage() const {
	return this->_message;
}


Exception::operator bool() const {
	return this->_message != "";
}
