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

#include "Settings.h"

View_Gtk_Settings::CustomOption_obj::CustomOption_obj(std::string name, std::string old_name, std::string value, bool isActive){
	this->name = name;
	this->old_name = old_name;
	this->value = value;
	this->isActive = isActive;
}

View_Gtk_Settings::View_Gtk_Settings()
	: event_lock(false), bttAddCustomEntry(Gtk::Stock::ADD), bttRemoveCustomEntry(Gtk::Stock::REMOVE),
	rbDefPredefined(gettext("pre_defined: "), true), rbDefSaved(gettext("previously _booted entry"), true),
	lblDefaultEntry(gettext("default entry")), lblView(gettext("visibility")), chkShowMenu(gettext("show menu")),
	lblKernelParams(gettext("kernel parameters")),
	chkGenerateRecovery(gettext("generate recovery entries")), chkOsProber(gettext("look for other operating systems")),
	chkResolution(gettext("custom resolution: ")), cbResolution(true),
	imgDefaultEntryHelp(Gtk::Stock::HELP, Gtk::ICON_SIZE_BUTTON)
{
	this->set_title("Grub Customizer - "+Glib::ustring(gettext("settings")));
	this->set_icon_name("grub-customizer");
	Gtk::Box* winBox = this->get_vbox();
	winBox->pack_start(tabbox);
	tabbox.append_page(alignCommonSettings, gettext("_General"), true);
	tabbox.append_page(vbAppearanceSettings, gettext("A_ppearance"), true);
	tabbox.append_page(vbAllEntries, gettext("_Advanced"), true);
	
	vbAllEntries.pack_start(hbAllEntriesControl, Gtk::PACK_SHRINK);
	vbAllEntries.pack_start(scrAllEntries);
	hbAllEntriesControl.add(bttAddCustomEntry);
	hbAllEntriesControl.add(bttRemoveCustomEntry);
	hbAllEntriesControl.set_border_width(5);
	hbAllEntriesControl.set_spacing(5);
	scrAllEntries.add(tvAllEntries);
	scrAllEntries.set_border_width(5);
	scrAllEntries.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	refAsListStore = Gtk::ListStore::create(asTreeModel);
	tvAllEntries.set_model(refAsListStore);
	tvAllEntries.append_column_editable(gettext("is active"), asTreeModel.active);
	tvAllEntries.append_column_editable(gettext("name"), asTreeModel.name);
	tvAllEntries.append_column_editable(gettext("value"), asTreeModel.value);
	refAsListStore->signal_row_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_setting_row_changed));
	vbCommonSettings.set_spacing(15);
	vbAppearanceSettings.set_spacing(5);
	
	//default entry group
	vbCommonSettings.pack_start(groupDefaultEntry, Gtk::PACK_SHRINK);
	groupDefaultEntry.add(alignDefaultEntry);
	groupDefaultEntry.set_sensitive(false);
	groupDefaultEntry.set_label_widget(lblDefaultEntry);
	lblDefaultEntry.set_attributes(attrDefaultEntry);
	aDefaultEntry = Pango::Attribute::create_attr_weight(Pango::WEIGHT_BOLD);
	attrDefaultEntry.insert(aDefaultEntry);
	alignDefaultEntry.add(vbDefaultEntry);
	vbDefaultEntry.add(hbDefPredefined);
	vbDefaultEntry.add(rbDefSaved);
	
	bttDefaultEntryHelp.add(imgDefaultEntryHelp);

	hbDefPredefined.pack_start(rbDefPredefined, Gtk::PACK_SHRINK);
	hbDefPredefined.pack_start(cbDefEntry);
	hbDefPredefined.pack_start(bttDefaultEntryHelp, Gtk::PACK_SHRINK);

	hbDefPredefined.set_spacing(5);
	vbDefaultEntry.set_spacing(5);
	groupDefaultEntry.set_shadow_type(Gtk::SHADOW_NONE);
	alignDefaultEntry.set_padding(2, 2, 25, 2);
	rbDefPredefined.set_group(rbgDefEntry);
	rbDefSaved.set_group(rbgDefEntry);
	//cbDefEntry.set_sensitive(false);
	
	//set maximum size of the combobox
	Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = cbDefEntry.get_cells();
	Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();
	cellRenderer->set_fixed_size(200, -1);
	
	cbDefEntry.set_wrap_width(2);
	
	//view group
	alignCommonSettings.add(vbCommonSettings);
	alignCommonSettings.set_padding(20, 0, 0, 0);
	vbCommonSettings.pack_start(groupView, Gtk::PACK_SHRINK);
	groupView.add(alignView);
	groupView.set_shadow_type(Gtk::SHADOW_NONE);
	groupView.set_label_widget(lblView);
	lblView.set_attributes(attrDefaultEntry);
	alignView.add(vbView);
	vbView.set_spacing(5);
	alignView.set_padding(2, 2, 25, 2);
	vbView.add(chkShowMenu);
	vbView.add(chkOsProber);
	vbView.add(hbTimeout);
	hbTimeout.set_spacing(5);

	Glib::ustring defaultEntryLabelText = gettext("Boot default entry after %1 Seconds");
	int timeoutInputPos = defaultEntryLabelText.find("%1");

	if (timeoutInputPos != -1) {
		chkTimeout.set_label(defaultEntryLabelText.substr(0, timeoutInputPos));
		lblTimeout2.set_text(defaultEntryLabelText.substr(timeoutInputPos + 2));
	} else {
		chkTimeout.set_label(defaultEntryLabelText);
	}

	hbTimeout.pack_start(chkTimeout, Gtk::PACK_SHRINK);
	hbTimeout.pack_start(spTimeout, Gtk::PACK_SHRINK);
	hbTimeout.pack_start(lblTimeout2, Gtk::PACK_SHRINK);
	
	spTimeout.set_digits(0);
	spTimeout.set_increments(1, 5);
	spTimeout.set_range(0, 1000000);
	
	//kernel parameters
	vbCommonSettings.pack_start(groupKernelParams, Gtk::PACK_SHRINK);
	groupKernelParams.add(alignKernelParams);
	groupKernelParams.set_shadow_type(Gtk::SHADOW_NONE);
	groupKernelParams.set_label_widget(lblKernelParams);
	lblKernelParams.set_attributes(attrDefaultEntry);
	alignKernelParams.add(vbKernelParams);
	alignKernelParams.set_padding(10, 2, 25, 2);
	vbKernelParams.add(txtKernelParams);
	vbKernelParams.add(chkGenerateRecovery);
	vbKernelParams.set_spacing(5);
	
	//screen resolution and theme chooser
	vbAppearanceSettings.pack_start(alignResolutionAndTheme, Gtk::PACK_SHRINK);
	alignResolutionAndTheme.add(hbResolutionAndTheme);
	alignResolutionAndTheme.set_padding(10, 0, 6, 0);
	hbResolutionAndTheme.set_homogeneous(true);
	hbResolutionAndTheme.set_spacing(15);

	//screen resolution
	hbResolutionAndTheme.pack_start(hbResolution);
	hbResolution.pack_start(chkResolution, Gtk::PACK_SHRINK);
	hbResolution.pack_start(cbResolution, Gtk::PACK_EXPAND_WIDGET);
	cbResolution.append("saved");


	//<signals>
	rbDefPredefined.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_default_entry_predefined_toggeled));
	rbDefSaved.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_default_entry_saved_toggeled));
	cbDefEntry.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_default_entry_changed));
	chkShowMenu.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_showMenu_toggled));
	chkOsProber.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_osProber_toggled));
	spTimeout.signal_value_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_timeout_changed));
	chkTimeout.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_timeout_checkbox_toggled));
	txtKernelParams.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_kernelparams_changed));
	chkGenerateRecovery.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_recovery_toggled));
	chkResolution.signal_toggled().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_chkResolution_toggled));
	cbResolution.get_entry()->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_resolution_selected));
	bttAddCustomEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_add_row_button_clicked));
	bttRemoveCustomEntry.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_remove_row_button_clicked));
	bttDefaultEntryHelp.signal_clicked().connect(sigc::mem_fun(this, &View_Gtk_Settings::signal_defEntryHelpClick));

	this->add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
	this->set_default_size(500, 600);
}

Gtk::VBox& View_Gtk_Settings::getCommonSettingsPane() {
	tabbox.remove(alignCommonSettings);
	alignCommonSettings.remove();
	return vbCommonSettings;
}

Gtk::VBox& View_Gtk_Settings::getAppearanceSettingsPane() {
	tabbox.remove(vbAppearanceSettings);
	return vbAppearanceSettings;
}

void View_Gtk_Settings::show(bool burgMode) {
	this->show_all();
}

void View_Gtk_Settings::hide(){
	Gtk::Dialog::hide();
}


void View_Gtk_Settings::on_response(int response_id) {
	this->controller->hideAction();
}

void View_Gtk_Settings::addEntryToDefaultEntryChooser(std::string const& labelPathValue, std::string const& labelPathLabel, std::string const& numericPathValue, std::string const& numericPathLabel){
	event_lock = true;
	this->defEntryValueMapping[this->defEntryValueMapping.size()] = numericPathValue;
	cbDefEntry.append(Glib::ustring::compose(gettext("Entry %1 (by position)"), numericPathLabel));
	this->defEntryValueMapping[this->defEntryValueMapping.size()] = labelPathValue;
	cbDefEntry.append(labelPathLabel);
	cbDefEntry.set_active(0);
	this->groupDefaultEntry.set_sensitive(true);
	event_lock = false;
}

void View_Gtk_Settings::clearDefaultEntryChooser(){
	event_lock = true;
	cbDefEntry.remove_all();
	this->defEntryValueMapping.clear();
	this->groupDefaultEntry.set_sensitive(false); //if there's no entry to select, disable this area
	event_lock = false;
}


void View_Gtk_Settings::clearResolutionChooser(){
	this->cbResolution.remove_all();
}
void View_Gtk_Settings::addResolution(std::string const& resolution){
	this->cbResolution.append(resolution);
}

View_Gtk_Settings::AdvancedSettingsTreeModel::AdvancedSettingsTreeModel(){
	this->add(active);
	this->add(name);
	this->add(old_name);
	this->add(value);
}

void View_Gtk_Settings::addCustomOption(bool isActive, std::string const& name, std::string const& value){
	this->event_lock = true;
	Gtk::TreeModel::iterator newItemIter = refAsListStore->append();
	(*newItemIter)[asTreeModel.active] = isActive;
	(*newItemIter)[asTreeModel.name] = name;
	(*newItemIter)[asTreeModel.old_name] = name;
	(*newItemIter)[asTreeModel.value] = value;
	this->event_lock = false;
}

void View_Gtk_Settings::selectCustomOption(std::string const& name){
	for (Gtk::TreeModel::iterator iter = refAsListStore->children().begin(); iter != refAsListStore->children().end(); iter++){
		if ((*iter)[asTreeModel.old_name] == name){
			tvAllEntries.set_cursor(refAsListStore->get_path(iter), *tvAllEntries.get_column(name == "" ? 1 : 2), name == "");
			break;
		}
	}
}

std::string View_Gtk_Settings::getSelectedCustomOption(){
	std::vector<Gtk::TreeModel::Path> p = tvAllEntries.get_selection()->get_selected_rows();
	if (p.size() == 1)
		return (Glib::ustring)(*refAsListStore->get_iter(p.front()))[asTreeModel.name];
	else
		return "";
}

void View_Gtk_Settings::removeAllSettingRows(){
	this->event_lock = true;
	this->refAsListStore->clear();
	this->event_lock = false;
}

void View_Gtk_Settings::setActiveDefEntryOption(DefEntryType option){
	this->event_lock = true;
	if (option == this->DEF_ENTRY_SAVED) {
		rbDefSaved.set_active(true);
		cbDefEntry.set_sensitive(false);
	}
	else if (option == this->DEF_ENTRY_PREDEFINED) {
		rbDefPredefined.set_active(true);
		cbDefEntry.set_sensitive(true);
	}
	this->event_lock = false;
}

void View_Gtk_Settings::setDefEntry(std::string const& defEntry){
	this->event_lock = true;

	int pos = 0;
	for (std::map<int, std::string>::iterator iter = this->defEntryValueMapping.begin(); iter != this->defEntryValueMapping.end(); iter++) {
		if (iter->second == defEntry) {
			pos = iter->first;
			break;
		}
	}

	cbDefEntry.set_active(pos);
	this->event_lock = false;
}

void View_Gtk_Settings::setShowMenuCheckboxState(bool isActive){
	this->event_lock = true;
	chkShowMenu.set_active(isActive);
	chkTimeout.set_sensitive(isActive);
	if (!isActive) {
		chkTimeout.set_active(true);
	}
	this->event_lock = false;
}

void View_Gtk_Settings::setOsProberCheckboxState(bool isActive){
	this->event_lock = true;
	chkOsProber.set_active(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setTimeoutValue(int value){
	this->event_lock = true;
	spTimeout.set_value(value);
	this->event_lock = false;
}

void View_Gtk_Settings::setTimeoutActive(bool active) {
	this->event_lock = true;
	chkTimeout.set_active(active);
	spTimeout.set_sensitive(active);
	this->event_lock = false;
}

void View_Gtk_Settings::setKernelParams(std::string const& params){
	this->event_lock = true;
	txtKernelParams.set_text(params);
	this->event_lock = false;
}

void View_Gtk_Settings::setRecoveryCheckboxState(bool isActive){
	this->event_lock = true;
	chkGenerateRecovery.set_active(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setResolutionCheckboxState(bool isActive){
	this->event_lock = true;
	chkResolution.set_active(isActive);
	cbResolution.set_sensitive(isActive);
	this->event_lock = false;
}

void View_Gtk_Settings::setResolution(std::string const& resolution){
	this->event_lock = true;
	cbResolution.get_entry()->set_text(resolution);
	this->event_lock = false;
}

std::string View_Gtk_Settings::getSelectedDefaultGrubValue(){
	return this->defEntryValueMapping[cbDefEntry.get_active_row_number()];
}

View_Gtk_Settings::CustomOption View_Gtk_Settings::getCustomOption(std::string const& name){
	for (Gtk::TreeModel::iterator iter = this->refAsListStore->children().begin(); iter != this->refAsListStore->children().end(); iter++){
		if ((*iter)[asTreeModel.old_name] == name)
			return CustomOption_obj(Glib::ustring((*iter)[asTreeModel.name]), Glib::ustring((*iter)[asTreeModel.old_name]), Glib::ustring((*iter)[asTreeModel.value]), (*iter)[asTreeModel.active]);
	}
	throw ItemNotFoundException("requested custom option not found", __FILE__, __LINE__);
}

void View_Gtk_Settings::signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
	if (!event_lock){
		this->controller->updateCustomSettingAction((Glib::ustring)(*iter)[asTreeModel.old_name]);
	}
}

void View_Gtk_Settings::signal_default_entry_predefined_toggeled(){
	if (!event_lock){
		this->controller->updateDefaultSystemAction();
	}
}

View_Gtk_Settings::DefEntryType View_Gtk_Settings::getActiveDefEntryOption(){
	return rbDefSaved.get_active() ? DEF_ENTRY_SAVED : DEF_ENTRY_PREDEFINED;
}

void View_Gtk_Settings::signal_default_entry_saved_toggeled(){
	if (!event_lock){
		this->controller->updateDefaultSystemAction();
	}
}

void View_Gtk_Settings::signal_default_entry_changed(){
	if (!event_lock){
		this->controller->updateDefaultSystemAction();
	}
}

bool View_Gtk_Settings::getShowMenuCheckboxState(){
	return chkShowMenu.get_active();
}

bool View_Gtk_Settings::getOsProberCheckboxState(){
	return chkOsProber.get_active();
}

void View_Gtk_Settings::showHiddenMenuOsProberConflictMessage(){
	Gtk::MessageDialog(Glib::ustring::compose(gettext("This option doesn't work when the \"os-prober\" script finds other operating systems. Disable \"%1\" if you don't need to boot other operating systems."), chkOsProber.get_label())).run();
}


void View_Gtk_Settings::signal_showMenu_toggled(){
	if (!event_lock){
		this->controller->updateShowMenuSettingAction();
	}
}


void View_Gtk_Settings::signal_osProber_toggled(){
	if (!event_lock){
		this->controller->updateOsProberSettingAction();
	}
}

int View_Gtk_Settings::getTimeoutValue(){
	return spTimeout.get_value_as_int();
}

std::string View_Gtk_Settings::getTimeoutValueString() {
	return Glib::ustring::format(this->getTimeoutValue());
}

bool View_Gtk_Settings::getTimeoutActive() {
	return this->chkTimeout.get_active();
}

void View_Gtk_Settings::signal_timeout_changed(){
	if (!event_lock){
		this->controller->updateTimeoutSettingAction();
	}
}

void View_Gtk_Settings::signal_timeout_checkbox_toggled() {
	if (!event_lock){
		this->controller->updateTimeoutSettingAction();
	}
}

std::string View_Gtk_Settings::getKernelParams(){
	return txtKernelParams.get_text();
}


void View_Gtk_Settings::signal_kernelparams_changed(){
	if (!event_lock){
		this->controller->updateKernelParamsAction();
	}
}

bool View_Gtk_Settings::getRecoveryCheckboxState(){
	return chkGenerateRecovery.get_active();
}


void View_Gtk_Settings::signal_recovery_toggled(){
	if (!event_lock){
		this->controller->updateRecoverySettingAction();
	}
}

bool View_Gtk_Settings::getResolutionCheckboxState(){
	return chkResolution.get_active();
}


void View_Gtk_Settings::signal_chkResolution_toggled(){
	if (!event_lock){
		this->controller->updateUseCustomResolutionAction();
	}
}

std::string View_Gtk_Settings::getResolution(){
	return cbResolution.get_entry()->get_text();
}

void View_Gtk_Settings::putThemeSelector(Gtk::Widget& themeSelector) {
	this->hbResolutionAndTheme.pack_start(themeSelector);
}

void View_Gtk_Settings::putThemeEditArea(Gtk::Widget& themeEditArea) {
	this->vbAppearanceSettings.pack_start(themeEditArea);
}

void View_Gtk_Settings::signal_resolution_selected(){
	if (!event_lock){
		this->controller->updateCustomResolutionAction();
	}
}

void View_Gtk_Settings::signal_defEntryHelpClick() {
	if (!event_lock){
		Gtk::MessageDialog helpDlg(
			gettext("This option allows choosing the operating system which should be selected when booting. "
			"By default this always is the first one.\n\nThere are two columns because "
			"there are two ways of selecting the default operating system. "
			"The first way is to say \"always select the operating system at position X\". This means when a new menuentry appears, "
			"the default menuentry may change (because there's another one at Position X).\n\n"
			"The other way is to say \"use the operating system named Linux 123\". "
			"Then it will always point to the same operating system - the position doesn't matter. "
			"When changing the name of an entry using Grub Customizer this option will be updated automatically. "
			"So you won't have to re-select the default entry."),
			true,
			Gtk::MESSAGE_INFO,
			Gtk::BUTTONS_OK,
			false
		);
		helpDlg.run();
	}
}

void View_Gtk_Settings::signal_add_row_button_clicked(){
	if (!event_lock)
		this->controller->addCustomSettingAction();
}
void View_Gtk_Settings::signal_remove_row_button_clicked(){
	if (!event_lock)
		this->controller->removeCustomSettingAction((Glib::ustring)(*tvAllEntries.get_selection()->get_selected())[asTreeModel.name]);
}




