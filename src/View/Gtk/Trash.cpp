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

#include "Trash.h"

View_Gtk_Trash::View_Gtk_Trash()
	: micRestore(Gtk::Stock::ADD),
	  bttRestore(Gtk::Stock::UNDELETE),
	  bttDelete(Gtk::Stock::DELETE),
	  micDelete(Gtk::Stock::DELETE),
	  event_lock(false)
{
	this->set_title(gettext("Add entry from trash"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(650, 500);
	this->add(frmList);
	frmList.set_label(gettext("Removed items"));
	frmList.set_shadow_type(Gtk::SHADOW_NONE);
	frmList.add(vbList);
	vbList.pack_start(scrEntryBox);
	vbList.pack_start(hbList, Gtk::PACK_SHRINK);
	hbList.pack_start(bttRestore);
	hbList.pack_start(bttDelete);
	scrEntryBox.add(list);
	scrEntryBox.set_min_content_width(250);
	
	bttRestore.set_label(gettext("_Restore"));
	bttRestore.set_use_underline(true);
	bttRestore.set_tooltip_text(gettext("Restore selected entries"));
	bttRestore.set_border_width(5);
	bttRestore.set_sensitive(false);

	bttDelete.set_border_width(5);
	bttDelete.set_tooltip_text(gettext("permanently delete selected entries"));
	bttDelete.set_no_show_all(true);

	list.set_tooltip_column(1);

	list.ellipsizeMode = Pango::ELLIPSIZE_END;

	this->micDelete.set_sensitive(false);
	this->micDelete.set_tooltip_text(gettext("delete entries permanently - this action is available on custom entries only"));

	this->miContext.set_submenu(this->contextMenu);
	this->contextMenu.attach(this->micRestore, 0, 1, 0, 1);
	this->contextMenu.attach(this->micDelete, 0, 1, 1, 2);

	this->list.get_selection()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_treeview_selection_changed));
	this->list.signal_row_activated().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_item_dblClick));
	this->list.signal_button_press_event().connect_notify(sigc::mem_fun(this, &View_Gtk_Trash::signal_button_press));
	this->list.signal_popup_menu().connect(sigc::mem_fun(this, &View_Gtk_Trash::signal_popup));
	this->micRestore.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Trash::restore_button_click));
	this->bttRestore.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Trash::restore_button_click));
	this->micDelete.signal_activate().connect(sigc::mem_fun(this, &View_Gtk_Trash::delete_button_click));
	this->bttDelete.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Trash::delete_button_click));
}

void View_Gtk_Trash::clear(){
	event_lock = true;
	list.refTreeStore->clear();
	event_lock = false;
}

void View_Gtk_Trash::signal_item_dblClick(Gtk::TreeModel::Path const& path, Gtk::TreeViewColumn* column) {
	this->list.get_selection()->unselect_all();
	this->list.get_selection()->select(path);
	controller->applyAction();
	this->hide();
}

void View_Gtk_Trash::restore_button_click() {
	controller->applyAction();
}

void View_Gtk_Trash::delete_button_click() {
	controller->deleteCustomEntriesAction();
}

std::list<Rule*> View_Gtk_Trash::getSelectedEntries(){
	std::list<Rule*> result;
	std::vector<Gtk::TreePath> pathes = list.get_selection()->get_selected_rows();
	for (std::vector<Gtk::TreePath>::iterator pathIter = pathes.begin(); pathIter != pathes.end(); pathIter++) {
		Gtk::TreeModel::iterator elementIter = list.refTreeStore->get_iter(*pathIter);
		result.push_back((*elementIter)[list.treeModel.relatedRule]);
	}
	return result;
}

void View_Gtk_Trash::addItem(View_Model_ListItem<Rule, Script> const& listItem){
	this->list.addListItem(listItem, this->options, *this);
}

void View_Gtk_Trash::setDeleteButtonEnabled(bool val) {
	this->bttDelete.set_visible(val);
}

void View_Gtk_Trash::show(){
	this->show_all();
	this->miContext.show_all();
}

void View_Gtk_Trash::hide(){
	this->Gtk::Window::hide();
}

void View_Gtk_Trash::askForDeletion(std::list<std::string> const& names) {
	Glib::ustring question = gettext("This deletes the following entries:");
	question += "\n";
	for (std::list<std::string>::const_iterator iter = names.begin(); iter != names.end(); iter++) {
		question += *iter + "\n";
	}

	int response = Gtk::MessageDialog(question, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL).run();
	if (response == Gtk::RESPONSE_OK) {
		this->controller->deleteCustomEntriesAction();
	}
}

Gtk::Widget& View_Gtk_Trash::getList() {
	this->remove();
	return this->frmList;
}

void View_Gtk_Trash::setDeleteButtonVisibility(bool visibility) {
	bttDelete.set_visible(visibility);
	micDelete.set_sensitive(visibility);
}

void View_Gtk_Trash::setOptions(std::map<ViewOption, bool> const& viewOptions) {
	this->options = viewOptions;
}

void View_Gtk_Trash::selectEntries(std::list<Rule*> const& entries) {
	this->list.selectRules(entries);
}

void View_Gtk_Trash::setRestoreButtonSensitivity(bool sensitivity) {
	this->bttRestore.set_sensitive(sensitivity);
}

void View_Gtk_Trash::signal_treeview_selection_changed() {
	if (!event_lock) {
		this->controller->updateSelectionAction(this->list.getSelectedRules());
	}
}

void View_Gtk_Trash::signal_button_press(GdkEventButton *event) {
	if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
		contextMenu.show_all();
		contextMenu.popup(event->button, event->time);
	}
}

bool View_Gtk_Trash::signal_popup() {
	contextMenu.show_all();
	contextMenu.popup(0, gdk_event_get_time(NULL));
	return true;
}
