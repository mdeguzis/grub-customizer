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

#ifndef THEME_GTK_H_
#define THEME_GTK_H_

#include "../Theme.h"
#include <gtkmm.h>
#include <string>
#include <libintl.h>
#include "../../Controller/ThemeController.h"
#include "../../Controller/Trait/ControllerAware.h"
#include "../../lib/Trait/LoggerAware.h"
#include "../../lib/assert.h"

//a gtkmm combobox with colorful foreground and background. useful to choose an item of a predefined color set
class View_Gtk_Theme_ColorChooser : public Gtk::ComboBox, public View_ColorChooser {
	struct Columns : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> idName;
		Gtk::TreeModelColumn<Glib::ustring> colorCode_background;
		Gtk::TreeModelColumn<Glib::ustring> colorCode_foreground;
		Columns();
	};
	Columns columns;
	Glib::RefPtr<Gtk::ListStore> refListStore;
	public:
	View_Gtk_Theme_ColorChooser();
	void addColor(std::string const& codeName, std::string const& outputName, std::string const& cell_background, std::string const& cell_foreground);
	void selectColor(std::string const& codeName);
	std::string getSelectedColor() const;
	Pango::Color getSelectedColorAsPangoObject() const;
	bool event_lock;
};

//a color chooser with predefined colors for grub
class GrubColorChooser : public View_Gtk_Theme_ColorChooser {
public:
	GrubColorChooser(bool blackIsTransparent = false);
};

class View_Gtk_Theme :
	public View_Theme,
	public Gtk::Dialog,
	public Trait_LoggerAware,
	public Trait_ControllerAware<ThemeController>
{
	Gtk::VBox vbMain;

	Gtk::Frame frmCustomTheme;
	Gtk::HPaned hpCustomTheme;
	Gtk::Frame frmCustomThemeSettings;
	Gtk::VBox vbCustomThemeSettings;
	Gtk::Frame frmCustomThemePreview;
	Gtk::VBox vbCustomThemePreview;

	Gtk::HPaned hpThemeEditor;
	Gtk::Toolbar toolbar;
	Gtk::ToolButton tbttAdd, tbttRemove;

	Gtk::VBox vbFiles;
	Gtk::Frame frmThemeEditor;
	Gtk::ScrolledWindow scrFiles;
	Gtk::ListViewText lvFiles;
	Gtk::VBox vbEdit, vbEditInner;
	Gtk::ScrolledWindow scrEdit;
	Gtk::TextView txtEdit;
	Gtk::Image imgPreview;
	Gtk::HBox hbFileSelection;
	Gtk::Label lblFileSelection;
	Gtk::FileChooserButton fcFileSelection;
	Glib::RefPtr<Gtk::SizeGroup> sizeGroupFooter;

	Gtk::HBox hbTheme;
	Gtk::Label lblTheme;
	Gtk::ComboBoxText cbTheme;
	Gtk::Button bttAddTheme, bttRemoveTheme, bttThemeHelp;
	Gtk::Image imgAddTheme, imgRemoveTheme, imgThemeHelp;

	Gtk::FileChooserDialog fcThemeFileChooser;

	// simple theme editor

	//color chooser
	Gtk::VBox vbColorChoosers;
	GrubColorChooser gccNormalForeground, gccNormalBackground, gccHighlightForeground, gccHighlightBackground;
	Gtk::Label lblNormalForeground, lblNormalBackground, lblHighlightForeground, lblHighlightBackground;
	Gtk::VBox vbNormalForeground, vbNormalBackground, vbHighlightForeground, vbHighlightBackground;
	Gtk::Frame groupNormalForeground, groupNormalBackground, groupHighlightForeground, groupHighlightBackground;

	//font selection
	Gtk::Frame groupFont;
	Gtk::Label lblFont;
	Gtk::Alignment alignFont;
	Gtk::FontButton bttFont;
	Gtk::HBox hbFont;
	Gtk::Button bttRemoveFont;
	Gtk::Image imgRemoveFont;

	//background image
	Gtk::Frame groupBackgroundImage;
	Gtk::Alignment alignBackgroundImage;
	Gtk::Label lblBackgroundImage, lblBackgroundRequiredInfo;
	Gtk::VBox vbBackgroundImage;
	Gtk::HBox hbBackgroundImage;
	Gtk::FileChooserButton fcBackgroundImage;
	Gtk::DrawingArea drwBackgroundPreview;
	Glib::ustring backgroundImagePath;
	std::list<std::string> previewEntryTitles;
	Glib::Mutex previewEntryTitles_mutex;
	Gtk::VBox vbButtons;
	Gtk::Button bttCopyBackground, bttRemoveBackground;
	Gtk::Image imgRemoveBackground;


	bool event_lock;
	std::string _getSelectedFileName();
public:
	View_Gtk_Theme();

	void addFile(std::string const& fileName);
	void clear();
	void setText(std::string const& text);
	void setImage(std::string const& path);
	void selectFile(std::string const& fileName, bool startEdit = false);
	void selectTheme(std::string const& name);
	std::string getSelectedTheme();
	void addTheme(std::string const& name);
	void clearThemeSelection();
	void show(bool burgMode);
	void setRemoveFunctionalityEnabled(bool value);

	void setEditorType(EditorType type);
	void showThemeFileChooser();
	void showError(Error const& e, std::string const& info = "");
	void setCurrentExternalThemeFilePath(std::string const& fileName);
	std::string getDefaultName() const;
	View_ColorChooser& getColorChooser(ColorChooserType type);
	std::string getFontName();
	int getFontSize();
	void setFontName(std::string const& value);
	void showFontWarning();
	Glib::RefPtr<Pango::Layout> createFormattedText(Cairo::RefPtr<Cairo::Context>& context, Glib::ustring const& text, std::string const& format, int r, int g, int b, int r_b, int g_b, int b_b, bool black_bg_is_transparent = true);
	void setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir);
	void redraw(std::string const& menuPicturePath, bool isInGrubDir, Cairo::RefPtr<Cairo::Context> const* cr = NULL);
	std::string getBackgroundImagePath();
	void setPreviewEntryTitles(std::list<std::string> const& entries);

	Gtk::Widget& getThemeSelector();
	Gtk::Widget& getEditorBox();
private:
	void signal_fileAddClick();
	void signal_fileRemoveClick();
	void signal_fileSelected();
	void signal_fileRenamed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
	void signal_fileChosen();
	void signal_textChanged();
	void signal_themeChosen();
	void signal_addThemeClicked();
	void signal_removeThemeClicked();
	void signal_themeFileChooserResponse(int response_id);
	void signal_dialogResponse(int response_id);

	void signal_color_changed(View_Gtk_Theme_ColorChooser& caller);
	void signal_font_changed();
	void signal_font_removed();
	void signal_other_image_chosen();
	void signal_help_click();
	void signal_bttRemoveBackground_clicked();
	bool signal_redraw_preview(const Cairo::RefPtr<Cairo::Context>& cr);
};


#endif /* THEME_GTK_H_ */
