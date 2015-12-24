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

#ifndef THEME_H_
#define THEME_H_
#include <string>
#include <list>
#include "ColorChooser.h"

class View_Theme {
public:
	enum EditorType {
		EDITORTYPE_CUSTOM,
		EDITORTYPE_THEME
	};
	enum ColorChooserType {
		COLOR_CHOOSER_DEFAULT_FONT,
		COLOR_CHOOSER_DEFAULT_BACKGROUND,
		COLOR_CHOOSER_HIGHLIGHT_FONT,
		COLOR_CHOOSER_HIGHLIGHT_BACKGROUND
	};

	enum Error {
		ERROR_INVALID_THEME_PACK_FORMAT,
		ERROR_RENAME_CONFLICT,
		ERROR_THEMEFILE_NOT_FOUND,
		ERROR_SAVE_FAILED,
		ERROR_NO_FILE_SELECTED
	};

	virtual void addFile(std::string const& fileName) = 0;
	virtual void clear() = 0;
	virtual void setText(std::string const& text) = 0;
	virtual void setImage(std::string const& path) = 0;
	virtual void selectFile(std::string const& fileName, bool startEdit = false) = 0;
	virtual void selectTheme(std::string const& name) = 0;
	virtual std::string getSelectedTheme() = 0;
	virtual void addTheme(std::string const& name) = 0;
	virtual void clearThemeSelection() = 0;
	virtual void show(bool burgMode) = 0;
	virtual void setRemoveFunctionalityEnabled(bool value) = 0;
	virtual void setEditorType(EditorType type) = 0;
	virtual void showThemeFileChooser() = 0;
	virtual void showError(Error const& e, std::string const& info = "") = 0;
	virtual void setCurrentExternalThemeFilePath(std::string const& fileName) = 0;
	virtual std::string getDefaultName() const = 0;

	//returns an interface to the given color chooser
	virtual View_ColorChooser& getColorChooser(ColorChooserType type)=0;
	//get the name of the selected font
	virtual std::string getFontName()=0;
	//get the font size
	virtual int getFontSize()=0;
	//set the name of the selected font
	virtual void setFontName(std::string const& value)=0;

	//shows a warning message about very large fonts
	virtual void showFontWarning() = 0;

	//sets the background image, remove it, if empty string is given
	virtual void setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir)=0;
	//reads the selected background image path
	virtual std::string getBackgroundImagePath()=0;

	//set the entry titles to be viewed in the preview image
	virtual void setPreviewEntryTitles(std::list<std::string> const& entries)=0;

	virtual inline ~View_Theme() {};
};

#endif /* THEME_H_ */
