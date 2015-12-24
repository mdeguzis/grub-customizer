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

#include "ErrorControllerImpl.h"
#include "ThreadController.h"

ErrorControllerImpl::ErrorControllerImpl()
	: ControllerAbstract("error"),
	  applicationStarted(false)
{
}


void ErrorControllerImpl::setApplicationStarted(bool val) {
	this->applicationStarted = val;
}

void ErrorControllerImpl::errorAction(Exception const& e){
	this->log(e, Logger::EXCEPTION);
	this->view->showErrorMessage(e, this->applicationStarted);
}

void ErrorControllerImpl::errorThreadedAction(Exception const& e) {
	if (this->threadController) {
		this->threadController->showException(e);
	} else {
		this->log(e, Logger::EXCEPTION);
		exit(1);
	}
}

void ErrorControllerImpl::quitAction() {
	if (this->applicationStarted) {
		this->getAllControllers().mainController->exitAction(true);
	} else {
		exit(2);
	}
}
