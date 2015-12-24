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

#ifndef ENTRY_ADD_DLG_INCLUDED
#define ENTRY_ADD_DLG_INCLUDED
#include <gtkmm.h>
#include "../../Controller/TrashController.h"
#include "../../Controller/Trait/ControllerAware.h"
#include "../Trash.h"
#include <libintl.h>
#include "../../lib/Trait/LoggerAware.h"
#include "../../lib/Type.h"
#include "Element/List.h"

class View_Gtk_Trash :
	public Gtk::Window,
	public View_Trash,
	public Trait_LoggerAware,
	public Trait_ControllerAware<TrashController>
{
	Gtk::ScrolledWindow scrEntryBox;
	View_Gtk_Element_List<Rule, Script> list;
	Gtk::Frame frmList;
	Gtk::VBox vbList;
	Gtk::HBox hbList;
	Gtk::Button bttRestore;
	Gtk::Button bttDelete;

	std::map<ViewOption, bool> options;

	Gtk::MenuItem miContext;
	Gtk::Menu contextMenu;
	Gtk::ImageMenuItem micRestore;
	Gtk::ImageMenuItem micDelete;

	bool event_lock;
public:
	View_Gtk_Trash();
	void signal_item_dblClick(Gtk::TreeModel::Path const& path, Gtk::TreeViewColumn* column);
	void restore_button_click();
	void delete_button_click();
	void clear();
	std::list<Rule*> getSelectedEntries();
	void addItem(View_Model_ListItem<Rule, Script> const& listItem);
	void setDeleteButtonEnabled(bool val);
	void show();
	void hide();
	void askForDeletion(std::list<std::string> const& names);
	Gtk::Widget& getList();
	void setDeleteButtonVisibility(bool visibility);
	void setOptions(std::map<ViewOption, bool> const& viewOptions);
	void selectEntries(std::list<Rule*> const& entries);
	void setRestoreButtonSensitivity(bool sensitivity);
private:
	void signal_treeview_selection_changed();
	void signal_button_press(GdkEventButton *event);
	bool signal_popup();

};

#endif
