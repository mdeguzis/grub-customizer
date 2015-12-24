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

#ifndef ENTRYNAME_H_
#define ENTRYNAME_H_

#include <string>
#include "../Model/Entry.h"

class Mapper_EntryName {
public:
	virtual inline ~Mapper_EntryName(){};
	virtual std::string map(Model_Entry const* sourceEntry, std::string const& defaultName, bool treatSubmenuAsPlaceholder) = 0;
};

class Mapper_EntryName_Connection {
protected:
	Mapper_EntryName* entryNameMapper;
public:
	Mapper_EntryName_Connection() : entryNameMapper(NULL) {}
	void setEntryNameMapper(Mapper_EntryName& mapper) {
		this->entryNameMapper = &mapper;
	}
};

#endif /* ENTRYNAME_H_ */
