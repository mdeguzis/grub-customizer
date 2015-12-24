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

#ifndef GLIBTHREADCONTROLLER_H_INCLUDED
#define GLIBTHREADCONTROLLER_H_INCLUDED
#include "../ThreadController.h"
#include <glibmm/thread.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/main.h>
#include "../ControllerCollection.h"
#include "../../lib/Trait/LoggerAware.h"
#include "../../lib/assert.h"
#include "../../lib/Type.h"

class GLib_ThreadController : public ThreadController, public Trait_LoggerAware {
	ControllerCollection& _controllers;

	Glib::Dispatcher disp_sync_load, disp_sync_save, disp_thread_died, disp_updateSettingsDlgResolutionList, disp_settings_loaded, disp_exception, disp_postSaveActions, disp_config_saving_error;

	Exception _cachedException;
	Rule* _cachedRulePtr;
	std::string _cachedThemeFileName;
	std::string _cachedConfigSavingError;
public:
	GLib_ThreadController(ControllerCollection& controllers);
	void syncEntryList();
	void updateSaveProgress();
	void updateSettingsDlgResolutionList();
	void showThreadDiedError();
	void enableSettings();
	void startLoadThread(bool preserveConfig);
	void startSaveThread();
	void startFramebufferResolutionLoader();
	void startGrubInstallThread(std::string const& device);
	void stopApplication();
	void showException(Exception const& e);
	void showConfigSavingError(std::string const& message);
	void startEdit(Rule* rule);
	void startThemeFileEdit(std::string const& fileName);
	void doPostSaveActions();
private:
	void _execLoadSync();
	void _execSaveSync();
	void _execLoad(bool preserveConfig);
	void _execSave();
	void _execDie();
	void _execActivateSettings();
	void _execResolutionListUpdate();
	void _execFbResolutionsGetter();
	void _execInstallGrub(std::string const& device);
	void _execShowException();
	void _execRuleEdit();
	void _execThemeFileEdit();
	void _execPostSaveActions();
	void _execShowConfigSavingError();
};

#endif
