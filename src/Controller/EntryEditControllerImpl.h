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

#ifndef ENTRY_EDIT_CONTROLLER_INCLUDED
#define ENTRY_EDIT_CONTROLLER_INCLUDED

#include "../Model/ListCfg.h"
#include "../View/Main.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/Installer.h"
#include "../View/EntryEditor.h"
#include "../View/Trait/ViewAware.h"

#include "../Model/ListCfg.h"
#include "../Model/DeviceDataList.h"
#include "../lib/ContentParserFactory.h"

#include "../lib/Trait/LoggerAware.h"
#include "../Controller/ControllerAbstract.h"
#include "../Controller/Trait/ThreadControllerAware.h"

#include "../lib/Exception.h"


class EntryEditControllerImpl :
	public EntryEditController,
	public ControllerAbstract,
	public View_Trait_ViewAware<View_EntryEditor>,
	public Trait_ThreadControllerAware,
	public Model_ListCfg_Connection,
	public ContentParserFactory_Connection,
	public Model_DeviceDataListInterface_Connection,
	public Model_Env_Connection
{
	ContentParser* currentContentParser;
	Model_Script* _createCustomScript();
public:
	EntryEditControllerImpl();

	void showAction(Rule* rule);
	void showCreatorAction();
	void syncOptionsAction();
	void syncSourceAction();
	void syncEntryEditDlg(bool useOptionsAsSource);
	void switchTypeAction(std::string const& newType);
	
	void applyAction();
};

#endif
