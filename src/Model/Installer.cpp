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

#include "Installer.h"

void Model_Installer::threadable_install(std::string const& device){
	this->install_result = install(device);
	if (controller)
		controller->showMessageAction(this->install_result);
}

std::string Model_Installer::install(std::string const& device){
	FILE* install_proc = popen((this->env->install_cmd+" '"+device+"' 2>&1").c_str(), "r");
	std::string output;
	int c;
	while ((c = fgetc(install_proc)) != EOF){
		output += c;
	}
	int success = pclose(install_proc);
	if (success == 0)
		return ""; //empty return string = no error
	else
		return output;
}

