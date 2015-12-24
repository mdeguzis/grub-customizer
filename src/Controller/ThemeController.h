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

#ifndef THEMECONTROLLER_H_
#define THEMECONTROLLER_H_
#include <string>

class ThemeController {
public:
	virtual void loadThemesAction() = 0;
	virtual void loadThemeAction(std::string const& name) = 0;
	virtual void addThemePackageAction(std::string const& filePath) = 0;
	virtual void removeThemeAction(const std::string& name) = 0;
	virtual void showThemeInstallerAction() = 0;
	virtual void showSimpleThemeConfigAction() = 0;
	virtual void addFileAction() = 0;
	virtual void startFileEditAction(std::string const& file) = 0;
	virtual void removeFileAction(std::string const& file) = 0;
	virtual void updateEditAreaAction(std::string const& file) = 0;
	virtual void renameAction(std::string const& newName) = 0;
	virtual void loadFileAction(std::string const& externalPath) = 0;
	virtual void saveTextAction(std::string const& newText) = 0;

	virtual void updateColorSettingsAction() = 0;
	virtual void updateFontSettingsAction(bool removeFont) = 0;
	virtual void updateFontSizeAction() = 0;
	virtual void updateBackgroundImageAction() = 0;
	virtual void removeBackgroundImageAction() = 0;
	virtual void updateSettingsDataAction() = 0;

	virtual void syncAction() = 0;
	virtual void saveAction() = 0;

	virtual void postSaveAction() = 0;

	virtual inline ~ThemeController() {}
};


#endif /* THEMECONTROLLER_H_ */
