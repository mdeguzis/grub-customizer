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

#ifndef ENTRYEDITDLGGTK_H_
#define ENTRYEDITDLGGTK_H_
#include "../EntryEditor.h"
#include "../../lib/Trait/LoggerAware.h"
#include "../../lib/str_replace.h"
#include <libintl.h>
#include <gtkmm.h>
#include "../../Controller/EntryEditController.h"
#include "../../Controller/Trait/ControllerAware.h"
#include "EntryEditor.h"
#include "../../Model/DeviceDataListInterface.h"
#include "Element/PartitionChooser.h"

class View_Gtk_EntryEditor :
	public View_EntryEditor,
	public Gtk::Dialog,
	public Trait_LoggerAware,
	public Trait_ControllerAware<EntryEditController>,
	public Model_DeviceDataListInterface_Connection
{
	Gtk::Notebook tabbox;
	Gtk::TextView tvSource;
	Gtk::ScrolledWindow scrSource;
	Gtk::ScrolledWindow scrOptions;
	Gtk::Table tblOptions;
	std::map<std::string, Gtk::Widget*> optionMap;
	std::map<std::string, Gtk::Label*> labelMap;
	Gtk::ComboBoxText cbType;
	Gtk::Label lblType;
	bool lock_state;

	Rule* rulePtr;
protected:
	virtual std::string mapOptionName(std::string const& name);
public:
	View_Gtk_EntryEditor();
	void setSourcecode(std::string const& source);
	std::string getSourcecode();
	void addOption(std::string const& name, std::string const& value);
	void setOptions(std::map<std::string, std::string> options);
	std::map<std::string, std::string> getOptions() const;
	void removeOptions();
	void setRulePtr(Rule* rulePtr);
	Rule* getRulePtr();
	void show();
	void hide();

	void setAvailableEntryTypes(std::list<std::string> const& names);
	void selectType(std::string const& name);
	std::string getSelectedType() const;

	void signal_response_action(int response_id);

	bool signal_sourceModified(GdkEventKey* event);
	void signal_optionsModified();
	void signal_typeModified();
};

#endif /* ENTRYEDITDLGGTK_H_ */
