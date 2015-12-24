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

#include "SettingsManagerData.h"

Model_SettingsManagerData::Model_SettingsManagerData()
	: _reloadRequired(false), color_helper_required(false), grubFontSize(-1)
{
}

bool Model_SettingsManagerData::reloadRequired() const {
	return this->_reloadRequired;
}

std::map<std::string, std::string> Model_SettingsManagerData::parsePf2(std::string const& fileName) {
	std::map<std::string, std::string> result;
	FILE* file = fopen(fileName.c_str(), "rb");
	if (file) {
		while (true) {
			char nameBuf [5]; // stores 4 bytes of data (possible \0 at pos 5)
			if (fgets(nameBuf, 5, file) == NULL || nameBuf == std::string("CHIX") || nameBuf == std::string("DATA")) {
				break;
			}

			char sizeBuf [5];
			fgets(sizeBuf, 5, file);
			unsigned int size = sizeBuf[3] + (sizeBuf[2] << 8) + (sizeBuf[1] << 16) + (sizeBuf[0] << 24);

			char* contentBuf = new char[size + 1];

			fgets(contentBuf, size + 1, file);

			result[nameBuf] = contentBuf;
		}
		fclose(file);
	}
	return result;
}

std::string Model_SettingsManagerData::getFontFileByName(std::string const& name) {
	std::string result;
	std::string translatedName = name;
	int lastWhitespacePos = translatedName.find_last_of(' ');
	if (lastWhitespacePos != -1) {
		translatedName[lastWhitespacePos] = ':';
	}
	translatedName = str_replace(" Bold", ":Bold", translatedName);
	translatedName = str_replace(" Italic", ":Italic", translatedName);
	translatedName = str_replace(" Medium", ":Medium", translatedName);
	translatedName = str_replace(" Oblique", ":Oblique", translatedName);
	translatedName = str_replace(" Regular", ":Regular", translatedName);

	std::string cmd = "fc-match -f '%{file[0]}' '" + translatedName + "'";
	FILE* proc = popen(cmd.c_str(), "r");
	int c;
	while ((c = getc(proc)) != EOF) {
		result += char(c);
	}
	pclose(proc);
	return result;
}

std::string Model_SettingsManagerData::mkFont(std::string fontFile, std::string outputPath) {
	int fontSize = -1;
	if (fontFile == "") {
		fontFile = Model_SettingsManagerData::getFontFileByName(this->grubFont);
		fontSize = this->grubFontSize;
	}
	std::string sizeParam;
	if (fontSize != -1) {
		std::ostringstream stream;
		stream << fontSize;
		sizeParam = " --size='" + stream.str() + "'";
		if (fontSize > 72) {
			this->log("Error: font too large: " + stream.str() + "!", Logger::ERROR);
			return ""; // fehler
		}
	}
	outputPath = outputPath != "" ? outputPath : this->env->output_config_dir_noprefix + "/unicode.pf2";
	std::string cmd = this->env->mkfont_cmd + " --output='" + str_replace("'", "\\'", outputPath) + "'" + sizeParam + " '" + str_replace("'", "\\'", fontFile) + "' 2>&1";
	this->log("running " + cmd, Logger::INFO);
	FILE* mkfont_proc = popen(cmd.c_str(), "r");
	int c;
//	std::string row = "";
	while ((c = fgetc(mkfont_proc)) != EOF) {
//		if (c == '\n') {
//			this->log("MKFONT: " + row, Logger::INFO);
//		} else {
//			row += char(c);
//		}
	}
	int result = pclose(mkfont_proc);
	if (result != 0) {
		this->log("error running " + this->env->mkfont_cmd, Logger::ERROR);
		return "";
	}
	this->setValue("GRUB_FONT", outputPath);
	return outputPath;
}

bool Model_SettingsManagerData::load(){
	settings.clear();

	FILE* file = fopen(this->env->settings_file.c_str(), "r");
	if (file){
		Model_SettingsStore::load(file);
		this->grubFontSize = -1;
		if (this->getValue("GRUB_FONT") != "") {
			this->oldFontFile = this->getValue("GRUB_FONT");
			this->log("parsing " + this->getValue("GRUB_FONT"), Logger::INFO);
			this->grubFont = Model_SettingsManagerData::parsePf2(this->env->cfg_dir_prefix + this->getValue("GRUB_FONT"))["NAME"];
			this->log("result " + this->grubFont, Logger::INFO);
			this->removeItem("GRUB_FONT");
		}

		fclose(file);
		return true;
	}
	else
		return false;
}

bool Model_SettingsManagerData::save(){
	const char* background_script = "\
#! /bin/sh -e\n\
# Name of this script: 'grub_background.sh'\n\
\n\
   WALLPAPER=\"/usr/share/images/desktop-base/background\"\n\
   COLOR_NORMAL=\"light-gray/black\"\n\
   COLOR_HIGHLIGHT=\"magenta/black\"\n\
\n\
if [ \"${GRUB_MENU_PICTURE}\" ] ; then\n\
   echo \"using custom appearance settings\" >&2\n\
   [ \"${GRUB_COLOR_NORMAL}\" ] && COLOR_NORMAL=\"${GRUB_COLOR_NORMAL}\"\n\
   [ \"${GRUB_COLOR_HIGHLIGHT}\" ] && COLOR_HIGHLIGHT=\"${GRUB_COLOR_HIGHLIGHT}\"\n\
   WALLPAPER=\"${GRUB_MENU_PICTURE}\"\n\
fi\n";

	FILE* outFile = fopen(this->env->settings_file.c_str(), "w");
	if (outFile){
		if (this->oldFontFile != "") {
			remove(this->oldFontFile.c_str());
			this->oldFontFile = "";
		}

		std::string generatedFont;
		if (this->grubFont != "") {
			generatedFont = this->mkFont();
		}
		bool background_script_required = false;
		bool isGraphical = false;
		this->color_helper_required = false;
		for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(false); iter != this->end(); iter++){
			if (iter != this->begin(false)) {
				fputs("\n", outFile);
			}
			fputs((iter->getOutput()).c_str(), outFile);

			if (!background_script_required && !this->env->useDirectBackgroundProps && (iter->name == "GRUB_MENU_PICTURE" || iter->name == "GRUB_COLOR_NORMAL" || iter->name == "GRUB_COLOR_HIGHLIGHT")) {
				background_script_required = true;
				isGraphical = true;
			}
			if (this->env->useDirectBackgroundProps && (iter->name == "GRUB_COLOR_NORMAL" || iter->name == "GRUB_COLOR_HIGHLIGHT")) {
				this->color_helper_required = true;
				isGraphical = true;
			}
			if (iter->name == "GRUB_BACKGROUND" && this->env->useDirectBackgroundProps) {
				isGraphical = true;
			}
		}
		fclose(outFile);
		if (background_script_required){
			mkdir((env->cfg_dir_prefix+"/usr/share/desktop-base").c_str(), 0755);
			FILE* bgScriptFile = fopen((env->cfg_dir_prefix+"/usr/share/desktop-base/grub_background.sh").c_str(), "w");
			chmod((env->cfg_dir_prefix+"/usr/share/desktop-base/grub_background.sh").c_str(), 0755);
			fputs(background_script, bgScriptFile);
			fclose(bgScriptFile);
		}

		if (isGraphical && generatedFont == "") {
			FILE* fontFile = fopen((this->env->output_config_dir + "/unicode.pf2").c_str(), "r");
			if (fontFile) {
				this->log("font file exists", Logger::INFO);
				fclose(fontFile);
			} else {
				this->log("generating the font file", Logger::EVENT);
				this->mkFont("/usr/share/fonts/dejavu/DejaVuSansMono.ttf");
			}
		}
		this->removeItem("GRUB_FONT");

		this->_reloadRequired = false;
		return true;
	}
	else
		return false;
}

bool Model_SettingsManagerData::setValue(std::string const& name, std::string const& value){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){

			if (iter->value != value){ //only set when the new value is really new
				iter->value = value;
				if (name == "GRUB_DISABLE_LINUX_RECOVERY" || name == "GRUB_DISABLE_OS_PROBER")
					_reloadRequired = true;
			}

			return true;
		}
	}

	settings.push_back(Model_SettingsStore_Row());
	settings.back().name = name;
	settings.back().value = value;
	settings.back().validate();
	if (name == "GRUB_DISABLE_LINUX_RECOVERY" || name == "GRUB_DISABLE_OS_PROBER")
		_reloadRequired = true;
	return false;
}

bool Model_SettingsManagerData::setIsActive(std::string const& name, bool value){
	for (std::list<Model_SettingsStore_Row>::iterator iter = this->begin(); iter != this->end(); this->iter_to_next_setting(iter)){
		if (name == iter->name){
			if (iter->isActive != value){
				iter->isActive = value;
				if (name == "GRUB_DISABLE_LINUX_RECOVERY" || name == "GRUB_DISABLE_OS_PROBER")
					_reloadRequired = true;
			}
			return true;
		}
	}
	return false;
}
