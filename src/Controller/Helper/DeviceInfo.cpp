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

#include "DeviceInfo.h"

std::map<std::string, std::string> Controller_Helper_DeviceInfo::fetch(std::string const& menuEntryData, ContentParserFactory& contentParserFactory, Model_DeviceDataListInterface const& deviceDataList) {
	std::map<std::string, std::string> options;
	try {
		options = contentParserFactory.create(menuEntryData)->getOptions();
		if (options.find("partition_uuid") != options.end()) {
			// add device path
			for (Model_DeviceDataListInterface::const_iterator iter = deviceDataList.begin(); iter != deviceDataList.end(); iter++) {
				if (iter->second.find("UUID") != iter->second.end() && iter->second.at("UUID") == options["partition_uuid"]) {
					options["_deviceName"] = iter->first;
					break;
				}
			}
		}
	} catch (ParserNotFoundException const& e) {
		// nothing to do
	}
	return options;
}
