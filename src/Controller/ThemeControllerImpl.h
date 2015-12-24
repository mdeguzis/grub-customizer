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

#ifndef THEMECONTROLLERIMPL_H_
#define THEMECONTROLLERIMPL_H_
#include "ThemeController.h"
#include "../Model/Env.h"
#include "../View/Theme.h"
#include "../View/Trait/ViewAware.h"
#include "../Model/SettingsManagerData.h"
#include "ControllerAbstract.h"
#include "Trait/ThreadControllerAware.h"
#include "../Model/ThemeManager.h"
#include "../Model/ListCfg.h"
#include <algorithm>

class ThemeControllerImpl :
	public ThemeController,
	public ControllerAbstract,
	public View_Trait_ViewAware<View_Theme>,
	public Trait_ThreadControllerAware,
	public Model_ThemeManager_Connection,
	public Model_SettingsManagerData_Connection,
	public Model_ListCfg_Connection,
	public Model_Env_Connection
{
	std::string currentTheme, currentThemeFile;
	bool syncActive; // should only be controlled by syncSettings()
	bool isImage(std::string const& fileName);

	void syncSettings();
	void syncFiles();
public:
	ThemeControllerImpl();

	void loadThemesAction();
	void loadThemeAction(std::string const& name);
	void addThemePackageAction(const std::string& filePath);
	void removeThemeAction(const std::string& name);
	void showThemeInstallerAction();
	void showSimpleThemeConfigAction();
	void addFileAction();
	void startFileEditAction(std::string const& file);
	void removeFileAction(std::string const& file);
	void updateEditAreaAction(std::string const& file);
	void renameAction(std::string const& newName);
	void loadFileAction(std::string const& externalPath);
	void saveTextAction(std::string const& newText);

	void updateColorSettingsAction();
	void updateFontSettingsAction(bool removeFont);
	void updateFontSizeAction();
	void updateBackgroundImageAction();
	void removeBackgroundImageAction();
	void updateSettingsDataAction();

	void syncAction();
	void saveAction();
	void postSaveAction();
};


#endif /* THEMECONTROLLERIMPL_H_ */
