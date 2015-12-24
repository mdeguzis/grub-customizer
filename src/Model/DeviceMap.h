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

#ifndef GRUBDEVICEMAP_H_
#define GRUBDEVICEMAP_H_
#include "SmartFileHandle.h"
#include "Env.h"
#include "../lib/regex.h"
#include <map>
#include <unistd.h>

struct Model_DeviceMap_PartitionIndex {
	std::string hddNum, partNum;
	operator std::string();
};

class Model_DeviceMap :
	public Model_Env_Connection
{
	mutable std::map<std::string, Model_DeviceMap_PartitionIndex> _cache;
public:
	Model_SmartFileHandle getFileHandle() const;
	Model_DeviceMap_PartitionIndex getHarddriveIndexByPartitionUuid(std::string partitionUuid) const;
	void clearCache();
};

#endif
