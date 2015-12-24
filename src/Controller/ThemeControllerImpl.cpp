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

#include "ThemeControllerImpl.h"

ThemeControllerImpl::ThemeControllerImpl()
	: ControllerAbstract("theme"),
	  syncActive(false)
{
}

void ThemeControllerImpl::syncSettings() {
	if (this->syncActive) {
		return;
	}
	this->syncActive = true;
	std::string nColor = this->settings->getValue("GRUB_COLOR_NORMAL");
	std::string hColor = this->settings->getValue("GRUB_COLOR_HIGHLIGHT");
	if (nColor != ""){
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).selectColor(nColor.substr(0, nColor.find('/')));
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor(nColor.substr(nColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).selectColor("light-gray");
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).selectColor("black");
	}
	if (hColor != ""){
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor(hColor.substr(0, hColor.find('/')));
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor(hColor.substr(hColor.find('/')+1));
	}
	else {
		//default grub menu colors
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).selectColor("magenta");
		this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).selectColor("black");
	}

	std::string wallpaper_key = this->env->useDirectBackgroundProps ? "GRUB_BACKGROUND" : "GRUB_MENU_PICTURE";
	std::string menuPicturePath = this->settings->getValue(wallpaper_key);
	bool menuPicIsInGrubDir = false;
	if (menuPicturePath != "" && menuPicturePath[0] != '/'){
		menuPicturePath = env->output_config_dir + "/" + menuPicturePath;
		menuPicIsInGrubDir = true;
	}
	this->view->setFontName(this->settings->grubFont);

	if (this->settings->isActive(wallpaper_key) && menuPicturePath != ""){
		this->view->setBackgroundImagePreviewPath(menuPicturePath, menuPicIsInGrubDir);
	}
	else {
		this->view->setBackgroundImagePreviewPath("", menuPicIsInGrubDir);
	}

	std::string selectedTheme = this->view->getSelectedTheme();

	this->view->clearThemeSelection();
	for (std::list<Model_Theme>::iterator themeIter = this->themeManager->themes.begin(); themeIter != this->themeManager->themes.end(); themeIter++) {
		this->view->addTheme(themeIter->name);
	}
	this->view->selectTheme(selectedTheme);

	std::string themeName;
	if (this->settings->isActive("GRUB_THEME")) {
		try {
			themeName = this->themeManager->extractThemeName(this->settings->getValue("GRUB_THEME"));
		} catch (InvalidStringFormatException const& e) {
			this->log(e, Logger::ERROR);
		}
	}

	if (this->currentTheme != themeName) {
		if (this->settings->isActive("GRUB_THEME")) {
			if (this->themeManager->themeExists(themeName)) {
				this->loadThemeAction(themeName);
			} else {
				this->log("theme " + themeName + " not found!", Logger::ERROR);
			}
		} else if (!this->settings->isActive("GRUB_THEME")) {
			this->showSimpleThemeConfigAction();
		}
	}

	this->getAllControllers().settingsController->syncAction();
	this->syncActive = false;
}

void ThemeControllerImpl::syncFiles() {
	this->view->clear();
	if (this->currentTheme != "") {
		Model_Theme* theme = &this->themeManager->getTheme(this->currentTheme);
		for (std::list<Model_ThemeFile>::iterator themeFileIter = theme->files.begin(); themeFileIter != theme->files.end(); themeFileIter++) {
			this->view->addFile(themeFileIter->newLocalFileName);
		}
		if (this->currentThemeFile != "") {
			this->view->selectFile(theme->getFile(this->currentThemeFile).newLocalFileName);
		}
	}
}

bool ThemeControllerImpl::isImage(std::string const& fileName) {
	std::list<std::string> imageExtensions;
	imageExtensions.push_back("png");
	imageExtensions.push_back("jpg");
	imageExtensions.push_back("bmp");
	imageExtensions.push_back("gif");
	imageExtensions.push_back("pf2"); // not really, but shouldn't be handled as text

	if (fileName.find_last_of(".") != std::string::npos) {
		int dotPos = fileName.find_last_of(".");
		std::string extension = fileName.substr(dotPos + 1);
		if (std::find(imageExtensions.begin(), imageExtensions.end(), extension) != imageExtensions.end()) {
			return true;
		}
	}
	return false;
}

void ThemeControllerImpl::loadThemesAction() {
	this->logActionBegin("load-themes");
	try {
		try {
			this->themeManager->load();
		} catch (FileReadException const& e) {
			this->log("Theme directory not found", Logger::INFO);
		}
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::loadThemeAction(std::string const& name) {
	this->logActionBegin("load-theme");
	try {
		this->view->setEditorType(View_Theme::EDITORTYPE_THEME);
		this->view->setRemoveFunctionalityEnabled(true);
		this->view->selectTheme(name);
		this->currentTheme = name;
		try {
			this->themeManager->getTheme(name).getFileByNewName("theme.txt");
		} catch (ItemNotFoundException const& e) {
			this->view->showError(View_Theme::ERROR_THEMEFILE_NOT_FOUND);
		}
		this->settings->setValue("GRUB_THEME", this->themeManager->getThemePath() + "/" + name + "/theme.txt");
		this->settings->setIsActive("GRUB_THEME", true);

		this->syncFiles();
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::addThemePackageAction(const std::string& filePath) {
	this->logActionBegin("add-theme-package");
	try {
		try {
			std::string themeName = this->themeManager->addThemePackage(filePath);
			this->themeManager->getTheme(themeName).isModified = true;
			this->loadThemeAction(themeName);
			this->syncSettings();
			this->view->selectTheme(themeName);
		} catch (InvalidFileTypeException const& e) {
			this->view->showError(View_Theme::ERROR_INVALID_THEME_PACK_FORMAT);
		}
	} catch (const Exception& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::removeThemeAction(const std::string& name) {
	this->logActionBegin("remove-theme");
	try {
		this->themeManager->removeTheme(this->themeManager->getTheme(name));
		this->showSimpleThemeConfigAction();
		this->syncSettings();
	} catch (const Exception& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::showThemeInstallerAction() {
	this->logActionBegin("show-theme-installer");
	try {
		this->view->showThemeFileChooser();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::showSimpleThemeConfigAction() {
	this->logActionBegin("show-simple-theme-config");
	try {
		this->view->setEditorType(View_Theme::EDITORTYPE_CUSTOM);
		this->view->setRemoveFunctionalityEnabled(false);
		this->view->selectTheme("");
		this->updateColorSettingsAction();
		this->settings->setIsActive("GRUB_THEME", false);
		this->currentTheme = "";
		this->currentThemeFile = "";
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::addFileAction() {
	this->logActionBegin("add-file");
	try {
		std::string defaultName = this->view->getDefaultName();
		Model_Theme* theme = &this->themeManager->getTheme(this->currentTheme);
		if (!theme->hasConflicts(defaultName)) {
			Model_ThemeFile newFile(defaultName, true);
			newFile.content = "";
			newFile.contentLoaded = true;
			theme->files.push_back(newFile);
			theme->isModified = true;
			theme->sort();
			this->syncFiles();
			this->threadController->startThemeFileEdit(defaultName);
		} else {
			this->view->showError(View_Theme::ERROR_RENAME_CONFLICT);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::startFileEditAction(std::string const& file) {
	this->logActionBegin("select-file");
	try {
		this->view->selectFile(file, true);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::removeFileAction(std::string const& file) {
	this->logActionBegin("remove-file");
	try {
		Model_ThemeFile* fileObj = &this->themeManager->getTheme(this->currentTheme).getFileByNewName(file);
		this->themeManager->getTheme(this->currentTheme).removeFile(*fileObj);
		this->themeManager->getTheme(this->currentTheme).isModified = true;
		this->syncFiles();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateEditAreaAction(std::string const& file) {
	this->logActionBegin("update-edit-area");
	try {
		Model_Theme* theme = &this->themeManager->getTheme(this->currentTheme);
		Model_ThemeFile* themeFile = &theme->getFileByNewName(file);
		std::string originalFileName = themeFile->localFileName;
		bool isImage = this->isImage(file);
		this->currentThemeFile = originalFileName;
		if (themeFile->content != "") {
			this->view->setText(themeFile->content);
		} else if (themeFile->externalSource != "") {
			if (isImage) {
				this->view->setImage(themeFile->externalSource);
			} else {
				std::string content = theme->loadFileContentExternal(themeFile->externalSource);
				this->view->setText(content);
			}
		} else if (!themeFile->isAddedByUser) {
			if (isImage) {
				this->view->setImage(theme->getFullFileName(originalFileName));
			} else {
				std::string content = theme->loadFileContent(originalFileName);
				this->view->setText(content);
			}
		} else {
			if (!isImage) {
				this->view->setText("");
			}
		}
		this->view->setCurrentExternalThemeFilePath(themeFile->externalSource);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::renameAction(std::string const& newName) {
	this->logActionBegin("rename");
	try {
		Model_ThemeFile* themeFile = &this->themeManager->getTheme(this->currentTheme).getFile(this->currentThemeFile);
		if (themeFile->newLocalFileName == newName) {
			// do nothing
		} else if (!this->themeManager->getTheme(this->currentTheme).hasConflicts(newName)) {
			this->themeManager->getTheme(this->currentTheme).isModified = true;
			themeFile->newLocalFileName = newName;
			if (themeFile->isAddedByUser) {
				themeFile->localFileName = newName;
				this->currentThemeFile = newName;
			}
			this->updateEditAreaAction(newName);
		} else {
			this->view->showError(View_Theme::ERROR_RENAME_CONFLICT);
		}

		this->themeManager->getTheme(this->currentTheme).sort();
		this->syncFiles();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::loadFileAction(std::string const& externalPath) {
	this->logActionBegin("load-file");
	try {
		if (this->currentThemeFile == "") {
			this->view->showError(View_Theme::ERROR_NO_FILE_SELECTED);
			return;
		}
		this->themeManager->getTheme(this->currentTheme).isModified = true;
		Model_ThemeFile* file = &this->themeManager->getTheme(this->currentTheme).getFile(this->currentThemeFile);
		file->externalSource = externalPath;
		file->content = "";
		file->contentLoaded = false;
		this->updateEditAreaAction(file->newLocalFileName);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::saveTextAction(std::string const& newText) {
	this->logActionBegin("save-text");
	try {
		this->themeManager->getTheme(this->currentTheme).isModified = true;
		Model_ThemeFile* themeFile = &this->themeManager->getTheme(this->currentTheme).getFile(this->currentThemeFile);
		themeFile->externalSource = "";
		this->view->setCurrentExternalThemeFilePath(themeFile->externalSource);
		themeFile->content = newText;
		themeFile->contentLoaded = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateBackgroundImageAction(){
	this->logActionBegin("update-background-image");
	try {
		if (!this->env->useDirectBackgroundProps) {
			this->settings->setValue("GRUB_MENU_PICTURE", this->view->getBackgroundImagePath());
			this->settings->setIsActive("GRUB_MENU_PICTURE", true);
			this->settings->setIsExport("GRUB_MENU_PICTURE", true);
		} else {
			this->settings->setValue("GRUB_BACKGROUND", this->view->getBackgroundImagePath());
			this->settings->setIsActive("GRUB_BACKGROUND", true);
		}
		this->syncSettings();
		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateColorSettingsAction(){
	this->logActionBegin("update-color-settings");
	try {
		if (this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() != "" && this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor() != ""){
			this->settings->setValue("GRUB_COLOR_NORMAL", this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_FONT).getSelectedColor() + "/" + this->view->getColorChooser(View_Theme::COLOR_CHOOSER_DEFAULT_BACKGROUND).getSelectedColor());
			this->settings->setIsActive("GRUB_COLOR_NORMAL", true);
			this->settings->setIsExport("GRUB_COLOR_NORMAL", true);
		}
		if (this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() != "" && this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor() != ""){
			this->settings->setValue("GRUB_COLOR_HIGHLIGHT", this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_FONT).getSelectedColor() + "/" + this->view->getColorChooser(View_Theme::COLOR_CHOOSER_HIGHLIGHT_BACKGROUND).getSelectedColor());
			this->settings->setIsActive("GRUB_COLOR_HIGHLIGHT", true);
			this->settings->setIsExport("GRUB_COLOR_HIGHLIGHT", true);
		}
		this->syncSettings();
		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateFontSettingsAction(bool removeFont) {
	this->logActionBegin("update-font-settings");
	try {
		std::string fontName;
		int fontSize = -1;

		if (!removeFont) {
			fontName = this->view->getFontName();
			fontSize = this->view->getFontSize();
		}
		if (fontName != "" && this->settings->grubFont == "") {
			this->view->showFontWarning();
		}

		this->settings->grubFont = fontName;
		this->settings->grubFontSize = fontSize;

		if (fontName != "") {
			std::string fullTmpFontPath = this->env->cfg_dir_prefix + "/tmp/grub_customizer_chosen_font_test.pf2";

			this->settings->mkFont("", "/tmp/grub_customizer_chosen_font_test.pf2");
			this->settings->grubFont = this->settings->parsePf2(fullTmpFontPath)["NAME"];

			// must be done again to check the font returned by parsePf2
			this->settings->mkFont("", "/tmp/grub_customizer_chosen_font_test.pf2");
			this->settings->grubFont = this->settings->parsePf2(fullTmpFontPath)["NAME"];

			// to prevent usage of the temporary font file
			this->settings->removeItem("GRUB_FONT");
		}

		this->syncSettings();
		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

/**
 * sync font size from view back to model. should be called on startup
 */
void ThemeControllerImpl::updateFontSizeAction() {
	this->logActionBegin("update-font-size");
	try {
		this->settings->grubFontSize = this->view->getFontSize();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::removeBackgroundImageAction(){
	this->logActionBegin("remove-background-image");
	try {
		if (!this->env->useDirectBackgroundProps) {
			this->settings->setIsActive("GRUB_MENU_PICTURE", false);
		} else {
			this->settings->setIsActive("GRUB_BACKGROUND", false);
		}
		this->syncSettings();
		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::updateSettingsDataAction() {
	this->logActionBegin("update-settings-data");
	try {
		std::list<std::string> labelListToplevel  = this->grublistCfg->proxies.getToplevelEntryTitles();

		this->view->setPreviewEntryTitles(labelListToplevel);

		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::syncAction() {
	this->logActionBegin("sync");
	try {
		this->syncSettings();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

/**
 * Threaded
 */
void ThemeControllerImpl::saveAction() {
	this->logActionBegin("save");
	try {
		this->themeManager->save();
		this->threadController->doPostSaveActions();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void ThemeControllerImpl::postSaveAction() {
	this->logActionBegin("post-save");
	try {
		if (this->themeManager->hasSaveErrors()) {
			this->view->showError(View_Theme::ERROR_SAVE_FAILED, this->themeManager->getSaveErrors());
		}
		this->syncSettings();
		this->syncFiles();
		this->currentThemeFile = "";
		this->view->selectFile("");
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


