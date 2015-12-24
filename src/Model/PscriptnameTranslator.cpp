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

#include "PscriptnameTranslator.h"

std::string Model_PscriptnameTranslator::decode(std::string const& input){
	std::string result = input;
	int last_nonnum_pos = input.find_last_not_of("0123456789");
	if (last_nonnum_pos != -1 && result[last_nonnum_pos] == '~' && last_nonnum_pos != input.length()-1)
		result = result.substr(0, last_nonnum_pos);
	return result;
}

std::string Model_PscriptnameTranslator::encode(std::string const& input, int x){
	std::ostringstream out;
	out << input;
	int last_nonnum_pos = input.find_last_not_of("0123456789");
	if (x != 0 || (last_nonnum_pos != -1 && input[last_nonnum_pos] == '~') && last_nonnum_pos != input.length()-1)
		out << "~" << x;
	return out.str();
}
