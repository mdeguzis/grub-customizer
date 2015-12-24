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

#include "Main.h"

ImageMenuItemOwnKey::ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key) : Gtk::ImageMenuItem(id){
	set_accel_key(accel_key);
}

View_Gtk_Main::View_Gtk_Main()
	: tbttRemove(Gtk::Stock::REMOVE), tbttUp(Gtk::Stock::GO_UP), tbttDown(Gtk::Stock::GO_DOWN),
	tbttLeft(Gtk::Stock::GO_BACK), tbttRight(Gtk::Stock::GO_FORWARD),
	tbttSave(Gtk::Stock::SAVE), tbttEditEntry(Gtk::Stock::EDIT),
	miFile(gettext("_File"), true), miExit(Gtk::Stock::QUIT), tbttReload(Gtk::Stock::REFRESH),
	tbttRevert(Gtk::Stock::REVERT_TO_SAVED), tbttCreateEntry(Gtk::Stock::NEW),
	miEdit(gettext("_Edit"), true), miView(gettext("_View"), true), miHelp(gettext("_Help"), true),
	miInstallGrub(gettext("_Install to MBR …"), true),
	miRemove(Gtk::Stock::REMOVE, Gtk::AccelKey('-', Gdk::CONTROL_MASK)), miUp(Gtk::Stock::GO_UP, Gtk::AccelKey('u', Gdk::CONTROL_MASK)), miDown(Gtk::Stock::GO_DOWN, Gtk::AccelKey('d', Gdk::CONTROL_MASK)),
	miLeft(Gtk::Stock::GO_BACK, Gtk::AccelKey('l', Gdk::CONTROL_MASK)), miRight(Gtk::Stock::GO_FORWARD, Gtk::AccelKey('r', Gdk::CONTROL_MASK)),
	miEditEntry(Gtk::Stock::EDIT, Gtk::AccelKey('e', Gdk::CONTROL_MASK)),
	miCRemove(Gtk::Stock::REMOVE), miCUp(Gtk::Stock::GO_UP), miCDown(Gtk::Stock::GO_DOWN),
	miCLeft(Gtk::Stock::GO_BACK), miCRight(Gtk::Stock::GO_FORWARD), miCRename(Gtk::Stock::EDIT), miCEditEntry(Gtk::Stock::EDIT),
	miReload(Gtk::Stock::REFRESH, Gtk::AccelKey("F5")), miSave(Gtk::Stock::SAVE),
	miAbout(Gtk::Stock::ABOUT), miModifyEnvironment(Gtk::Stock::OPEN), miRevert(Gtk::Stock::REVERT_TO_SAVED),
	miAboutEntryTypes("About the entry types…"), miCAboutEntryTypes("About the entry types…"),
	miCreateEntry(Gtk::Stock::NEW), miShowDetails(gettext("_Show details"), true), miShowHiddenEntries(gettext("Show _hidden entries"), true),
	miGroupByScript(gettext("_Group by Script"), true), miShowPlaceholders(gettext("Show _Placeholders"), true),
	lock_state(~0), burgSwitcher(gettext("BURG found!"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO),
	bttAdvancedSettings1(gettext("advanced settings")), bttAdvancedSettings2(gettext("advanced settings")),
	bbxAdvancedSettings1(Gtk::BUTTONBOX_END), bbxAdvancedSettings2(Gtk::BUTTONBOX_END),
	lblReloadRequired(gettext("The modifications you've done affects the visible entries. Please reload!"), Pango::ALIGN_LEFT),
	lblScriptUpdatesFound(gettext("Script updates found. Click save to apply the changes!"), Pango::ALIGN_LEFT),
	trashList(NULL)
{
	win.set_icon_name("grub-customizer");

	win.set_default_size(900,600);
	win.add(vbMainSplit);
	
	vbMainSplit.pack_start(menu, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(toolbar, Gtk::PACK_SHRINK);
	vbMainSplit.pack_start(notebook);
	vbMainSplit.pack_start(statusbar, Gtk::PACK_SHRINK);

	notebook.append_page(hpLists, gettext("_List configuration"), true);
	hpLists.pack1(vbEntryList, Gtk::FILL | Gtk::EXPAND);

	vbEntryList.pack_start(infoReloadRequired, Gtk::PACK_SHRINK);
	vbEntryList.pack_start(infoScriptUpdatesFound, Gtk::PACK_SHRINK);
	vbEntryList.pack_start(scrEntryList);
	scrEntryList.add(tvConfList);
	statusbar.add(progressBar);
	
	scrEntryList.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrEntryList.set_shadow_type(Gtk::SHADOW_IN);
	
	infoReloadRequired.set_message_type(Gtk::MESSAGE_WARNING);
	infoReloadRequired.add_button(Gtk::Stock::REFRESH, Gtk::RESPONSE_APPLY);
	dynamic_cast<Gtk::Container&>(*infoReloadRequired.get_content_area()).add(lblReloadRequired);
	infoReloadRequired.set_no_show_all(true);

	infoScriptUpdatesFound.set_message_type(Gtk::MESSAGE_INFO);
	lblScriptUpdatesFound.set_line_wrap(true);
	dynamic_cast<Gtk::Container&>(*infoScriptUpdatesFound.get_content_area()).add(lblScriptUpdatesFound);
	infoScriptUpdatesFound.set_no_show_all(true);

	progressBar.set_pulse_step(0.1);
	
	//toolbar
	toolbar.append(tbttSave);
	Glib::RefPtr<Gtk::StyleContext> context = toolbar.get_style_context();
	context->add_class(GTK_STYLE_CLASS_PRIMARY_TOOLBAR);

	tbttSave.set_is_important(true);
	
	ti_sep1.add(vs_sep1);
	toolbar.append(ti_sep1);

	toolbar.append(tbttRemove);
	tbttRemove.set_tooltip_text(gettext("Remove selected entries (you can restore them from trash)"));
	tbttRemove.set_is_important(true);
	
	toolbar.append(tbttEditEntry);
	toolbar.append(tbttCreateEntry);

	ti_sep2.add(vs_sep2);
	toolbar.append(ti_sep2);
	
	toolbar.append(tbttUp);
	tbttUp.set_tooltip_text(gettext("Move up the selected entry or script"));
	toolbar.append(tbttDown);
	tbttDown.set_tooltip_text(gettext("Move down the selected entry or script"));

	ti_sep3.add(vs_sep3);
	toolbar.append(ti_sep3);

	toolbar.append(tbttLeft);
	tbttLeft.set_tooltip_text(gettext("remove this entry from the current submenu"));

	toolbar.append(tbttRight);
	tbttRight.set_tooltip_text(gettext("add this entry to a new submenu"));

	ti_sep4.add(vs_sep4);
	toolbar.append(ti_sep4);
	
	toolbar.append(tbttReload);

	toolbar.append(ti_sep5);

	ti_sep5.set_expand(true);
	ti_sep5.set_draw(false);

	vs_sep1.set_margin_top(5);
	vs_sep1.set_margin_bottom(5);
	vs_sep2.set_margin_top(5);
	vs_sep2.set_margin_bottom(5);
	vs_sep3.set_margin_top(5);
	vs_sep3.set_margin_bottom(5);
	vs_sep4.set_margin_top(5);
	vs_sep4.set_margin_bottom(5);

	toolbar.append(tbttRevert);
	tbttRevert.set_is_important(true);
	tbttRevert.set_tooltip_text(gettext("Reverts the list to the default order"));

	tbttReload.set_tooltip_text(gettext("reloads the configuration. Unsaved changes will be preserved."));
	
	this->setLockState(3);
	this->options[VIEW_SHOW_DETAILS] = true;
	this->options[VIEW_SHOW_HIDDEN_ENTRIES] = false;
	this->options[VIEW_SHOW_PLACEHOLDERS] = false;
	this->options[VIEW_GROUP_BY_SCRIPT] = false;

	//menu
	menu.append(miFile);
	menu.append(miEdit);
	menu.append(miView);
	menu.append(miHelp);
	
	miFile.set_submenu(subFile);
	miEdit.set_submenu(subEdit);
	miView.set_submenu(subView);
	miHelp.set_submenu(subHelp);
	miContext.set_submenu(contextMenu);

	subFile.attach(miModifyEnvironment, 0,1,0,1);
	subFile.attach(miSave, 0,1,1,2);
	subFile.attach(miInstallGrub, 0,1,2,3);
	subFile.attach(miExit, 0,1,3,4);
	
	subEdit.attach(miRemove, 0,1,1,2);
	subEdit.attach(miUp, 0,1,2,3);
	subEdit.attach(miDown, 0,1,3,4);
	subEdit.attach(miLeft, 0,1,4,5);
	subEdit.attach(miRight, 0,1,5,6);
	subEdit.attach(miEditEntry, 0,1,6,7);
	subEdit.attach(miCreateEntry, 0,1,7,8);
	subEdit.attach(miRevert, 0,1,8,9);
	

	contextMenu.attach(miCRename, 0,1,0,1);
	contextMenu.attach(miCEditEntry, 0,1,1,2);
	contextMenu.attach(miCRemove, 0,1,2,3);
	contextMenu.attach(miCUp, 0,1,3,4);
	contextMenu.attach(miCDown, 0,1,4,5);
	contextMenu.attach(miCLeft, 0,1,5,6);
	contextMenu.attach(miCRight, 0,1,6,7);
	contextMenu.attach(miCAboutEntryTypes, 0,1,7,8);

	miCRename.set_label(gettext("Rename"));
	miUp.set_label(gettext("Move up"));
	miCUp.set_label(gettext("Move up"));
	miDown.set_label(gettext("Move down"));
	miCDown.set_label(gettext("Move down"));
	miLeft.set_label(gettext("Remove from submenu"));
	miCLeft.set_label(gettext("Remove from submenu"));
	miRight.set_label(gettext("Create submenu"));
	miCRight.set_label(gettext("Create submenu"));


	subView.attach(miReload, 0,1,0,1);
	subView.attach(miShowDetails, 0, 1, 1, 2);
	subView.attach(miShowHiddenEntries, 0, 1, 2, 3);
	subView.attach(miGroupByScript, 0, 1, 3, 4);
	subView.attach(miShowPlaceholders, 0, 1, 4, 5);

	miShowDetails.set_active(true);
	miShowHiddenEntries.set_active(false);

	subHelp.attach(miAboutEntryTypes, 0,1,0,1);
	subHelp.attach(miAbout, 0,1,1,2);
	
	miModifyEnvironment.set_label(gettext("_Change Environment …"));
	miModifyEnvironment.set_use_underline(true);
	
	//burg switcher
	burgSwitcher.set_skip_taskbar_hint(false);
	burgSwitcher.set_title("Grub Customizer");
	burgSwitcher.set_icon_name("grub-customizer");
	burgSwitcher.set_secondary_text(gettext("Do you want to configure BURG instead of grub2?"));
	burgSwitcher.set_default_response(Gtk::RESPONSE_YES);

	//signals
	
	tbttUp.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),-1));
	tbttDown.signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),1));
	tvConfList.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_treeview_selection_changed));
	tvConfList.textRenderer.signal_editing_started().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_edit_name));
	tvConfList.textRenderer.signal_edited().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_edit_name_finished));
	tvConfList.toggleRenderer.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_checkbox_toggled));
	tvConfList.signal_button_press_event().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_button_press), false);
	tvConfList.signal_popup_menu().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_popup));
	tvConfList.signal_key_press_event().connect_notify(sigc::mem_fun(this, &View_Gtk_Main::signal_key_press));
	tbttSave.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::saveConfig));
	tbttRemove.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_remove_click));
	tbttLeft.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_left_click));
	tbttRight.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_right_click));
	tbttReload.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_reload_click));
	tbttEditEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_edit_click));
	tbttCreateEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_create_click));
	tbttRevert.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_revert));
	bttAdvancedSettings1.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_preference_click));
	bttAdvancedSettings2.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_preference_click));
	
	miUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),-1));
	miCUp.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),-1));
	miDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),1));
	miCDown.signal_activate().connect(sigc::bind<int>(sigc::mem_fun(this, &View_Gtk_Main::signal_move_click),1));
	miLeft.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_left_click));
	miCLeft.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_left_click));
	miRight.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_right_click));
	miCRight.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_move_right_click));
	miSave.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::saveConfig));
	miEditEntry.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_edit_click));
	miCEditEntry.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_edit_click));
	miCreateEntry.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_create_click));
	miRemove.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_remove_click));
	miCRemove.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_remove_click));
	miCRename.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_rename_click));
	miReload.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_reload_click));
	miInstallGrub.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_show_grub_install_dialog_click));
	miModifyEnvironment.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_show_envEditor));
	miRevert.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_revert));
	miShowDetails.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_viewopt_details_toggled));
	miShowHiddenEntries.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_viewopt_checkboxes_toggled));
	miGroupByScript.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_viewopt_script_toggled));
	miShowPlaceholders.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_viewopt_placeholders_toggled));
	miAboutEntryTypes.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_type_help_click));
	miCAboutEntryTypes.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_entry_type_help_click));

	miExit.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_quit_click));
	miAbout.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_info_click));
	
	burgSwitcher.signal_response().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_burg_switcher_response));

	infoReloadRequired.signal_response().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_reload_recommendation_response));

	win.signal_delete_event().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_delete_event));

	notebook.signal_switch_page().connect(sigc::mem_fun(this, &View_Gtk_Main::signal_tab_changed));
}

void View_Gtk_Main::putSettingsDialog(Gtk::VBox& commonSettingsPane, Gtk::VBox& appearanceSettingsPane) {
//	notebook.append_page(this->settingsHBox, "_settings", true);
	commonSettingsPane.set_border_width(20);
	notebook.append_page(scrCommonSettings, gettext("_General settings"), true);
	scrCommonSettings.add(commonSettingsPane);
	scrCommonSettings.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	commonSettingsPane.pack_end(bbxAdvancedSettings1, false, false);
	bbxAdvancedSettings1.pack_end(bttAdvancedSettings1);

	appearanceSettingsPane.set_border_width(5);
	notebook.append_page(scrAppearanceSettings, gettext("_Appearance settings"), true);
	scrAppearanceSettings.add(appearanceSettingsPane);
	scrAppearanceSettings.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	appearanceSettingsPane.pack_end(bbxAdvancedSettings2, false, false);
	bbxAdvancedSettings2.pack_end(bttAdvancedSettings2);
}

void View_Gtk_Main::putTrashList(Gtk::Widget& trashList) {
	hpLists.pack2(trashList, Gtk::FILL);
	this->trashList = &trashList;
}

void View_Gtk_Main::signal_edit_name(Gtk::CellEditable* editable, const Glib::ustring& path) {
	Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(path);
	Glib::ustring name = (*iter)[this->tvConfList.treeModel.name];
	editable->set_property<Glib::ustring>("text", name);
}

void View_Gtk_Main::setIsBurgMode(bool isBurgMode){
	if (isBurgMode)
		win.set_title("Grub Customizer (" + Glib::ustring(gettext("BURG Mode")) + ")");
	else
		win.set_title("Grub Customizer");

	tbttSave.set_tooltip_text(Glib::ustring(gettext("Save configuration and generate a new "))+(isBurgMode?"burg.cfg":"grub.cfg"));
}

bool View_Gtk_Main::signal_button_press(GdkEventButton *event) {
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		contextMenu.popup(event->button, event->time);
		if (this->tvConfList.get_selection()->count_selected_rows() > 1) {
			return true; // prevent re-selection
		} else {
			return false;
		}
	}
	return false;
}

bool View_Gtk_Main::signal_popup() {
	contextMenu.popup(0, gdk_event_get_time(NULL));
	return true;
}

void View_Gtk_Main::signal_key_press(GdkEventKey* key) {
	if (key->keyval == GDK_KEY_Delete) {
		this->controller->removeRulesAction(this->getSelectedRules());
	}
}

void View_Gtk_Main::signal_revert() {
	Gtk::MessageDialog msgDlg(gettext("Are you sure?"), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL);
	msgDlg.set_secondary_text(gettext("This removes all your list modifications of the bootloader menu!"));
	int response = msgDlg.run();
	if (response == Gtk::RESPONSE_OK) {
		this->controller->revertAction();
	}
}

void View_Gtk_Main::signal_reload_recommendation_response(int response_id) {
	if (response_id == Gtk::RESPONSE_APPLY) {
		this->controller->reloadAction();
	}
}

void View_Gtk_Main::signal_tab_changed(Gtk::Widget* page, guint page_num) {
	if (this->controller && this->lock_state == 0) { // this->eventListener must be called because this event may be propagated from bootstrap
		this->controller->refreshTabAction(page_num);
	}
}

void View_Gtk_Main::signal_viewopt_details_toggled() {
	if (this->controller) {
		this->controller->setViewOptionAction(VIEW_SHOW_DETAILS, this->miShowDetails.get_active());
	}
}

void View_Gtk_Main::signal_viewopt_checkboxes_toggled() {
	if (this->controller) {
		this->controller->setViewOptionAction(VIEW_SHOW_HIDDEN_ENTRIES, this->miShowHiddenEntries.get_active());
	}
}

void View_Gtk_Main::signal_viewopt_script_toggled() {
	if (this->controller) {
		this->controller->setViewOptionAction(VIEW_GROUP_BY_SCRIPT, this->miGroupByScript.get_active());
	}
}

void View_Gtk_Main::signal_viewopt_placeholders_toggled() {
	if (this->controller) {
		this->controller->setViewOptionAction(VIEW_SHOW_PLACEHOLDERS, this->miShowPlaceholders.get_active());
	}
}

void View_Gtk_Main::showBurgSwitcher(){
	burgSwitcher.show();
}

void View_Gtk_Main::hideBurgSwitcher(){
	burgSwitcher.hide();
}

bool View_Gtk_Main::isVisible(){
	return win.get_visible();
}

void View_Gtk_Main::show(){
	win.show_all();
	miContext.show_all();
	assert(trashList != NULL);
	trashList->hide();
}

void View_Gtk_Main::hide() {
	win.hide();
}

void View_Gtk_Main::run(){
	this->show();
	Gtk::Main::run(win);
}

void View_Gtk_Main::setProgress(double progress){
	progressBar.set_fraction(progress);
	progressBar.show();
	statusbar.show();
}

void View_Gtk_Main::progress_pulse(){
	progressBar.pulse();
	progressBar.show();
	statusbar.show();
}

void View_Gtk_Main::hideProgressBar(){
	statusbar.hide();
}

void View_Gtk_Main::setStatusText(std::string const& new_status_text){
	statusbar.push(new_status_text);
}

void View_Gtk_Main::setStatusText(std::string const& name, int pos, int max){
	if (name == "") {
		statusbar.push(gettext("loading configuration…"));
	} else {
		statusbar.push(Glib::ustring::compose(gettext("loading script %2/%3 (%1)"), name, pos, max));
	}
}

void View_Gtk_Main::appendEntry(View_Model_ListItem<Rule, Proxy> const& listItem){
	this->tvConfList.addListItem(listItem, this->options, this->win);

	tvConfList.expand_all();
}



void View_Gtk_Main::showProxyNotFoundMessage(){
	Gtk::MessageDialog msg(gettext("Proxy binary not found!"), false, Gtk::MESSAGE_WARNING);
	msg.set_secondary_text(gettext("You will see all entries (uncustomized) when you run grub. This error accurs (in most cases), when you didn't install grub gustomizer currectly."));
	msg.run();
}

std::string View_Gtk_Main::createNewEntriesPlaceholderString(std::string const& parentMenu, std::string const& sourceScriptName) {
	if (sourceScriptName != "" && parentMenu != "") {
		return Glib::ustring::compose(gettext("(incoming Entries of %1, Script: %2)"), parentMenu, sourceScriptName);
	} else if (parentMenu != "") {
		return Glib::ustring::compose(gettext("(incoming Entries of %1)"), parentMenu);
	} else if (sourceScriptName != "") {
		return Glib::ustring::compose(gettext("(incoming Entries of Script: %1)"), sourceScriptName);
	} else {
		return gettext("(incoming Entries)");
	}
}

std::string View_Gtk_Main::createPlaintextString(std::string const& scriptName) const {
	if (scriptName == "") {
		return gettext("(script code)");
	} else {
		return Glib::ustring::compose(gettext("(script code of %1)"), scriptName);
	}
}

void View_Gtk_Main::saveConfig(){
	controller->saveAction();
}


void View_Gtk_Main::setLockState(int state){
	//state(bin):
	//	0: lock nothing
	//	1: lock normal items/buttons
	//	2: lock grub-install
	//	4: lock partition chooser
	//  8: lock settings dialog
	this->lock_state = state;
	this->updateLockState();
}

void View_Gtk_Main::updateLockState() {
	int state = this->lock_state;
	bool isListView = this->notebook.get_current_page() == 0;

	tbttSave.set_sensitive((state & 1) == 0);
	miSave.set_sensitive((state & 1) == 0);

	tbttUp.set_sensitive((state & 1) == 0 && isListView);
	miUp.set_sensitive((state & 1) == 0 && isListView);
	miCUp.set_sensitive((state & 1) == 0 && isListView);
	tbttDown.set_sensitive((state & 1) == 0 && isListView);
	miDown.set_sensitive((state & 1) == 0 && isListView);
	miCDown.set_sensitive((state & 1) == 0 && isListView);
	tbttLeft.set_sensitive((state & 1) == 0 && isListView);
	miLeft.set_sensitive((state & 1) == 0 && isListView);
	miCLeft.set_sensitive((state & 1) == 0 && isListView);
	tbttRight.set_sensitive((state & 1) == 0 && isListView);
	miRight.set_sensitive((state & 1) == 0 && isListView);
	miCRight.set_sensitive((state & 1) == 0 && isListView);
	tbttEditEntry.set_sensitive((state & 1) == 0 && isListView);
	miEditEntry.set_sensitive((state & 1) == 0 && isListView);
	miCEditEntry.set_sensitive((state & 1) == 0 && isListView);

	tbttRemove.set_sensitive((state & 1) == 0 && isListView);
	miRemove.set_sensitive((state & 1) == 0 && isListView);
	miCRemove.set_sensitive((state & 1) == 0 && isListView);

	miCRename.set_sensitive((state & 1) == 0 && isListView);
	
	tbttReload.set_sensitive((state & 1) == 0);
	miReload.set_sensitive((state & 1) == 0);

	tbttRevert.set_sensitive((state & 1) == 0 && isListView);
	miRevert.set_sensitive((state & 1) == 0 && isListView);

	miModifyEnvironment.set_sensitive((state & 4) == 0);
	bttAdvancedSettings1.set_sensitive((state & 8) == 0);
	bttAdvancedSettings1.set_sensitive((state & 8) == 0);

	tvConfList.set_sensitive((state & 1) == 0);
	if (this->trashList) {
		this->trashList->set_sensitive((state & 1) == 0);
	}

	miCreateEntry.set_sensitive((state & 1) == 0 && isListView);
	tbttCreateEntry.set_sensitive((state & 1) == 0 && isListView);

	miInstallGrub.set_sensitive((state & 2) == 0);
	
	if ((state & 1) == 0 && isListView) {
		this->updateButtonsState();
	}
}


void View_Gtk_Main::updateButtonsState(){
	update_remove_button();
	update_move_buttons();

	std::vector<Gtk::TreeModel::Path> selectedElementents = this->tvConfList.get_selection()->get_selected_rows();
	bool renamableEntrySelected = selectedElementents.size() == 1 && (*this->tvConfList.refTreeStore->get_iter(selectedElementents[0]))[this->tvConfList.treeModel.is_renamable];
	bool editableEntrySelected = selectedElementents.size() == 1 && (*this->tvConfList.refTreeStore->get_iter(selectedElementents[0]))[this->tvConfList.treeModel.is_editable];
	miCRename.set_sensitive(renamableEntrySelected);
	tbttEditEntry.set_sensitive(editableEntrySelected);
	miEditEntry.set_sensitive(editableEntrySelected);
	miCEditEntry.set_sensitive(editableEntrySelected);
}

bool View_Gtk_Main::selectedEntriesAreOnSameLevel() {
	assert(this->tvConfList.get_selection()->count_selected_rows() >= 1);

	std::vector<Gtk::TreeModel::Path> pathes = this->tvConfList.get_selection()->get_selected_rows();

	// first check whether the selected items are entries
	if ((*this->tvConfList.refTreeStore->get_iter(pathes[0]))[this->tvConfList.treeModel.relatedScript]) {
		return false;
	}

	bool result = true;
	if ((*this->tvConfList.refTreeStore->get_iter(pathes[0]))[this->tvConfList.treeModel.is_toplevel]) { // first entry is on toplevel, so all entries should be there
		for (std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
			if (!(*this->tvConfList.refTreeStore->get_iter(*pathIter))[this->tvConfList.treeModel.is_toplevel]) {
				result = false;
				break;
			}
		}
	} else {
		Gtk::TreeModel::Path parent = this->tvConfList.refTreeStore->get_path(this->tvConfList.refTreeStore->get_iter(pathes[0])->parent());
		for (std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
			Gtk::TreeModel::iterator treeIter = this->tvConfList.refTreeStore->get_iter(*pathIter);
			if (!treeIter->parent() || parent != this->tvConfList.refTreeStore->get_path(treeIter->parent())) {
				result = false;
				break;
			}
		}
	}
	return result;
}

bool View_Gtk_Main::selectedEntriesAreSubsequent() {
	assert(this->selectedEntriesAreOnSameLevel());
	if (this->tvConfList.get_selection()->count_selected_rows() == 1) {
		return true;
	}
	std::vector<Gtk::TreeModel::Path> pathes = this->tvConfList.get_selection()->get_selected_rows();

	Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(pathes[0]);

	std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin();
	pathIter++;
	for (; pathIter != pathes.end(); pathIter++) {
		iter++;
		if (iter != this->tvConfList.refTreeStore->get_iter(*pathIter)) {
			return false;
		}
	}
	return true;
}

std::list<Rule*> View_Gtk_Main::getSelectedRules() {
	return this->tvConfList.getSelectedRules();
}


void View_Gtk_Main::_rDisableRules(Gtk::TreeNodeChildren const& list) {
	for (Gtk::TreeNodeChildren::iterator pathIter = list.begin(); pathIter != list.end(); pathIter++) {
		(*pathIter)[this->tvConfList.treeModel.is_renamable] = false;
		if (pathIter->children().size()) {
			this->_rDisableRules(pathIter->children());
		}
	}
}


void View_Gtk_Main::signal_reload_click(){
	controller->reloadAction();
}

void View_Gtk_Main::signal_checkbox_toggled(Glib::ustring const& path) {
	if (!this->lock_state) {
		this->controller->entryStateToggledAction(
			(*this->tvConfList.refTreeStore->get_iter(path))[this->tvConfList.treeModel.relatedRule],
			!(*this->tvConfList.refTreeStore->get_iter(path))[this->tvConfList.treeModel.is_activated]
		);
	}
}

void View_Gtk_Main::signal_edit_name_finished(const Glib::ustring& path, const Glib::ustring& new_text){
	if (this->lock_state == 0){
		Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(path);
		controller->renameRuleAction((Rule*)(*iter)[tvConfList.treeModel.relatedRule], new_text);
	}
}

void View_Gtk_Main::signal_show_envEditor(){
	controller->showEnvEditorAction();
}



void View_Gtk_Main::setRuleName(Rule* rule, std::string const& newName){
	this->setLockState(~0);
	this->tvConfList.setRuleName(rule, newName);
	this->setLockState(0);
}


void View_Gtk_Main::selectRule(Rule* rule, bool startEdit) {
	this->tvConfList.selectRule(rule, startEdit);
}

void View_Gtk_Main::selectRules(std::list<Rule*> rules) {
	this->tvConfList.selectRules(rules);
}

void View_Gtk_Main::setTrashPaneVisibility(bool value) {
	this->trashList->set_visible(value);
}

void View_Gtk_Main::showReloadRecommendation() {
	this->infoReloadRequired.show();
	this->lblReloadRequired.show();
}

void View_Gtk_Main::hideReloadRecommendation() {
	this->infoReloadRequired.hide();
}

void View_Gtk_Main::showScriptUpdateInfo() {
	this->infoScriptUpdatesFound.show();
	this->lblScriptUpdatesFound.show();
}

void View_Gtk_Main::hideScriptUpdateInfo() {
	this->infoScriptUpdatesFound.hide();
}

void View_Gtk_Main::showPlaintextRemoveWarning() {
	Gtk::MessageDialog dlg(gettext("Removing Script Code can cause problems when trying to boot entries relying on it. Are you sure you want to do it anyway?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	dlg.set_default_response(Gtk::RESPONSE_OK);
	int result = dlg.run();
	if (result == Gtk::RESPONSE_YES) {
		controller->removeRulesAction(this->getSelectedRules(), true);
	}
}

void View_Gtk_Main::showSystemRuleRemoveWarning() {
	Gtk::MessageDialog dlg(gettext("You're trying to remove an entry of the currently running system. Make sure there are other working entries of this system!\nIf you just want to remove old kernels: The better way is uninstalling them instead of just hiding them in boot menu."), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
	dlg.set_default_response(Gtk::RESPONSE_OK);
	int result = dlg.run();
	if (result == Gtk::RESPONSE_OK) {
		controller->removeRulesAction(this->getSelectedRules(), true);
	}
}

void View_Gtk_Main::setOption(ViewOption option, bool value) {
	int oldLockState = this->lock_state;
	this->setLockState(~0);
	this->options[option] = value;
	switch (option) {
	case VIEW_SHOW_DETAILS: this->miShowDetails.set_active(value); break;
	case VIEW_SHOW_HIDDEN_ENTRIES:
		this->miShowHiddenEntries.set_active(value);
		this->tvConfList.toggleRenderer.set_visible(value);
		this->tvConfList.pixbufRenderer.set_visible(!value);
		break;
	case VIEW_GROUP_BY_SCRIPT: this->miGroupByScript.set_active(value); break;
	case VIEW_SHOW_PLACEHOLDERS: this->miShowPlaceholders.set_active(value); break;
	default:
		throw LogicException("unexpected option");
	}
	this->setLockState(oldLockState);
}

std::map<ViewOption, bool> const& View_Gtk_Main::getOptions() {
	return this->options;
}

void View_Gtk_Main::setOptions(std::map<ViewOption, bool> const& options) {
	for (std::map<ViewOption, bool>::const_iterator iter = options.begin(); iter != options.end(); iter++) {
		this->setOption(iter->first, iter->second);
	}
}

void View_Gtk_Main::setEntryVisibility(Rule* entry, bool value) {
	this->tvConfList.setEntryVisibility(entry, value);
}

void View_Gtk_Main::signal_move_click(int direction){
	if (this->lock_state == 0){
		assert(direction == 1 || direction == -1);

		//if rule swap
		controller->moveAction(this->getSelectedRules(), direction);
	}
}

void View_Gtk_Main::update_remove_button(){
	bool scriptSelected = false;
	std::vector<Gtk::TreeModel::Path> pathes = tvConfList.get_selection()->get_selected_rows();
	for (std::vector<Gtk::TreeModel::Path>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
		if ((*this->tvConfList.refTreeStore->get_iter(*pathIter))[this->tvConfList.treeModel.relatedScript] != NULL) {
			scriptSelected = true;
		}
	}

	if (tvConfList.get_selection()->count_selected_rows() >= 1 && !scriptSelected){
		tbttRemove.set_sensitive(true);
		miRemove.set_sensitive(true);
		miCRemove.set_sensitive(true);
	} else {
		tbttRemove.set_sensitive(false);
		miRemove.set_sensitive(false);
		miCRemove.set_sensitive(false);
	}
}

void View_Gtk_Main::signal_treeview_selection_changed(){
	if (this->lock_state == 0){
		if (tvConfList.get_selection()->count_selected_rows()) {
			std::vector<Gtk::TreeModel::Path> selectedRows = tvConfList.get_selection()->get_selected_rows();
			Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(selectedRows[0]);

			// all entries must be not renamable while not selected to allow direct toggling of the checkboxes
			this->_rDisableRules(tvConfList.refTreeStore->children());

			if (selectedRows.size() == 1) {
				(*this->tvConfList.refTreeStore->get_iter(selectedRows[0]))[this->tvConfList.treeModel.is_renamable] = (*this->tvConfList.refTreeStore->get_iter(selectedRows[0])).get_value(this->tvConfList.treeModel.is_renamable_real);
			}
		}

		this->controller->updateSelectionAction(this->getSelectedRules());

		this->updateButtonsState();
	}
}

void View_Gtk_Main::signal_entry_edit_click() {
	std::list<Rule*> rules = this->getSelectedRules();
	assert(rules.size() == 1);
	controller->showEntryEditorAction(rules.front());
}

void View_Gtk_Main::signal_entry_create_click() {
	controller->showEntryCreatorAction();
}

void View_Gtk_Main::signal_remove_click() {
	controller->removeRulesAction(this->getSelectedRules());
}

void View_Gtk_Main::signal_rename_click() {
	this->selectRule(this->getSelectedRules().front(), true);
}

void View_Gtk_Main::signal_preference_click(){
	controller->showSettingsAction();
}

void View_Gtk_Main::signal_entry_type_help_click() {
	Gtk::MessageDialog helpDlg(
		Glib::ustring() + gettext("menuentry") + "\n" + gettext(
			"An menuentry which is visible at the grub menu and boots an operating system when activated.")
		+ "\n\n" + gettext("submenu") + "\n" + gettext(
			"A group of other operating systems. Grub shows it as a normal menuentry however when clicking it, it loads "
			"another menu which allows you to choose one the the containing menuentries.")
		+ "\n\n" + gettext("placeholder") + "\n" + gettext(
			"There are two types of this special entry: Incoming entries are the place where Grub Customizer places "
			"new operating systems which are found by grub but still not configured using Grub Customizer. "
			"The Script code is code generated by grub which doesn't look like an menuentry. "
			"In most cases it's used to configure grub. You should only touch it when knowing what it does. "
			"Placeholders are hidden by default.")
		+ "\n\n" + gettext("script") + "\n" + gettext(
			"Scripts are generating menuentries. So every menuentry belongs to a script. They are shown as top level groups "
			"when activated. However in contrast to submenus they aren't shown at the boot menu."),
		true,
		Gtk::MESSAGE_INFO,
		Gtk::BUTTONS_OK,
		false
	);
	helpDlg.set_title(gettext("About the entry types"));
	helpDlg.run();
}

void View_Gtk_Main::update_move_buttons(){
	int selectedRowsCount = tvConfList.get_selection()->count_selected_rows();
	bool is_toplevel = false;
	bool sameLevel = false;
	bool subsequent = false;
	bool isOnTop = false;
	bool isOnBottom = false;

	if (selectedRowsCount >= 1) {
		sameLevel = this->selectedEntriesAreOnSameLevel();
		if (sameLevel) {
			subsequent = this->selectedEntriesAreSubsequent();
		}
	}

	if (selectedRowsCount >= 1 && sameLevel) {
		std::vector<Gtk::TreeModel::Path> pathes = tvConfList.get_selection()->get_selected_rows();
		Gtk::TreeModel::iterator iter = this->tvConfList.refTreeStore->get_iter(pathes.front());
		Gtk::TreeModel::iterator lastIter = this->tvConfList.refTreeStore->get_iter(pathes.back());
		is_toplevel = (*iter)[this->tvConfList.treeModel.is_toplevel];

		if (iter == tvConfList.refTreeStore->children().begin()) {
			isOnTop = true;
		}
		if (lastIter == --tvConfList.refTreeStore->children().end()) {
			isOnBottom = true;
		}
	}

	tbttUp.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !isOnTop);
	miUp.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !isOnTop);
	miCUp.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !isOnTop);
	tbttDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !isOnBottom);
	miDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !isOnBottom);
	miCDown.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !isOnBottom);
	tbttLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	miLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	miCLeft.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent && !is_toplevel); //selected entry must be inside a submenu
	tbttRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
	miCRight.set_sensitive(selectedRowsCount >= 1 && sameLevel && subsequent);
}


void View_Gtk_Main::close(){
	win.hide();
	Gtk::Main::quit();
}

/**
 * @param type int: which type of dialog to show (1: changes unsaved, 2: conf not up to date, 3: 1 + 2)
 * @return int: type of the answer: 0: cancel, 1: yes, 2: no
 */
int View_Gtk_Main::showExitConfirmDialog(int type){
	int dlgResponse = Gtk::RESPONSE_NO;
	if (type != 0){
		Gtk::MessageDialog msgDlg("", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE);
		if (type & 2){
			msgDlg.set_message(gettext("The saved configuration is not up to date!"));
			msgDlg.set_secondary_text(gettext("The generated configuration didn't equal to the saved configuration on startup. So what you see now may not be what you see when you restart your pc. To fix this, click update!"));
			
			Gtk::Button* btnQuit = msgDlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_NO);
			btnQuit->set_label(gettext("_Quit without update"));
			msgDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			Gtk::Button* btnSave = msgDlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
			btnSave->set_label(gettext("_Update & Quit"));
		}
		if ((type & 1) && !(type & 2)){
			msgDlg.property_message_type() = Gtk::MESSAGE_QUESTION;
			msgDlg.set_message(gettext("Do you want to save your modifications?"));

			Gtk::Button* btnQuit = msgDlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_NO);
			btnQuit->set_label(gettext("_Quit without saving"));
			msgDlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			Gtk::Button* btnSave = msgDlg.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_YES);
			btnSave->set_label(gettext("_Save & Quit"));
		}
		if ((type & 3) == 3){
			msgDlg.set_secondary_text(msgDlg.property_secondary_text()+"\n\n"+gettext("AND: your modifications are still unsaved, update will save them too!"));
		}

		msgDlg.set_default_response(Gtk::RESPONSE_YES);

		dlgResponse = msgDlg.run();
	}
	switch (dlgResponse){
		case Gtk::RESPONSE_CANCEL: return 0;
		case Gtk::RESPONSE_YES: return 1;
		default: return 2;
	}
}

bool View_Gtk_Main::signal_delete_event(GdkEventAny* event){ //return value: keep window open
	controller->exitAction();
	return true;
}

void View_Gtk_Main::signal_quit_click(){
	controller->exitAction();
}


void View_Gtk_Main::signal_show_grub_install_dialog_click(){
	controller->showInstallerAction();
}

void View_Gtk_Main::signal_move_left_click() {
	controller->removeSubmenuAction(this->getSelectedRules());
}

void View_Gtk_Main::signal_move_right_click() {
	controller->createSubmenuAction(this->getSelectedRules());
}

void View_Gtk_Main::showErrorMessage(std::string const& msg, std::vector<std::string> const& values = std::vector<std::string>()){
	Glib::ustring msg2 = msg;
	switch (values.size()) {
	case 1:	msg2 = Glib::ustring::compose(msg, values[0]); break;
	case 2:	msg2 = Glib::ustring::compose(msg, values[0], values[1]); break;
	case 3:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2]); break;
	case 4:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3]); break;
	case 5:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4]); break;
	case 6:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5]); break;
	case 7:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5], values[6]); break;
	case 8:	msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7]); break;
	case 9: msg2 = Glib::ustring::compose(msg, values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], values[8]); break;
	}
	Gtk::MessageDialog(msg2, false, Gtk::MESSAGE_ERROR).run();
}

void View_Gtk_Main::showConfigSavingError(std::string const& message) {
	Gtk::MessageDialog dlg(gettext("Failed saving grub configuration!"), false, Gtk::MESSAGE_ERROR);
	dlg.set_title(gettext("The grub configuration cannot be saved"));
	dlg.set_secondary_text(Glib::ustring::compose(gettext("Please take a look at the command line output below. If you think this is a bug of Grub Customizer, please create one at %1! Generally Grub Customizer should prevent errors like this."), "https://launchpad.net/grub-customizer") + "\n\n" + message);
	dlg.run();
}

bool View_Gtk_Main::askForEnvironmentSettings(std::string const& failedCmd, std::string const& errorMessage) {
	Glib::ustring msg = Glib::ustring::compose(gettext("%1 couldn't be executed successfully. error message:\n %2"), failedCmd, errorMessage);
	Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_NONE);
	dlg.add_button(gettext("Environment settings"), Gtk::RESPONSE_YES);
	dlg.add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_CLOSE);
	dlg.set_default_response(Gtk::RESPONSE_CLOSE);
	int response = dlg.run();
	return response == Gtk::RESPONSE_YES;
}

void View_Gtk_Main::clear(){
	tvConfList.refTreeStore->clear();
}

bool View_Gtk_Main::confirmUnsavedSwitch() {
	Gtk::MessageDialog dlg(gettext("Do you want to proceed without saving the current configuration?"), false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
	dlg.set_title(gettext("There are unsaved modifications!"));
	dlg.set_default_response(Gtk::RESPONSE_YES);
	int response = dlg.run();
	return response == Gtk::RESPONSE_YES;
}

void View_Gtk_Main::signal_info_click(){
	controller->showAboutAction();
}

void View_Gtk_Main::signal_burg_switcher_response(int response_id){
	if (response_id == Gtk::RESPONSE_DELETE_EVENT)
		controller->cancelBurgSwitcherAction();
	else
		controller->initModeAction(response_id == Gtk::RESPONSE_YES);
}

