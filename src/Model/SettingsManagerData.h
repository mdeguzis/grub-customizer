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

#ifndef SETTING_MANAGER_DATASTORE_INCLUDED
#define SETTING_MANAGER_DATASTORE_INCLUDED
#include <sys/stat.h> //mkdir
#include "Env.h"
#include "../lib/Trait/LoggerAware.h"
#include "SettingsStore.h"
#include <sstream>
#include "../lib/str_replace.h"
#include <map>

class Model_SettingsManagerData :
	public Model_SettingsStore,
	public Trait_LoggerAware,
	public Model_Env_Connection
{
	bool _reloadRequired;
public:
	bool color_helper_required;
	std::string grubFont, oldFontFile;
	int grubFontSize;
	Model_SettingsManagerData();
	bool reloadRequired() const;
	static std::map<std::string, std::string> parsePf2(std::string const& fileName);
	static std::string getFontFileByName(std::string const& name);
	std::string mkFont(std::string fontFile = "", std::string outputDir = "");
	bool load();
	bool save();
	bool setValue(std::string const& name, std::string const& value);
	bool setIsActive(std::string const& name, bool value);
};

class Model_SettingsManagerData_Connection {
protected:
	Model_SettingsManagerData* settings;
public:
	Model_SettingsManagerData_Connection() : settings(NULL) {}
	void setSettingsManager(Model_SettingsManagerData& settings){
		this->settings = &settings;
	}
};
#endif
