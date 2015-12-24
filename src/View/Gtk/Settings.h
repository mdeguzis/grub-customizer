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

#ifndef SETTING_DLG_GTK_INCLUDED
#define SETTING_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../Settings.h"
#include <libintl.h>
#include "../ColorChooser.h"
#include <string>
#include "../../lib/Trait/LoggerAware.h"
#include "../../Controller/SettingsController.h"
#include "../../Controller/Trait/ControllerAware.h"
#include "../../lib/assert.h"


class View_Gtk_Settings :
	public Gtk::Dialog,
	public View_Settings,
	public Trait_LoggerAware,
	public Trait_ControllerAware<SettingsController>
{
	struct AdvancedSettingsTreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<bool> active;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> old_name;
		Gtk::TreeModelColumn<Glib::ustring> value;
		AdvancedSettingsTreeModel();
	};
	struct CustomOption_obj : public CustomOption {
		CustomOption_obj(std::string name, std::string old_name, std::string value, bool isActive);
	};
	AdvancedSettingsTreeModel asTreeModel;
	Glib::RefPtr<Gtk::ListStore> refAsListStore;
	bool event_lock;
	
	Gtk::Notebook tabbox;
	Gtk::ScrolledWindow scrAllEntries;
	Gtk::TreeView tvAllEntries;
	Gtk::VBox vbAllEntries;
	Gtk::HBox hbAllEntriesControl;
	Gtk::Button bttAddCustomEntry, bttRemoveCustomEntry;

	Gtk::VBox vbCommonSettings, vbAppearanceSettings;
	Gtk::Alignment alignCommonSettings;
	
	Pango::AttrList attrDefaultEntry;
	Pango::Attribute aDefaultEntry;
	//default entry group
	Gtk::Frame groupDefaultEntry;
	Gtk::Alignment alignDefaultEntry;
	Gtk::Label lblDefaultEntry;
	//Gtk::Table tblDefaultEntry;
	Gtk::RadioButton rbDefPredefined, rbDefSaved;
	Gtk::RadioButtonGroup rbgDefEntry;
	Gtk::VBox vbDefaultEntry;
	Gtk::HBox hbDefPredefined;
	//Gtk::SpinButton spDefPosition;
	Gtk::ComboBoxText cbDefEntry;
	std::map<int, std::string> defEntryValueMapping;
	Gtk::Button bttDefaultEntryHelp;
	Gtk::Image imgDefaultEntryHelp;
	
	//view group
	Gtk::Frame groupView;
	Gtk::Alignment alignView;
	Gtk::Label lblView;
	Gtk::VBox vbView;
	Gtk::CheckButton chkShowMenu, chkOsProber;
	Gtk::HBox hbTimeout;
	Gtk::CheckButton chkTimeout;
	Gtk::SpinButton spTimeout;
	Gtk::Label lblTimeout2;
	
	//kernel parameters
	Gtk::Frame groupKernelParams;
	Gtk::Alignment alignKernelParams;
	Gtk::Label lblKernelParams;
	Gtk::VBox vbKernelParams;
	Gtk::Entry txtKernelParams;
	Gtk::CheckButton chkGenerateRecovery;
	
	//screen resolution
	Gtk::Alignment alignResolutionAndTheme;
	Gtk::HBox hbResolutionAndTheme;
	Gtk::HBox hbResolution;
	Gtk::CheckButton chkResolution;
	Gtk::ComboBoxText cbResolution;


	void signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
	void signal_add_row_button_clicked();
	void signal_remove_row_button_clicked();
	void signal_default_entry_predefined_toggeled();
	void signal_default_entry_saved_toggeled();
	void signal_default_entry_changed();
	void signal_showMenu_toggled();
	void signal_osProber_toggled();
	void signal_timeout_changed();
	void signal_timeout_checkbox_toggled();
	void signal_kernelparams_changed();
	void signal_recovery_toggled();
	void signal_chkResolution_toggled();
	void signal_resolution_selected();
	void signal_defEntryHelpClick();
	void on_response(int response_id);
	public:
	View_Gtk_Settings();
	Gtk::VBox& getCommonSettingsPane();
	Gtk::VBox& getAppearanceSettingsPane();
	void show(bool burgMode);
	void hide();
	void addEntryToDefaultEntryChooser(std::string const& labelPathValue, std::string const& labelPathLabel, std::string const& numericPathValue, std::string const& numericPathLabel);
	void clearDefaultEntryChooser();
	void clearResolutionChooser();
	void addResolution(std::string const& resolution);
	std::string getSelectedDefaultGrubValue();
	void addCustomOption(bool isActive, std::string const& name, std::string const& value);
	void selectCustomOption(std::string const& name);
	std::string getSelectedCustomOption();
	void removeAllSettingRows();
	CustomOption getCustomOption(std::string const& name);
	void setActiveDefEntryOption(DefEntryType option);
	DefEntryType getActiveDefEntryOption();
	void setDefEntry(std::string const& defEntry);
	void setShowMenuCheckboxState(bool isActive);
	bool getShowMenuCheckboxState();
	void setOsProberCheckboxState(bool isActive);
	bool getOsProberCheckboxState();
	void showHiddenMenuOsProberConflictMessage();
	void setTimeoutValue(int value);
	void setTimeoutActive(bool active);
	int getTimeoutValue();
	std::string getTimeoutValueString();
	bool getTimeoutActive();
	void setKernelParams(std::string const& params);
	std::string getKernelParams();
	void setRecoveryCheckboxState(bool isActive);
	bool getRecoveryCheckboxState();
	void setResolutionCheckboxState(bool isActive);
	bool getResolutionCheckboxState();
	void setResolution(std::string const& resolution);
	std::string getResolution();
	void putThemeSelector(Gtk::Widget& themeSelector);
	void putThemeEditArea(Gtk::Widget& themeSelector);
};

#endif
