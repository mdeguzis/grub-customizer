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

#ifndef GRUBENV_INCLUDED
#define GRUBENV_INCLUDED

#include <string>
#include <cstdio>
#include "MountTable.h"
#include <cstdlib>
#include <dirent.h>
#include <map>
#include <sys/stat.h>
#include "../lib/Trait/LoggerAware.h"
#include "SettingsStore.h"
#include "../lib/Exception.h"
#include "../lib/ArrayStructure.h"
#include "../lib/Type.h"
#include "../lib/FileSystem.h"

struct Model_Env : public Trait_LoggerAware {
public:
	enum Mode {
		GRUB_MODE,
		BURG_MODE
	};

	// application status flags:
	bool quit_requested;
	int activeThreadCount;
	bool modificationsUnsaved;
	std::string rootDeviceName;

	Model_Env();
	bool init(Model_Env::Mode mode, std::string const& dir_prefix);
	void loadFromFile(FILE* cfg_file, std::string const& dir_prefix);
	void save();
	void saveViewOptions(std::map<ViewOption, bool> const& options);
	std::map<ViewOption, bool> loadViewOptions();
	std::map<std::string, std::string> getProperties();
	void setProperties(std::map<std::string, std::string> const& props);
	std::list<std::string> getRequiredProperties();
	std::list<std::string> getValidProperties();
	bool check_cmd(std::string const& cmd, std::string const& cmd_prefix = "") const;
	bool check_dir(std::string const& dir) const;
	bool check_file(std::string const& file) const;
	std::string trim_cmd(std::string const& cmd) const;
	std::string getRootDevice();
	std::string cfg_dir, cfg_dir_noprefix, mkconfig_cmd, mkfont_cmd, cfg_dir_prefix, update_cmd, install_cmd, output_config_file, output_config_dir, output_config_dir_noprefix, settings_file, devicemap_file, mkdevicemap_cmd, cmd_prefix;
	bool burgMode;
	bool useDirectBackgroundProps; // Whether background settings should be set directly or by creating a desktop-base script
	std::list<Model_Env::Mode> getAvailableModes();
	void createBackup();
	operator ArrayStructure();
};

class Model_Env_Connection {
protected:
	Model_Env* env;
public:
	Model_Env_Connection() : env(NULL) {}

	void setEnv(Model_Env& env) {
		this->env = &env;
	}
};

#endif
