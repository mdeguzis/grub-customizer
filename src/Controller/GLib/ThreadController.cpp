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

#include "ThreadController.h"

GLib_ThreadController::GLib_ThreadController(ControllerCollection& controllers)
	: _controllers(controllers), _cachedException("")
{
	disp_sync_load.connect(sigc::mem_fun(this, &GLib_ThreadController::_execLoadSync));
	disp_sync_save.connect(sigc::mem_fun(this, &GLib_ThreadController::_execSaveSync));
	disp_thread_died.connect(sigc::mem_fun(this, &GLib_ThreadController::_execDie));
	disp_settings_loaded.connect(sigc::mem_fun(this, &GLib_ThreadController::_execActivateSettings));
	disp_updateSettingsDlgResolutionList.connect(sigc::mem_fun(this, &GLib_ThreadController::_execResolutionListUpdate));
	disp_exception.connect(sigc::mem_fun(this, &GLib_ThreadController::_execShowException));
	disp_postSaveActions.connect(sigc::mem_fun(this, &GLib_ThreadController::_execPostSaveActions));
	disp_config_saving_error.connect(sigc::mem_fun(this, &GLib_ThreadController::_execShowConfigSavingError));
}

void GLib_ThreadController::syncEntryList(){
	this->disp_sync_load();
}

void GLib_ThreadController::updateSaveProgress(){
	this->disp_sync_save();
}

void GLib_ThreadController::updateSettingsDlgResolutionList(){
	this->disp_updateSettingsDlgResolutionList();
}

void GLib_ThreadController::showThreadDiedError() {
	this->disp_thread_died();
}

void GLib_ThreadController::enableSettings() {
	this->disp_settings_loaded();
}

void GLib_ThreadController::startEdit(Rule* rule) {
	this->_cachedRulePtr = rule;

	Glib::signal_timeout().connect_once(sigc::mem_fun(this, &GLib_ThreadController::_execRuleEdit), 10);
}

void GLib_ThreadController::startThemeFileEdit(std::string const& fileName) {
	this->_cachedThemeFileName = fileName;
	Glib::signal_timeout().connect_once(sigc::mem_fun(this, &GLib_ThreadController::_execThemeFileEdit), 10);
}

void GLib_ThreadController::doPostSaveActions() {
	this->disp_postSaveActions();
}

void GLib_ThreadController::startLoadThread(bool preserveConfig) {
	Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &GLib_ThreadController::_execLoad), preserveConfig), false);
}

void GLib_ThreadController::startSaveThread() {
	Glib::Thread::create(sigc::mem_fun(this, &GLib_ThreadController::_execSave), false);
}

void GLib_ThreadController::startFramebufferResolutionLoader() {
	Glib::Thread::create(sigc::mem_fun(this, &GLib_ThreadController::_execFbResolutionsGetter), false);
}

void GLib_ThreadController::startGrubInstallThread(std::string const& device) {
	Glib::Thread::create(sigc::bind<std::string>(sigc::mem_fun(this, &GLib_ThreadController::_execInstallGrub), device), false);
}

void GLib_ThreadController::stopApplication() {
	Gtk::Main::quit();
}

void GLib_ThreadController::showException(Exception const& e) {
	this->_cachedException = e;
	this->disp_exception();
}

void GLib_ThreadController::showConfigSavingError(std::string const& message) {
	this->_cachedConfigSavingError = message;
	this->disp_config_saving_error();
}

void GLib_ThreadController::_execLoadSync() {
	this->_controllers.mainController->syncLoadStateAction();
}

void GLib_ThreadController::_execSaveSync() {
	this->_controllers.mainController->syncSaveStateAction();
}

void GLib_ThreadController::_execLoad(bool preserveConfig) {
	this->_controllers.mainController->loadThreadedAction(preserveConfig);
}

void GLib_ThreadController::_execSave() {
	this->_controllers.mainController->saveThreadedAction();
}

void GLib_ThreadController::_execDie() {
	this->_controllers.mainController->dieAction();
}

void GLib_ThreadController::_execActivateSettings() {
	this->_controllers.mainController->activateSettingsAction();
}

void GLib_ThreadController::_execResolutionListUpdate() {
	this->_controllers.settingsController->updateResolutionlistAction();
}

void GLib_ThreadController::_execFbResolutionsGetter() {
	this->_controllers.settingsController->loadResolutionsAction();
}

void GLib_ThreadController::_execInstallGrub(std::string const& device) {
	this->_controllers.installerController->installGrubThreadedAction(device);
}

void GLib_ThreadController::_execShowException() {
	this->_controllers.errorController->errorAction(this->_cachedException);
}

void GLib_ThreadController::_execRuleEdit() {
	if (this->_cachedRulePtr != NULL) {
		this->_controllers.mainController->selectRuleAction(this->_cachedRulePtr, true);
		this->_cachedRulePtr = NULL;
	}
}

void GLib_ThreadController::_execThemeFileEdit() {
	this->_controllers.themeController->startFileEditAction(this->_cachedThemeFileName);
}

void GLib_ThreadController::_execPostSaveActions() {
	this->_controllers.themeController->postSaveAction();
}

void GLib_ThreadController::_execShowConfigSavingError() {
	this->_controllers.mainController->showConfigSavingErrorAction(this->_cachedConfigSavingError);
}
