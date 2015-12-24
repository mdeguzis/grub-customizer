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

#include "Env.h"

Model_Env::Model_Env()
	: burgMode(false),
	  useDirectBackgroundProps(false),
	  modificationsUnsaved(false),
	  quit_requested(false),
	  activeThreadCount(0)
{}

bool Model_Env::init(Model_Env::Mode mode, std::string const& dir_prefix){
	useDirectBackgroundProps = false;
	this->cmd_prefix = dir_prefix != "" ? "chroot '"+dir_prefix+"' " : "";
	this->cfg_dir_prefix = dir_prefix;
	std::string output_config_file_noprefix;
	switch (mode){
	case BURG_MODE: {
		this->burgMode = true;
		FILE* burg_cfg = fopen((dir_prefix + "/etc/grub-customizer/burg.cfg").c_str(), "r");
		if (burg_cfg) { // try to use the settings file ...
			this->log("using custom BURG configuration", Logger::INFO);
			this->loadFromFile(burg_cfg, dir_prefix);
			fclose(burg_cfg);
		} else { // otherwise use the built-in config
			this->mkconfig_cmd = "burg-mkconfig";
			this->install_cmd = "burg-install";
			this->mkfont_cmd = "burg-mkfont";
			this->mkdevicemap_cmd = "burg-mkdevicemap --device-map=/dev/stdout";
			this->cfg_dir = dir_prefix+"/etc/burg.d";
			this->cfg_dir_noprefix = "/etc/burg.d";
			this->output_config_dir = dir_prefix+"/boot/burg";
			this->output_config_dir_noprefix = "/boot/burg";
			this->output_config_file = dir_prefix+"/boot/burg/burg.cfg";
			this->settings_file = dir_prefix+"/etc/default/burg";
			this->devicemap_file = dir_prefix+"/boot/burg/device.map";
		}
		} break;
	case GRUB_MODE: {
		this->burgMode = false;
		FILE* grub_cfg = fopen((dir_prefix + "/etc/grub-customizer/grub.cfg").c_str(), "r");
		if (grub_cfg) { // try to use the settings file ...
			this->log("using custom Grub2 configuration", Logger::INFO);
			this->loadFromFile(grub_cfg, dir_prefix);
			fclose(grub_cfg);
		} else { // otherwise use the built-in config
			this->mkconfig_cmd = "grub-mkconfig";
			this->install_cmd = "grub-install";
			this->mkfont_cmd = "grub-mkfont";
			this->mkdevicemap_cmd = "grub-mkdevicemap --device-map=/dev/stdout";
			this->cfg_dir = dir_prefix+"/etc/grub.d";
			this->cfg_dir_noprefix = "/etc/grub.d";
			this->output_config_dir = dir_prefix+"/boot/grub";
			this->output_config_dir_noprefix = "/boot/grub";
			this->output_config_file = dir_prefix+"/boot/grub/grub.cfg";
			this->settings_file = dir_prefix+"/etc/default/grub";
			this->devicemap_file = dir_prefix+"/boot/grub/device.map";
		}
		} break;
	}
	
	this->update_cmd = this->mkconfig_cmd + " -o \"" + this->output_config_file.substr(dir_prefix.size()) + "\"";
	this->log("update-CMD: " + this->update_cmd, Logger::INFO);

	bool is_valid = check_cmd(mkconfig_cmd, cmd_prefix) && check_cmd(update_cmd, cmd_prefix) && check_cmd(install_cmd, cmd_prefix) && check_dir(cfg_dir);
	
	this->mkconfig_cmd = cmd_prefix+this->mkconfig_cmd;
	this->update_cmd = cmd_prefix+this->update_cmd;
	this->install_cmd = cmd_prefix+this->install_cmd;
	this->mkfont_cmd = cmd_prefix+this->mkfont_cmd;
	this->mkdevicemap_cmd = cmd_prefix+this->mkdevicemap_cmd;
	
	return is_valid;
}

void Model_Env::loadFromFile(FILE* cfg_file, std::string const& dir_prefix) {
	Model_SettingsStore ds(cfg_file);
	this->mkconfig_cmd = ds.getValue("MKCONFIG_CMD");
	this->install_cmd = ds.getValue("INSTALL_CMD");
	this->mkfont_cmd = ds.getValue("MKFONT_CMD");
	this->mkdevicemap_cmd = ds.getValue("MKDEVICEMAP_CMD");
	this->cfg_dir = dir_prefix + ds.getValue("CFG_DIR");
	this->cfg_dir_noprefix = ds.getValue("CFG_DIR");
	this->output_config_dir = dir_prefix + ds.getValue("OUTPUT_DIR");
	this->output_config_dir_noprefix = ds.getValue("OUTPUT_DIR");
	this->output_config_file = dir_prefix + ds.getValue("OUTPUT_FILE");
	this->settings_file = dir_prefix + ds.getValue("SETTINGS_FILE");
	this->devicemap_file = dir_prefix + ds.getValue("DEVICEMAP_FILE");
}

void Model_Env::save() {
	FILE* cfg_file = NULL;
	DIR* dir = opendir((cfg_dir_prefix + "/etc/grub-customizer").c_str());
	if (dir) {
		closedir(dir);
	} else {
		int res = mkdir((cfg_dir_prefix + "/etc/grub-customizer").c_str(), 0755);
		if (res != 0) {
			throw FileSaveException("cannot save the config file (directory creation)", __FILE__, __LINE__);
		}
	}
	if (this->burgMode) {
		cfg_file = fopen((cfg_dir_prefix + "/etc/grub-customizer/burg.cfg").c_str(), "w");
	} else {
		cfg_file = fopen((cfg_dir_prefix + "/etc/grub-customizer/grub.cfg").c_str(), "w");
	}
	if (!cfg_file) {
		throw FileSaveException("cannot save the config file (file creation)", __FILE__, __LINE__);
	}
	Model_SettingsStore ds;
	std::map<std::string, std::string> props = this->getProperties();
	for (std::map<std::string, std::string>::iterator iter = props.begin(); iter != props.end(); iter++) {
		ds.setValue(iter->first, iter->second);
	}
	ds.save(cfg_file);
	fclose(cfg_file);
}

void Model_Env::saveViewOptions(std::map<ViewOption, bool> const& options) {
	FILE* cfg_file = NULL;
	DIR* dir = opendir((cfg_dir_prefix + "/etc/grub-customizer").c_str());
	if (dir) {
		closedir(dir);
	} else {
		int res = mkdir((cfg_dir_prefix + "/etc/grub-customizer").c_str(), 0755);
		if (res != 0) {
			throw FileSaveException("cannot save the view config file (directory creation)", __FILE__, __LINE__);
		}
	}
	cfg_file = fopen((cfg_dir_prefix + "/etc/grub-customizer/viewOptions.cfg").c_str(), "w");
	if (!cfg_file) {
		throw FileSaveException("cannot save the view config file (file creation)", __FILE__, __LINE__);
	}
	Model_SettingsStore ds;
	for (std::map<ViewOption, bool>::const_iterator iter = options.begin(); iter != options.end(); iter++) {
		std::string optionText = "";
		switch (iter->first) {
		case VIEW_SHOW_DETAILS: optionText = "SHOW_DETAILS"; break;
		case VIEW_SHOW_HIDDEN_ENTRIES: optionText = "SHOW_HIDDEN_ENTRIES"; break;
		case VIEW_GROUP_BY_SCRIPT: optionText = "GROUP_BY_SCRIPT"; break;
		case VIEW_SHOW_PLACEHOLDERS: optionText = "SHOW_PLACEHOLDERS"; break;
		default: throw LogicException("option mapping failed");
		}
		ds.setValue(optionText, iter->second ? "true" : "false");
	}
	ds.save(cfg_file);
	fclose(cfg_file);
}

std::map<ViewOption, bool> Model_Env::loadViewOptions() {
	FILE* file = fopen((cfg_dir_prefix + "/etc/grub-customizer/viewOptions.cfg").c_str(), "r");
	if (file == NULL) {
		throw FileReadException("viewOptions not found");
	}
	Model_SettingsStore ds(file);
	fclose(file);
	std::map<ViewOption, bool> result;
	if (ds.getValue("SHOW_DETAILS") != "") {
		result[VIEW_SHOW_DETAILS] = ds.getValue("SHOW_DETAILS") == "true";
	} else {
		result[VIEW_SHOW_DETAILS] = true;
	}
	if (ds.getValue("SHOW_HIDDEN_ENTRIES") != "") {
		result[VIEW_SHOW_HIDDEN_ENTRIES] = ds.getValue("SHOW_HIDDEN_ENTRIES") == "true";
	} else {
		result[VIEW_SHOW_HIDDEN_ENTRIES] = false;
	}
	if (ds.getValue("GROUP_BY_SCRIPT") != "") {
		result[VIEW_GROUP_BY_SCRIPT] = ds.getValue("GROUP_BY_SCRIPT") == "true";
	} else {
		result[VIEW_GROUP_BY_SCRIPT] = false;
	}
	if (ds.getValue("SHOW_PLACEHOLDERS") != "") {
		result[VIEW_SHOW_PLACEHOLDERS] = ds.getValue("SHOW_PLACEHOLDERS") == "true";
	} else {
		result[VIEW_SHOW_PLACEHOLDERS] = false;
	}
	return result;
}

std::map<std::string, std::string> Model_Env::getProperties() {
	std::map<std::string, std::string> result;
	result["MKCONFIG_CMD"] = this->mkconfig_cmd.substr(this->cmd_prefix.size());
	result["INSTALL_CMD"] = this->install_cmd.substr(this->cmd_prefix.size());
	result["MKFONT_CMD"] = this->mkfont_cmd.substr(this->cmd_prefix.size());
	result["MKDEVICEMAP_CMD"] = this->mkdevicemap_cmd.substr(this->cmd_prefix.size());
	result["CFG_DIR"] = this->cfg_dir_noprefix;
	result["OUTPUT_DIR"] = this->output_config_dir.substr(this->cfg_dir_prefix.size());
	result["OUTPUT_FILE"] = this->output_config_file.substr(this->cfg_dir_prefix.size());
	result["SETTINGS_FILE"] = this->settings_file.substr(this->cfg_dir_prefix.size());
	result["DEVICEMAP_FILE"] = this->devicemap_file.substr(this->cfg_dir_prefix.size());

	return result;
}

void Model_Env::setProperties(std::map<std::string, std::string> const& props) {
	this->mkconfig_cmd = this->cmd_prefix + props.at("MKCONFIG_CMD");
	this->install_cmd = this->cmd_prefix + props.at("INSTALL_CMD");
	this->mkfont_cmd = this->cmd_prefix + props.at("MKFONT_CMD");
	this->mkdevicemap_cmd = this->cmd_prefix + props.at("MKDEVICEMAP_CMD");
	this->cfg_dir_noprefix = props.at("CFG_DIR");
	this->cfg_dir = this->cfg_dir_prefix + props.at("CFG_DIR");
	this->output_config_dir_noprefix = props.at("OUTPUT_DIR");
	this->output_config_dir = this->cfg_dir_prefix + props.at("OUTPUT_DIR");
	this->output_config_file = this->cfg_dir_prefix + props.at("OUTPUT_FILE");
	this->settings_file = this->cfg_dir_prefix + props.at("SETTINGS_FILE");
	this->devicemap_file = this->cfg_dir_prefix + props.at("DEVICEMAP_FILE");
}

std::list<std::string> Model_Env::getRequiredProperties() {
	std::list<std::string> result;
	result.push_back("MKCONFIG_CMD");
	result.push_back("INSTALL_CMD");
	result.push_back("CFG_DIR");
	return result;
}

std::list<std::string> Model_Env::getValidProperties() {
	std::list<std::string> result;
	if (this->check_cmd(this->mkconfig_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("MKCONFIG_CMD");
	}
	if (this->check_cmd(this->install_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("INSTALL_CMD");
	}
	if (this->check_cmd(this->mkfont_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("MKFONT_CMD");
	}
	if (this->check_cmd(this->mkdevicemap_cmd.substr(this->cmd_prefix.size()), this->cmd_prefix)) {
		result.push_back("MKDEVICEMAP_CMD");
	}
	if (this->check_dir(this->cfg_dir)) {
		result.push_back("CFG_DIR");
	}
	if (this->check_dir(this->output_config_dir)) {
		result.push_back("OUTPUT_DIR");
	}
	if (this->check_file(this->output_config_file)) {
		result.push_back("OUTPUT_FILE");
	}
	if (this->check_file(this->settings_file)) {
		result.push_back("SETTINGS_FILE");
	}
	if (this->check_file(this->devicemap_file)) {
		result.push_back("DEVICEMAP_FILE");
	}
	return result;
}

bool Model_Env::check_cmd(std::string const& cmd, std::string const& cmd_prefix) const {
	this->log("checking the " + this->trim_cmd(cmd) + " command… ", Logger::INFO);
	FILE* proc = popen((cmd_prefix + " which " + this->trim_cmd(cmd) + " 2>&1").c_str(), "r");
	std::string output;
	int c;
	while ((c = fgetc(proc)) != EOF) {
		if (c != '\n') {
			output += char(c);
		}
	}
	bool result = pclose(proc) == 0;
	if (result == true) {
		this->log("found at: " + output, Logger::INFO);
	} else {
		this->log("not found", Logger::INFO);
	}
	return result;
}

bool Model_Env::check_dir(std::string const& dir_str) const {
	DIR* dir = opendir(dir_str.c_str());
	if (dir){
		closedir(dir);
		return true;
	}
	return false;
}

bool Model_Env::check_file(std::string const& file_str) const {
	FILE* file = fopen(file_str.c_str(), "r");
	if (file){
		fclose(file);
		return true;
	}
	return false;
}

std::string Model_Env::trim_cmd(std::string const& cmd) const {
	int firstSpace = cmd.find_first_of(' ');
	if (firstSpace != -1) {
		return cmd.substr(0, firstSpace);
	} else {
		return cmd;
	}
}

std::string Model_Env::getRootDevice(){
	FILE* mtabFile = fopen("/etc/mtab", "r");
	Model_MountTable mtab;
	if (mtabFile){
		mtab.loadData(mtabFile, "");
		fclose(mtabFile);
	}
	return mtab.getEntryByMountpoint(cfg_dir_prefix == "" ? "/" : cfg_dir_prefix).device;
}

std::list<Model_Env::Mode> Model_Env::getAvailableModes(){
	std::list<Mode> result;
	if (this->init(Model_Env::BURG_MODE, this->cfg_dir_prefix))
		result.push_back(Model_Env::BURG_MODE);
	if (this->init(Model_Env::GRUB_MODE, this->cfg_dir_prefix))
		result.push_back(Model_Env::GRUB_MODE);
	return result;
}

void Model_Env::createBackup() {
	std::string backupDir = this->cfg_dir + "/backup";
	DIR* dirChk = opendir(backupDir.c_str());
	if (dirChk) {
		// if backup already exists - dont't create or update. Should only contain the initial config.
		closedir(dirChk);
	} else {
		mkdir(backupDir.c_str(), 0755);

		std::list<std::string> ignoreList;
		ignoreList.push_back(backupDir);

		FileSystem fileSystem;
		fileSystem.copy(this->cfg_dir, backupDir + "/etc_grub_d", true, ignoreList);
		fileSystem.copy(this->output_config_dir, backupDir + "/boot_grub", true, ignoreList);
		fileSystem.copy(this->settings_file, backupDir + "/default_grub", true, ignoreList);

		FILE* restoreHowto = fopen((backupDir + "/RESTORE_INSTRUCTIONS").c_str(), "w");
		fputs("How to restore this backup\n", restoreHowto);
		fputs("--------------------------\n", restoreHowto);
		fputs(" * make sure you have root permissions (`gksu nautilus` or `sudo -s` on command line) otherwise you won't be able to copy the files\n", restoreHowto);
		fputs(" * to fix an unbootable configuration, just copy:\n", restoreHowto);
		fputs(("     * '" + backupDir + "/boot_grub' to '" + this->output_config_dir + "'\n").c_str(), restoreHowto);
		fputs(" * to reset the whole configuration (if it cannot be fixed by using grub customizer), also copy these files:\n", restoreHowto);
		fputs(("     * '" + backupDir + "/etc_grub_d' to '" + this->cfg_dir + "'\n").c_str(), restoreHowto);
		fputs(("     * '" + backupDir + "/default_grub' to '" + this->settings_file + "'\n").c_str(), restoreHowto);
		fclose(restoreHowto);
	}
}

Model_Env::operator ArrayStructure() {
	ArrayStructure result;
	result["cfg_dir"] = this->cfg_dir;
	result["cfg_dir_noprefix"] = this->cfg_dir_noprefix;
	result["mkconfig_cmd"] = this->mkconfig_cmd;
	result["mkfont_cmd"] = this->mkfont_cmd;
	result["cfg_dir_prefix"] = this->cfg_dir_prefix;
	result["install_cmd"] = this->install_cmd;
	result["output_config_file"] = this->output_config_file;
	result["output_config_dir"] = this->output_config_dir;
	result["settings_file"] = this->settings_file;
	result["devicemap_file"] = this->devicemap_file;
	result["mkdevicemap_cmd"] = this->mkdevicemap_cmd;
	result["cmd_prefix"] = this->cmd_prefix;
	result["burgMode"] = this->burgMode;
	result["useDirectBackgroundProps"] = this->useDirectBackgroundProps;
	result["quit_requested"] = this->quit_requested;
	result["activeThreadCount"] = this->activeThreadCount;
	result["modificationsUnsaved"] = this->modificationsUnsaved;
	result["rootDeviceName"] = this->rootDeviceName;
	return result;
}
