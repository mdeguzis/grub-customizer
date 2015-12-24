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

#ifndef CONTROLLERCOLLECTION_H_
#define CONTROLLERCOLLECTION_H_
#include "EntryEditController.h"
#include "MainController.h"
#include "SettingsController.h"
#include "EnvEditorController.h"
#include "TrashController.h"
#include "InstallerController.h"
#include "AboutController.h"
#include "ErrorController.h"
#include "ThemeController.h"
#include "../lib/assert.h"

struct ControllerCollection {
	EntryEditController* entryEditController;
	MainController* mainController;
	SettingsController* settingsController;
	EnvEditorController* envEditController;
	TrashController* trashController;
	InstallerController* installerController;
	AboutController* aboutController;
	ErrorController* errorController;
	ThemeController* themeController;
};

class ControllerCollection_Connection {
private:
	ControllerCollection* _controllerCollection;
protected:
	ControllerCollection& getAllControllers() {
		assert(this->_controllerCollection != NULL);
		return *this->_controllerCollection;
	}
public:
	ControllerCollection_Connection() : _controllerCollection(NULL) {}

	void setControllerCollection(ControllerCollection& controllerCollection) {
		this->_controllerCollection = &controllerCollection;
	}
};


#endif /* CONTROLLERCOLLECTION_H_ */
