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

#include "Theme.h"

View_Gtk_Theme::View_Gtk_Theme()
	: lvFiles(1, true), lblFileSelection(gettext("_Load file: "), true),
	  tbttAdd(Gtk::Stock::ADD), tbttRemove(Gtk::Stock::REMOVE), event_lock(false),
	  lblTheme(gettext("_Theme:"), true), lblBackgroundImage(gettext("background image")),
	  imgRemoveBackground(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON), imgRemoveFont(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON),
	  imgThemeHelp(Gtk::Stock::HELP, Gtk::ICON_SIZE_BUTTON),
	  lblBackgroundRequiredInfo(gettext("Please choose a background image!")),
	  gccNormalBackground(true), gccHighlightBackground(true), lblFont(gettext("_Font"), true),
	  imgAddTheme(Gtk::Stock::ADD, Gtk::ICON_SIZE_BUTTON), imgRemoveTheme(Gtk::Stock::DELETE, Gtk::ICON_SIZE_BUTTON),
	  fcThemeFileChooser(*this, gettext("choose theme file"), Gtk::FILE_CHOOSER_ACTION_OPEN),
	  frmThemeEditor(gettext("Theme contents")), frmCustomTheme(gettext("Custom Theme settings")),
	  lblNormalForeground(gettext("Normal: Font"), Pango::ALIGN_LEFT, Pango::ALIGN_CENTER, false),
	  lblNormalBackground(gettext("Normal: Background"), Pango::ALIGN_LEFT, Pango::ALIGN_CENTER, false),
	  lblHighlightForeground(gettext("Highlighted: Font"), Pango::ALIGN_LEFT, Pango::ALIGN_CENTER, false),
	  lblHighlightBackground(gettext("Highlighted: Background"), Pango::ALIGN_LEFT, Pango::ALIGN_CENTER, false)
{
	Gtk::Box& dlgVBox = *this->get_vbox();

	dlgVBox.pack_start(hbTheme, Gtk::PACK_SHRINK);
	hbTheme.pack_start(lblTheme, Gtk::PACK_SHRINK);
	hbTheme.pack_start(cbTheme);
	hbTheme.pack_start(bttAddTheme, Gtk::PACK_SHRINK);
	hbTheme.pack_start(bttRemoveTheme, Gtk::PACK_SHRINK);
	hbTheme.pack_start(bttThemeHelp, Gtk::PACK_SHRINK);
	bttAddTheme.add(imgAddTheme);
	bttRemoveTheme.add(imgRemoveTheme);
	bttThemeHelp.add(imgThemeHelp);

	dlgVBox.pack_start(vbMain);

	vbMain.pack_start(frmThemeEditor);
	vbMain.pack_start(frmCustomTheme);

	frmThemeEditor.add(hpThemeEditor);
	frmCustomTheme.add(hpCustomTheme);

	toolbar.add(tbttAdd);
	toolbar.add(tbttRemove);

	hpThemeEditor.pack1(vbFiles, Gtk::FILL);
	hpThemeEditor.pack2(vbEdit, Gtk::FILL, Gtk::EXPAND);

	vbFiles.pack_start(scrFiles);
	vbFiles.pack_start(toolbar, Gtk::PACK_SHRINK);

	vbEdit.pack_start(scrEdit);
	vbEdit.pack_start(hbFileSelection, Gtk::PACK_SHRINK);

	scrFiles.add(lvFiles);
	scrEdit.add(vbEditInner);
	vbEditInner.pack_start(txtEdit);
	vbEditInner.pack_start(imgPreview);

	hbFileSelection.pack_start(lblFileSelection, Gtk::PACK_SHRINK);
	hbFileSelection.pack_start(fcFileSelection);

	sizeGroupFooter = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_VERTICAL);
	sizeGroupFooter->add_widget(toolbar);
	sizeGroupFooter->add_widget(hbFileSelection);

	hpCustomTheme.pack1(frmCustomThemeSettings, Gtk::FILL);
	hpCustomTheme.pack2(frmCustomThemePreview, Gtk::FILL, Gtk::EXPAND);
	hpCustomTheme.set_position(200);

	frmCustomThemeSettings.add(vbCustomThemeSettings);
	frmCustomThemePreview.add(vbCustomThemePreview);

	vbCustomThemeSettings.set_border_width(5);
	vbCustomThemePreview.set_border_width(5);

	vbCustomThemeSettings.set_spacing(10);

	hpCustomTheme.set_border_width(5);

	frmCustomThemeSettings.set_shadow_type(Gtk::SHADOW_IN);
	frmCustomThemePreview.set_shadow_type(Gtk::SHADOW_IN);

	lblTheme.set_mnemonic_widget(cbTheme);

	tbttAdd.set_is_important(true);

	txtEdit.set_no_show_all(true);

	lblFileSelection.set_mnemonic_widget(fcFileSelection);

	lvFiles.set_column_title(0, gettext("File"));

	scrEdit.set_shadow_type(Gtk::SHADOW_IN);
	scrEdit.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	hbFileSelection.set_border_width(5);
	hbFileSelection.set_spacing(5);

	scrFiles.set_min_content_width(200);
	scrFiles.set_shadow_type(Gtk::SHADOW_IN);
	scrFiles.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

	bttAddTheme.set_tooltip_text(gettext("add theme"));
	bttRemoveTheme.set_tooltip_text(gettext("delete this theme"));

	frmThemeEditor.set_no_show_all(true);
	hpThemeEditor.set_border_width(5);

	Glib::RefPtr<Gtk::FileFilter> fileFilter = Gtk::FileFilter::create();
	fileFilter->add_mime_type("application/x-gzip");
	fileFilter->add_mime_type("application/x-tar");
	fileFilter->add_mime_type("application/x-bzip2");
	fileFilter->add_pattern("*.tar.gz");
	fileFilter->add_pattern("*.tar");
	fileFilter->add_pattern("*.tar.bz2");
	fileFilter->set_name(gettext("Archive files"));
	fcThemeFileChooser.add_filter(fileFilter);

	Glib::RefPtr<Gtk::FileFilter> fileFilterAll = Gtk::FileFilter::create();
	fileFilterAll->add_pattern("*");
	fileFilterAll->set_name(gettext("All files"));
	fcThemeFileChooser.add_filter(fileFilterAll);

	//theme selection
	hbTheme.set_spacing(5);
	fcThemeFileChooser.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	fcThemeFileChooser.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_APPLY);

	//color chooser
	vbCustomThemeSettings.pack_start(vbColorChoosers, Gtk::PACK_SHRINK);

	vbColorChoosers.set_spacing(10);

	vbColorChoosers.pack_start(groupNormalForeground, Gtk::PACK_SHRINK);
	vbColorChoosers.pack_start(groupNormalBackground, Gtk::PACK_SHRINK);
	vbColorChoosers.pack_start(groupHighlightForeground, Gtk::PACK_SHRINK);
	vbColorChoosers.pack_start(groupHighlightBackground, Gtk::PACK_SHRINK);

	groupNormalForeground.add(gccNormalForeground);
	groupNormalBackground.add(gccNormalBackground);
	groupHighlightForeground.add(gccHighlightForeground);
	groupHighlightBackground.add(gccHighlightBackground);

	groupNormalForeground.set_label_widget(lblNormalForeground);
	groupNormalBackground.set_label_widget(lblNormalBackground);
	groupHighlightForeground.set_label_widget(lblHighlightForeground);
	groupHighlightBackground.set_label_widget(lblHighlightBackground);

	groupNormalForeground.set_shadow_type(Gtk::SHADOW_NONE);
	groupNormalBackground.set_shadow_type(Gtk::SHADOW_NONE);
	groupHighlightForeground.set_shadow_type(Gtk::SHADOW_NONE);
	groupHighlightBackground.set_shadow_type(Gtk::SHADOW_NONE);


	//font selection
	vbCustomThemeSettings.pack_start(groupFont, Gtk::PACK_SHRINK);
	groupFont.add(alignFont);
	groupFont.set_label_widget(lblFont);
	groupFont.set_shadow_type(Gtk::SHADOW_NONE);
	lblFont.set_mnemonic_widget(bttFont);
	alignFont.add(hbFont);
	hbFont.pack_start(bttFont);
	hbFont.pack_start(bttRemoveFont, Gtk::PACK_SHRINK);
	bttRemoveFont.add(imgRemoveFont);
	bttRemoveFont.set_tooltip_text(gettext("remove font"));
	bttRemoveFont.set_no_show_all(true);

	//background selection
	vbCustomThemeSettings.pack_start(groupBackgroundImage, Gtk::PACK_SHRINK);
	groupBackgroundImage.set_shadow_type(Gtk::SHADOW_NONE);
	groupBackgroundImage.add(alignBackgroundImage);
	groupBackgroundImage.set_label_widget(lblBackgroundImage);
	alignBackgroundImage.add(vbBackgroundImage);
	vbBackgroundImage.pack_start(hbBackgroundImage, Gtk::PACK_SHRINK);
	hbBackgroundImage.pack_start(fcBackgroundImage);
	hbBackgroundImage.pack_start(bttRemoveBackground, Gtk::PACK_SHRINK);
	fcBackgroundImage.set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);

	vbCustomThemePreview.pack_start(lblBackgroundRequiredInfo);
	vbCustomThemePreview.pack_start(drwBackgroundPreview);

	vbButtons.set_spacing(5);

	bttRemoveBackground.set_tooltip_text(gettext("remove background"));
	bttRemoveBackground.add(imgRemoveBackground);
	bttRemoveBackground.set_no_show_all(true);
	lblBackgroundRequiredInfo.set_no_show_all(true);

	bttRemoveTheme.set_sensitive(false);



	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	this->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_APPLY);

	tbttAdd.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileAddClick));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileRemoveClick));
	lvFiles.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileSelected));
	lvFiles.get_model()->signal_row_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileRenamed));
	fcFileSelection.signal_file_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_fileChosen));
	txtEdit.get_buffer()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_textChanged));
	cbTheme.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_themeChosen));
	bttAddTheme.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_addThemeClicked));
	bttRemoveTheme.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_removeThemeClicked));
	bttThemeHelp.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_help_click));
	fcThemeFileChooser.signal_response().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_themeFileChooserResponse));
	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_dialogResponse));

	gccNormalForeground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccNormalForeground));
	gccNormalBackground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccNormalBackground));
	gccHighlightForeground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccHighlightForeground));
	gccHighlightBackground.signal_changed().connect(sigc::bind<View_Gtk_Theme_ColorChooser&>(sigc::mem_fun(this, &View_Gtk_Theme::signal_color_changed), gccHighlightBackground));
	bttFont.signal_font_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_font_changed));
	bttRemoveFont.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_font_removed));
	fcBackgroundImage.signal_file_set().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_other_image_chosen));
	bttRemoveBackground.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_bttRemoveBackground_clicked));

	drwBackgroundPreview.signal_draw().connect(sigc::mem_fun(this, &View_Gtk_Theme::signal_redraw_preview));
}

View_Gtk_Theme_ColorChooser::Columns::Columns(){
	this->add(this->idName);
	this->add(this->name);
	this->add(this->colorCode_background);
	this->add(this->colorCode_foreground);
}

View_Gtk_Theme_ColorChooser::View_Gtk_Theme_ColorChooser()
	: event_lock(false)
{
	refListStore = Gtk::ListStore::create(columns);
	this->set_model(refListStore);

	this->pack_start(columns.name);

	Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = this->get_cells();
	Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();

	this->add_attribute(*cellRenderer, "background", columns.colorCode_background);
	this->add_attribute(*cellRenderer, "foreground", columns.colorCode_foreground);
}
void View_Gtk_Theme_ColorChooser::addColor(std::string const& codeName, std::string const& outputName, std::string const& cell_background, std::string const& cell_foreground){
	this->event_lock = true;
	Gtk::TreeModel::iterator iter = refListStore->append();
	(*iter)[columns.idName] = codeName;
	(*iter)[columns.name] = outputName;
	(*iter)[columns.colorCode_background] = cell_background;
	(*iter)[columns.colorCode_foreground] = cell_foreground;
	this->event_lock = false;
}
void View_Gtk_Theme_ColorChooser::selectColor(std::string const& codeName){
	this->event_lock = true;
	this->set_active(0);
	for (Gtk::TreeModel::iterator iter = this->get_active(); iter; iter++){
		if ((*iter)[columns.idName] == codeName){
			this->set_active(iter);
			break;
		}
	}
	this->event_lock = false;
}
std::string View_Gtk_Theme_ColorChooser::getSelectedColor() const {
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter)
		return (Glib::ustring)(*iter)[columns.idName];
	else
		return "";
}

Pango::Color View_Gtk_Theme_ColorChooser::getSelectedColorAsPangoObject() const {
	Pango::Color color;
	Gtk::TreeModel::iterator iter = this->get_active();
	if (iter) {
		color.parse((Glib::ustring)(*iter)[columns.colorCode_background]);
	}
	return color;
}

GrubColorChooser::GrubColorChooser(bool blackIsTransparent) : View_Gtk_Theme_ColorChooser() {
	this->addColor("white",          gettext("white"),         "#ffffff", "#000000");
	this->addColor("yellow",         gettext("yellow"),        "#fefe54", "#000000");
	this->addColor("light-cyan",     gettext("light-cyan"),    "#54fefe", "#000000");
	this->addColor("cyan",           gettext("cyan"),          "#00a8a8", "#000000");
	this->addColor("light-blue",     gettext("light-blue"),    "#5454fe", "#000000");
	this->addColor("blue",           gettext("blue"),          "#0000a8", "#000000");
	this->addColor("light-green",    gettext("light-green"),   "#54fe54", "#000000");
	this->addColor("green",          gettext("green"),         "#00a800", "#000000");
	this->addColor("light-magenta",  gettext("light-magenta"), "#eb4eeb", "#000000");
	this->addColor("magenta",        gettext("magenta"),       "#a800a8", "#000000");
	this->addColor("light-red",      gettext("light-red"),     "#fe5454", "#000000");
	this->addColor("red",            gettext("red"),           "#ff0000", "#000000");
	this->addColor("brown",          gettext("brown"),         "#a85400", "#000000");
	this->addColor("light-gray",     gettext("light-gray"),    "#a8a8a8", "#000000");
	this->addColor("dark-gray",      gettext("dark-gray"),     "#545454", "#000000");
	this->addColor("black", blackIsTransparent ? gettext("transparent") : gettext("black"), "#000000", "#ffffff");
}


std::string View_Gtk_Theme::_getSelectedFileName() {
	std::vector<int> selectedFiles = this->lvFiles.get_selected();
	std::string result;
	if (selectedFiles.size() == 1) {
		result = this->lvFiles.get_text(selectedFiles[0]);
	} else {
		throw ItemNotFoundException("theme editor: invalid selection count", __FILE__, __LINE__);
	}
	return result;
}

void View_Gtk_Theme::addFile(std::string const& fileName) {
	event_lock = true;
	lvFiles.append(fileName);
	event_lock = false;
}

void View_Gtk_Theme::clear() {
	event_lock = true;
	lvFiles.clear_items();
	event_lock = false;
}

void View_Gtk_Theme::setText(std::string const& text) {
	event_lock = true;
	imgPreview.hide();
	txtEdit.show();
	txtEdit.get_buffer()->set_text(text);
	event_lock = false;
}

void View_Gtk_Theme::setImage(std::string const& path) {
	event_lock = true;
	txtEdit.hide();
	imgPreview.show();
	imgPreview.set(path);
	event_lock = false;
}

void View_Gtk_Theme::selectFile(std::string const& fileName, bool startEdit) {
	lvFiles.get_selection()->unselect_all();
	int pos = 0;
	for (Gtk::TreeModel::iterator iter = lvFiles.get_model()->get_iter("0"); iter; iter++) {
		if (lvFiles.get_text(pos) == fileName) {
			lvFiles.set_cursor(lvFiles.get_model()->get_path(iter), *lvFiles.get_column(0), startEdit);
			break;
		}
		pos++;
	}
}

void View_Gtk_Theme::selectTheme(std::string const& name) {
	this->event_lock = true;
	cbTheme.set_active_text(name);
	if (cbTheme.get_active_row_number() == -1 && cbTheme.get_children().size()) {
		cbTheme.set_active(0);
	}
	this->event_lock = false;
}

std::string View_Gtk_Theme::getSelectedTheme() {
	return cbTheme.get_active_text();
}

void View_Gtk_Theme::addTheme(std::string const& name) {
	this->cbTheme.append(name);
}

void View_Gtk_Theme::clearThemeSelection() {
	event_lock = true;
	this->cbTheme.remove_all();

	cbTheme.append(gettext("(Custom Settings)"));
	cbTheme.set_active(0);
	event_lock = false;
}

void View_Gtk_Theme::show(bool burgMode) {
	this->show_all();

	if (burgMode){
		vbColorChoosers.hide();
		groupBackgroundImage.hide();
	}
	else {
		vbColorChoosers.show();
		groupBackgroundImage.show();
	}
}

void View_Gtk_Theme::setRemoveFunctionalityEnabled(bool value) {
	this->bttRemoveTheme.set_sensitive(value);
}

void View_Gtk_Theme::setEditorType(EditorType type) {
	this->frmCustomTheme.hide();
	this->frmThemeEditor.hide();

	switch (type) {
	case EDITORTYPE_CUSTOM:
		this->frmCustomTheme.show();
		this->frmCustomTheme.show_all_children(true);
		break;
	case EDITORTYPE_THEME:
		this->frmThemeEditor.show();
		this->frmThemeEditor.show_all_children(true);
		break;
	default:
		throw LogicException("unsupported type given", __FILE__, __LINE__);
	}
}

void View_Gtk_Theme::showThemeFileChooser() {
	fcThemeFileChooser.show_all();
}

void View_Gtk_Theme::showError(Error const& e, std::string const& info) {
	switch (e) {
	case ERROR_INVALID_THEME_PACK_FORMAT:
		Gtk::MessageDialog(gettext("The chosen file cannot be loaded as theme"), false, Gtk::MESSAGE_ERROR).run();
		break;
	case ERROR_RENAME_CONFLICT:
		Gtk::MessageDialog(gettext("The given filename cannot be used"), false, Gtk::MESSAGE_ERROR).run();
		break;
	case ERROR_THEMEFILE_NOT_FOUND:
		Gtk::MessageDialog(gettext("This theme doesn't contain a theme.txt. Please look for the config file and rename it to \"theme.txt\"!"), false, Gtk::MESSAGE_WARNING).run();
		break;
	case ERROR_SAVE_FAILED:
		Gtk::MessageDialog(Glib::ustring(gettext("Saving of themes didn't succeed completely!")) + "\n" + info, false, Gtk::MESSAGE_WARNING).run();
		break;
	case ERROR_NO_FILE_SELECTED:
		Gtk::MessageDialog(gettext("File replacement failed. Please select a theme file first!"), false, Gtk::MESSAGE_ERROR).run();
		break;
	default:
		throw NotImplementedException("the current value of View_Theme::Error is not processed", __FILE__, __LINE__);
	}
}

void View_Gtk_Theme::setCurrentExternalThemeFilePath(std::string const& fileName) {
	this->event_lock = true;
	if (fileName == "") {
		this->fcFileSelection.unselect_all();
	} else {
		this->fcFileSelection.set_filename(fileName);
	}

	this->event_lock = false;
}

std::string View_Gtk_Theme::getDefaultName() const {
	return "[" + std::string(gettext("filename")) + "]";
}

View_ColorChooser& View_Gtk_Theme::getColorChooser(ColorChooserType type){
	View_ColorChooser* result = NULL;
	switch (type){
		case COLOR_CHOOSER_DEFAULT_BACKGROUND: result = &this->gccNormalBackground; break;
		case COLOR_CHOOSER_DEFAULT_FONT: result = &this->gccNormalForeground; break;
		case COLOR_CHOOSER_HIGHLIGHT_BACKGROUND: result = &this->gccHighlightBackground; break;
		case COLOR_CHOOSER_HIGHLIGHT_FONT: result = &this->gccHighlightForeground; break;
	}

	assert(result != NULL);
	return *result;
}

std::string View_Gtk_Theme::getFontName() {
	return bttFont.get_font_name();
}

int View_Gtk_Theme::getFontSize() {
	Pango::FontDescription desc(bttFont.get_font_name());
	return desc.get_size() / 1024;
}

void View_Gtk_Theme::setFontName(std::string const& value) {
	bttFont.set_font_name(value);
	bttRemoveFont.set_visible(value != "");
	imgRemoveFont.set_visible(value != "");
}

void View_Gtk_Theme::showFontWarning() {
	Gtk::MessageDialog dialog(gettext("please note: large fonts on low boot screen resolutions can corrupt the boot screen\n\
Because this problem depends on chosen font, it isn't possible to define a general maximum. \
When you're trying to select a larger font, please do it in small steps (rebooting and checking the results).\n\
The problem occurs when the grub gui elements (borders, description) wont fit into screen. \
So while there's enough vertical space you can try a larger font. \
The mass of menuentries is not a problem - they are scrolled if required.\n\n\
The corruption results in a endless loading time of grub2. You need a live cd to fix this:\n\
 * delete the font from (MOUNT)/boot/grub/unicode.pf2\n\
 * reboot your system (menu should show up with default fonts)\n\
 * open /etc/default/grub with root permissions and place a '#' in front of the line containing GRUB_FONT\n\
 * then run grub customizer to choose the font used before"
	), false, Gtk::MESSAGE_INFO);
	dialog.set_title(gettext("Grub fonts can be harmful (Info)"));
	dialog.run();
}

Glib::RefPtr<Pango::Layout> View_Gtk_Theme::createFormattedText(Cairo::RefPtr<Cairo::Context>& context, Glib::ustring const& text, std::string const& format, int r, int g, int b, int r_b, int g_b, int b_b, bool black_bg_is_transparent) {
	Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(context);
	layout->set_text(text);
	Pango::AttrList attrList;
	if (!black_bg_is_transparent || r_b != 0 || g_b != 0 || b_b != 0) {
		Pango::AttrColor bColor = Pango::Attribute::create_attr_background(r_b*255, g_b*255, b_b*255);
		attrList.insert(bColor);
	}
	Pango::AttrColor fColor = Pango::Attribute::create_attr_foreground(r*255, g*255, b*255);
	attrList.insert(fColor);
	Pango::AttrString font = Pango::Attribute::create_attr_family("monospace");
	if (format == "" || format == "Normal") {
		attrList.insert(font);
	} else {
		layout->set_font_description(Pango::FontDescription(format));
	}
	layout->set_attributes(attrList);
	return layout;
}

void View_Gtk_Theme::setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir){
	this->redraw(menuPicturePath, isInGrubDir);

	this->event_lock = true;
	if (menuPicturePath != "") {
		fcBackgroundImage.set_filename(menuPicturePath);
	} else {
		fcBackgroundImage.unselect_all();
	}
	this->event_lock = false;
}

void View_Gtk_Theme::redraw(std::string const& menuPicturePath, bool isInGrubDir, Cairo::RefPtr<Cairo::Context> const* cr){
	this->event_lock = true;
	this->backgroundImagePath = menuPicturePath;

	if (menuPicturePath != "" && !drwBackgroundPreview.get_visible()) {
		drwBackgroundPreview.show();
	}

	if (menuPicturePath != "" && drwBackgroundPreview.get_window()){ //it's important to check whether there's a gdk window, if not, Gdk::Pixbuf::create_from_file produces a crash!
		try {
			Glib::RefPtr<Gdk::Pixbuf> buf = Gdk::Pixbuf::create_from_file(menuPicturePath, drwBackgroundPreview.get_width(), -1, true);
			if (buf) {
				Cairo::RefPtr<Cairo::Context> context = cr ? *cr : drwBackgroundPreview.get_window()->create_cairo_context();

				drwBackgroundPreview.show();
				Gdk::Cairo::set_source_pixbuf(context, buf);
				context->rectangle(0, 0, buf->get_width(), buf->get_height());
				context->fill();

				std::list<Glib::RefPtr<Pango::Layout> > exampleTexts;
				Pango::Color fg_n = this->gccNormalForeground.getSelectedColorAsPangoObject();
				Pango::Color bg_n = this->gccNormalBackground.getSelectedColorAsPangoObject();
				Pango::Color fg_s = this->gccHighlightForeground.getSelectedColorAsPangoObject();
				Pango::Color bg_s = this->gccHighlightBackground.getSelectedColorAsPangoObject();
				std::string fontName = bttFont.get_font_name();
				this->previewEntryTitles_mutex.lock();
				for (std::list<std::string>::iterator iter = this->previewEntryTitles.begin(); iter != this->previewEntryTitles.end(); iter++) {
					if (iter == this->previewEntryTitles.begin()) {
						exampleTexts.push_back(View_Gtk_Theme::createFormattedText(context, *iter, fontName, fg_s.get_red() / 255, fg_s.get_green() / 255, fg_s.get_blue() / 255, bg_s.get_red() / 255, bg_s.get_green() / 255, bg_s.get_blue() / 255));
					} else {
						exampleTexts.push_back(View_Gtk_Theme::createFormattedText(context, *iter, fontName, fg_n.get_red() / 255, fg_n.get_green() / 255, fg_n.get_blue() / 255, bg_n.get_red() / 255, bg_n.get_green() / 255, bg_n.get_blue() / 255));
					}
				}
				this->previewEntryTitles_mutex.unlock();

				int vpos = 0;
				for (std::list<Glib::RefPtr<Pango::Layout> >::iterator iter = exampleTexts.begin(); iter != exampleTexts.end(); iter++) {
					context->move_to(0, vpos);
					(*iter)->show_in_cairo_context(context);
					vpos += (*iter)->get_height();
					int x,y;
					(*iter)->get_pixel_size(x,y);
					vpos += y;
				}
			} else {
				throw Glib::Error();
			}
		} catch (Glib::Error const& e){
			Cairo::RefPtr<Cairo::Context> context = cr ? *cr : drwBackgroundPreview.get_window()->create_cairo_context();
			Glib::RefPtr<Gdk::Pixbuf> buf = drwBackgroundPreview.render_icon_pixbuf(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_DIALOG);

			Gdk::Cairo::set_source_pixbuf(context, buf);
			context->rectangle(0, 0, buf->get_width(), buf->get_height());
			context->fill();
		}

		bttRemoveBackground.show();
		imgRemoveBackground.show();
		lblBackgroundRequiredInfo.hide();
	} else {
		bttRemoveBackground.hide();
		imgRemoveBackground.hide();
		drwBackgroundPreview.hide();
		lblBackgroundRequiredInfo.show();
	}

	bttCopyBackground.set_sensitive(!isInGrubDir);
	this->event_lock = false;
}

std::string View_Gtk_Theme::getBackgroundImagePath(){
	return fcBackgroundImage.get_filename();
}

void View_Gtk_Theme::setPreviewEntryTitles(std::list<std::string> const& entries) {
	this->previewEntryTitles_mutex.lock();
	this->previewEntryTitles = entries;
	this->previewEntryTitles_mutex.unlock();
}

Gtk::Widget& View_Gtk_Theme::getThemeSelector() {
	this->get_vbox()->remove(this->hbTheme);
	return this->hbTheme;
}

Gtk::Widget& View_Gtk_Theme::getEditorBox() {
	this->get_vbox()->remove(this->vbMain);
	return this->vbMain;
}

bool View_Gtk_Theme::signal_redraw_preview(const Cairo::RefPtr<Cairo::Context>& cr) {
	if (!event_lock) {
		this->redraw(this->backgroundImagePath, false, &cr);
	}
	return true;
}


void View_Gtk_Theme::signal_fileAddClick() {
	if (!event_lock) {
		this->controller->addFileAction();
	}
}

void View_Gtk_Theme::signal_fileRemoveClick() {
	if (!event_lock) {
		try {
			this->controller->removeFileAction(this->_getSelectedFileName());
		} catch (ItemNotFoundException const& e) {
			this->log("no file selected - ignoring event", Logger::ERROR);
		}
	}
}

void View_Gtk_Theme::signal_fileSelected() {
	if (!event_lock) {
		try {
			this->controller->updateEditAreaAction(this->_getSelectedFileName());
		} catch (ItemNotFoundException const& e) {
			this->log("no file selected - ignoring event", Logger::INFO);
		}
	}
}

void View_Gtk_Theme::signal_fileRenamed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	if (!event_lock) {
		try {
			this->controller->renameAction(this->_getSelectedFileName());
		} catch (ItemNotFoundException const& e) {
			this->log("no file selected - ignoring event", Logger::ERROR);
		}
	}
}

void View_Gtk_Theme::signal_fileChosen() {
	if (!event_lock) {
		this->controller->loadFileAction(fcFileSelection.get_filename());
	}
}

void View_Gtk_Theme::signal_textChanged() {
	if (!event_lock) {
		this->controller->saveTextAction(txtEdit.get_buffer()->get_text());
	}
}

void View_Gtk_Theme::signal_themeChosen() {
	if (!event_lock) {
		if (this->cbTheme.get_active_row_number() == 0) {
			this->controller->showSimpleThemeConfigAction();
		} else {
			this->controller->loadThemeAction(cbTheme.get_active_text());
		}
	}
}

void View_Gtk_Theme::signal_addThemeClicked() {
	if (!event_lock) {
		this->controller->showThemeInstallerAction();
	}
}

void View_Gtk_Theme::signal_removeThemeClicked() {
	if (!event_lock) {
		Gtk::MessageDialog confirmDlg(gettext("Are you sure you want to remove this theme"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
		confirmDlg.set_default_response(Gtk::RESPONSE_YES);
		if (confirmDlg.run() == Gtk::RESPONSE_YES) {
			this->controller->removeThemeAction(cbTheme.get_active_text());
		}
	}
}

void View_Gtk_Theme::signal_themeFileChooserResponse(int response_id) {
	if (!event_lock) {
		this->fcThemeFileChooser.hide();
		if (response_id == Gtk::RESPONSE_APPLY) {
			this->controller->addThemePackageAction(fcThemeFileChooser.get_filename());
		}
	}
}

void View_Gtk_Theme::signal_dialogResponse(int response_id) {
	if (!event_lock) {
		switch (response_id) {
		case Gtk::RESPONSE_DELETE_EVENT:
		case Gtk::RESPONSE_CLOSE:
			this->hide();
			break;
		case Gtk::RESPONSE_APPLY:
			this->controller->saveAction();
			break;
		default:
			throw NotImplementedException("the given response id is not supported", __FILE__, __LINE__);
		}
	}
}

void View_Gtk_Theme::signal_color_changed(View_Gtk_Theme_ColorChooser& caller){
	if (!event_lock && !caller.event_lock){
		this->controller->updateColorSettingsAction();
	}
}

void View_Gtk_Theme::signal_font_changed() {
	if (!event_lock) {
		this->controller->updateFontSettingsAction(false);
	}
}

void View_Gtk_Theme::signal_font_removed() {
	if (!event_lock) {
		this->controller->updateFontSettingsAction(true);
	}
}

void View_Gtk_Theme::signal_bttRemoveBackground_clicked(){
	if (!event_lock){
		this->controller->removeBackgroundImageAction();
	}
}


void View_Gtk_Theme::signal_other_image_chosen(){
	if (!event_lock){
		this->controller->updateBackgroundImageAction();
	}
}

void View_Gtk_Theme::signal_help_click() {
	if (!event_lock){
		Gtk::MessageDialog helpDlg(
			Glib::ustring::compose(
			gettext("Alternatively to the simple theme method which provides some options like color, wallpaper and font you can install complex theme packages to get an even better looking boot menu.\n\n"
				"There are several download sources like this thread at ubuntuforums.org:\n<a href='%1'>%1</a> (needs account)\n"
				"or search for '%2' at <a href='%3'>%3</a>\n\n"
				"Just download such a package (which is in tar.gz format in most cases) and add it to the list of available themes by using the add button next to the theme chooser.\n\n"
				"After changing the theme you'll see a simple editor which gives a preview of images and allows some file management. Modified theme contents will be saved when you're pressing the save button."),
			"http://ubuntuforums.org/showthread.php?t=1823915",
			"grub",
			"http://gnome-look.org"
			),
			true,
			Gtk::MESSAGE_INFO,
			Gtk::BUTTONS_OK,
			false
		);
		helpDlg.run();
	}
}

