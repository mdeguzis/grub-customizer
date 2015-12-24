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

#ifndef CONTENT_PARSER_LINUX_H_
#define CONTENT_PARSER_LINUX_H_
#include "../regex.h"
#include "../../Model/DeviceMap.h"
#include "Abstract.h"

class ContentParser_Linux : public ContentParser_Abstract {
	static const char* _regex;
	Model_DeviceMap& deviceMap;
	std::string sourceCode;
public:
	ContentParser_Linux(Model_DeviceMap& deviceMap);
	void parse(std::string const& sourceCode);
	std::string buildSource() const;
	void buildDefaultEntry(std::string const& partition_uuid);
};

#endif /* CONTENT_PARSER_LINUX_H_ */
