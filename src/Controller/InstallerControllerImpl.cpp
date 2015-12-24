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

#include "InstallerControllerImpl.h"

InstallerControllerImpl::InstallerControllerImpl()
	: ControllerAbstract("installer")
{
}


ThreadController& InstallerControllerImpl::getThreadController() {
	if (this->threadController == NULL) {
		throw ConfigException("missing ThreadController", __FILE__, __LINE__);
	}
	return *this->threadController;
}

void InstallerControllerImpl::showAction(){
	this->logActionBegin("show");
	try {
		view->show();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void InstallerControllerImpl::installGrubAction(std::string device){
	this->logActionBegin("install-grub");
	try {
		this->getThreadController().startGrubInstallThread(device);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void InstallerControllerImpl::installGrubThreadedAction(std::string device) {
	this->logActionBeginThreaded("install-grub-threaded");
	try {
		this->env->activeThreadCount++;
		installer->threadable_install(device);
		this->env->activeThreadCount--;
		if (this->env->activeThreadCount == 0 && this->env->quit_requested) {
			this->getAllControllers().mainController->exitAction(true);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}

void InstallerControllerImpl::showMessageAction(std::string const& msg){
	this->logActionBegin("show-message");
	try {
		view->showMessageGrubInstallCompleted(msg);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

