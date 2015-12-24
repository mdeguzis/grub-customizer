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

#include "DeviceMap.h"

Model_SmartFileHandle Model_DeviceMap::getFileHandle() const {
	Model_SmartFileHandle result;
	try {
		result.open(env->devicemap_file, "r", Model_SmartFileHandle::TYPE_FILE);
	} catch (FileReadException const& e) {
		if (env->check_cmd(env->mkdevicemap_cmd, env->cmd_prefix)) {
			result.open(env->mkdevicemap_cmd, "r", Model_SmartFileHandle::TYPE_COMMAND);
		} else {
			std::string staticMap = std::string() +
								 + "(hd0)\t/dev/sda\n"
								 + "(hd1)\t/dev/sdb\n"
								 + "(hd2)\t/dev/sdc\n"
								 + "(hd3)\t/dev/sdd\n"
								 + "(hd4)\t/dev/sde\n"
								 + "(hd5)\t/dev/sdf\n"
								 + "(hd6)\t/dev/sdg\n"
								 + "(hd7)\t/dev/sdh\n"
								 + "(hd8)\t/dev/sdi\n"
								 + "(hd9)\t/dev/sdj\n"
								 + "(hd10)\t/dev/sdk\n"
								 + "(hd11)\t/dev/sdl\n"
								 + "(hd12)\t/dev/sdm\n"
								 + "(hd13)\t/dev/sdn\n"
								 + "(hd14)\t/dev/sdo\n"
								 + "(hd15)\t/dev/sdp\n"
								 + "(hd16)\t/dev/sdq\n"
								 + "(hd17)\t/dev/sdr\n"
								 + "(hd18)\t/dev/sds\n"
								 + "(hd19)\t/dev/sdt\n"
								 + "(hd20)\t/dev/sdu\n"
								 + "(hd21)\t/dev/sdv\n"
								 + "(hd22)\t/dev/sdw\n"
								 + "(hd23)\t/dev/sdx\n"
								 + "(hd24)\t/dev/sdy\n"
								 + "(hd25)\t/dev/sdz\n";
			result.open(staticMap, "r", Model_SmartFileHandle::TYPE_STRING);
		}
	}
	return result;
}

Model_DeviceMap_PartitionIndex Model_DeviceMap::getHarddriveIndexByPartitionUuid(std::string partitionUuid) const {
	if (this->_cache.find(partitionUuid) != this->_cache.end()) {
		return this->_cache[partitionUuid];
	}

	Model_DeviceMap_PartitionIndex result;
	char deviceBuf[101];
	int size = readlink((this->env->cfg_dir_prefix + "/dev/disk/by-uuid/" + partitionUuid).c_str(), deviceBuf, 100);
	if (size == -1) { //if this didn't work, try to convert the uuid to uppercase
		for (std::string::iterator iter = partitionUuid.begin(); iter != partitionUuid.end(); iter++)
			*iter = std::toupper(*iter);
		size = readlink((this->env->cfg_dir_prefix + "/dev/disk/by-uuid/" + partitionUuid).c_str(), deviceBuf, 100);
	}
	if (size == -1) {
		return result; //abort with empty result
	}

	deviceBuf[size] = '\0';

	std::vector<std::string> regexResult = Regex::match("([^/.0-9]+)([0-9]+)$", deviceBuf);
	result.partNum = regexResult[2];

	std::string diskDevice = regexResult[1];

	Model_SmartFileHandle handle = this->getFileHandle();

	try {
		while (result.hddNum == "") {
			std::string row = handle.getRow();
			std::vector<std::string> rowMatch = Regex::match("^\\(hd([0-9]+)\\)[\t ]*(.*)$", row);
			std::string diskFile = rowMatch[2];

			int size = readlink(diskFile.c_str(), deviceBuf, 100); // if this is a link, follow it
			if (size != -1) {
				diskFile = std::string(deviceBuf, size);
			}

			int deviceStartPos = (int)diskFile.size() - diskDevice.size();
			if (deviceStartPos > 1 && diskFile.substr(deviceStartPos - 1) == "/" + diskDevice) {
				result.hddNum = rowMatch[1];
				break;
			}
		}
	} catch (EndOfFileException const& e) {
		//don't throw if we didn't find the mapped value
	}
	handle.close();

	this->_cache[partitionUuid] = result;
	return result;
}

void Model_DeviceMap::clearCache() {
	this->_cache.clear();
}
