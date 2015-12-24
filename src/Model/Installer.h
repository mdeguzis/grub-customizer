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

#ifndef GRUB_INSTALLER_INCLUDED
#define GRUB_INSTALLER_INCLUDED
#include <string>
#include "Env.h"
#include "../Controller/InstallerController.h"
#include "../Controller/Trait/ControllerAware.h"
#include "../lib/Trait/LoggerAware.h"

class Model_Installer :
	public Trait_LoggerAware,
	public Trait_ControllerAware<InstallerController>,
	public Model_Env_Connection
{
	std::string install_result;
public:
	void threadable_install(std::string const& device);
	std::string install(std::string const& device);
};

class Model_Installer_Connection {
protected:
	Model_Installer* installer;
public:
	Model_Installer_Connection() : installer(NULL) {}

	void setInstaller(Model_Installer& installer){
		this->installer = &installer;
	}
};

#endif
