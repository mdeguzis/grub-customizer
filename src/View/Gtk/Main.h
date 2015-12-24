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

#ifndef GRUBLIST_CFG_DLG_GTK_INCLUDED
#define GRUBLIST_CFG_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include <libintl.h>
#include "../../config.h"
#include "../Main.h"
#include "../../lib/Trait/LoggerAware.h"
#include "Settings.h"
#include "../../lib/str_replace.h"
#include "../../lib/assert.h"
#include "../../Controller/MainController.h"
#include "../../Controller/Trait/ControllerAware.h"
#include "../../lib/Type.h"
#include "Element/List.h"

class ImageMenuItemOwnKey : public Gtk::ImageMenuItem {
	public:
	ImageMenuItemOwnKey(const Gtk::StockID& id, const Gtk::AccelKey& accel_key);
};

class View_Gtk_Main : public View_Main, public Trait_LoggerAware, public Trait_ControllerAware<MainController> {
	Gtk::Window win;
	Gtk::VBox vbMainSplit;
	Gtk::Notebook notebook;
	
	Gtk::MenuBar menu;
	Gtk::Toolbar toolbar;
	Gtk::ScrolledWindow scrEntryList;
	Gtk::InfoBar infoReloadRequired;
	Gtk::Label lblReloadRequired;
	Gtk::InfoBar infoScriptUpdatesFound;
	Gtk::Label lblScriptUpdatesFound;

	Gtk::ScrolledWindow scrCommonSettings, scrAppearanceSettings;

	Gtk::VBox vbEntryList;
	Gtk::Statusbar statusbar;
	
	View_Gtk_Element_List<Rule, Proxy> tvConfList;
	Gtk::ProgressBar progressBar;
	Gtk::HPaned hpLists;
	Gtk::Widget* trashList;

	Gtk::ToolButton tbttRemove, tbttUp, tbttDown, tbttSave, tbttReload, tbttLeft, tbttRight, tbttEditEntry, tbttRevert, tbttCreateEntry;
	Gtk::ToolItem ti_sep1;
	Gtk::VSeparator vs_sep1;
	Gtk::ToolItem ti_sep2;
	Gtk::VSeparator vs_sep2;
	Gtk::ToolItem ti_sep3;
	Gtk::VSeparator vs_sep3;
	Gtk::ToolItem ti_sep4;
	Gtk::VSeparator vs_sep4;
	Gtk::SeparatorToolItem ti_sep5;
	
	Gtk::MenuItem miFile, miEdit, miView, miHelp, miInstallGrub, miContext, miCAboutEntryTypes, miAboutEntryTypes;
	Gtk::ImageMenuItem miExit, miSave, miAbout, miModifyEnvironment, miRevert, miCreateEntry;
	ImageMenuItemOwnKey miReload, miRemove, miUp, miDown, miLeft, miRight, miEditEntry;
	Gtk::ImageMenuItem miCRemove, miCUp, miCDown, miCLeft, miCRight, miCRename, miCEditEntry;
	Gtk::CheckMenuItem miShowDetails, miShowHiddenEntries, miGroupByScript, miShowPlaceholders;
	Gtk::Menu subFile, subEdit, subView, subHelp, contextMenu;
	
	Gtk::VBox settingsHBox;

	bool lock_state;

	std::map<ViewOption, bool> options;

	Gtk::MessageDialog burgSwitcher;

	Gtk::Button bttAdvancedSettings1, bttAdvancedSettings2;
	Gtk::HButtonBox bbxAdvancedSettings1, bbxAdvancedSettings2;


	void update_move_buttons();
	void update_remove_button();
	void saveConfig();
	void updateButtonsState();
	bool selectedEntriesAreOnSameLevel();
	bool selectedEntriesAreSubsequent();
	std::list<Rule*> getSelectedRules();
	void _rDisableRules(Gtk::TreeNodeChildren const& list);
public:
	View_Gtk_Main();
	void putSettingsDialog(Gtk::VBox& commonSettingsPane, Gtk::VBox& appearanceSettingsPane);
	void putEnvEditor(Gtk::Widget& envEditor);
	void putTrashList(Gtk::Widget& trashList);
	void show();
	void hide();
	void run();
	void close();
	void showBurgSwitcher();
	void hideBurgSwitcher();
	bool isVisible();
	
	void setIsBurgMode(bool isBurgMode);

	void setLockState(int state);
	void updateLockState();
	
	void setProgress(double progress);
	void progress_pulse();
	void hideProgressBar();
	void setStatusText(std::string const& new_status_text);
	void setStatusText(std::string const& name, int pos, int max);
	void appendEntry(View_Model_ListItem<Rule, Proxy> const& listItem);
	void showProxyNotFoundMessage();
	std::string createNewEntriesPlaceholderString(std::string const& parentMenu = "", std::string const& sourceScriptName = "");
	std::string createPlaintextString(std::string const& scriptName = "") const;
	
	int showExitConfirmDialog(int type);
	void showErrorMessage(std::string const& msg, std::vector<std::string> const& values);
	void showConfigSavingError(std::string const& message);
	bool askForEnvironmentSettings(std::string const& failedCmd, std::string const& errorMessage);
	void clear();
	bool confirmUnsavedSwitch();
	
	void setRuleName(Rule* rule, std::string const& newName);

	void selectRule(Rule* rule, bool startEdit = false);

	void selectRules(std::list<Rule*> rules);

	void setTrashPaneVisibility(bool value);

	void showReloadRecommendation();
	void hideReloadRecommendation();

	void showScriptUpdateInfo();
	void hideScriptUpdateInfo();

	void showPlaintextRemoveWarning();
	void showSystemRuleRemoveWarning();
	void setOption(ViewOption option, bool value);
	std::map<ViewOption, bool> const& getOptions();
	void setOptions(std::map<ViewOption, bool> const& options);
	void setEntryVisibility(Rule* entry, bool value);
private:
	//event handlers
	void signal_show_envEditor();
	void signal_checkbox_toggled(Glib::ustring const& path);
	void signal_edit_name_finished(const Glib::ustring& path, const Glib::ustring& new_text);
	void signal_move_click(int direction); //direction: -1: one position up, 1: one p. down
	void signal_remove_click();
	void signal_rename_click();
	void signal_reload_click();
	void signal_show_grub_install_dialog_click();
	void signal_move_left_click();
	void signal_move_right_click();
	void signal_treeview_selection_changed();
	void signal_entry_edit_click();
	void signal_entry_create_click();
	bool signal_delete_event(GdkEventAny* event);
	void signal_quit_click();
	void signal_preference_click();
	void signal_entry_type_help_click();
	void signal_info_click();
	void signal_burg_switcher_response(int response_id);
	void signal_edit_name(Gtk::CellEditable* editable, const Glib::ustring& path);
	bool signal_button_press(GdkEventButton *event);
	bool signal_popup();
	void signal_key_press(GdkEventKey* key);
	void signal_revert();
	void signal_reload_recommendation_response(int response_id);
	void signal_tab_changed(Gtk::Widget* page, guint page_num);
	void signal_viewopt_details_toggled();
	void signal_viewopt_checkboxes_toggled();
	void signal_viewopt_script_toggled();
	void signal_viewopt_placeholders_toggled();
};

#endif
