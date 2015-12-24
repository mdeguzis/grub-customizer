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

#ifndef ENVEDITORCONTROLLERIMPL_H_
#define ENVEDITORCONTROLLERIMPL_H_

#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/MountTable.h"

#include "../View/EnvEditor.h"
#include "../View/Trait/ViewAware.h"

#include "ControllerAbstract.h"

#include "EnvEditorController.h"
#include "../Model/DeviceMap.h"


class EnvEditorControllerImpl :
	public ControllerAbstract,
	public EnvEditorController,
	public View_Trait_ViewAware<View_EnvEditor>,
	public Model_Env_Connection
{
	Model_MountTable* mountTable;
	Model_DeviceMap* deviceMap;

public:
	void setMountTable(Model_MountTable& mountTable);
	void setDeviceMap(Model_DeviceMap& deviceMap);

	void showAction(bool resetPartitionChooser = false);

	EnvEditorControllerImpl();
	
	//partition chooser
	void mountSubmountpointAction(std::string const& submountpoint);
	void umountSubmountpointAction(std::string const& submountpoint);
	void generateSubmountpointSelection(std::string const& prefix);

	// env editor
	void switchPartitionAction(std::string const& newPartition);
	void switchBootloaderTypeAction(int newTypeIndex);
	void updateGrubEnvOptionsAction();
	void applyAction(bool saveConfig);

	void exitAction();
};

#endif
