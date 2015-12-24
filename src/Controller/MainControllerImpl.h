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

#ifndef MAINCONNTROLLER_INCLUDED
#define MAINCONNTROLLER_INCLUDED

#include "../Model/ListCfg.h"
#include "../View/Main.h"
#include "../View/Trait/ViewAware.h"
#include <libintl.h>
#include <locale.h>
#include <sstream>
#include <algorithm>
#include "../config.h"

#include "../Model/Env.h"

#include "../Model/ListCfg.h"
#include "../Model/FbResolutionsGetter.h"
#include "../Model/DeviceDataList.h"
#include "ThreadController.h"
#include "../lib/ContentParserFactory.h"

#include "../Controller/ControllerAbstract.h"
#include "../Controller/Trait/ThreadControllerAware.h"

#include "../lib/Trait/LoggerAware.h"
#include "../Mapper/EntryName.h"

#include "../lib/Exception.h"
#include "../View/Model/ListItem.h"

#include "Helper/DeviceInfo.h"

#include "MainController.h"

/**
 * This controller operates on the entry list
 */

class MainControllerImpl :
	public ControllerAbstract,
	public MainController,
	public View_Trait_ViewAware<View_Main>,
	public Trait_ThreadControllerAware,
	public Model_ListCfg_Connection,
	public Model_SettingsManagerData_Connection,
	public Model_FbResolutionsGetter_Connection,
	public Model_DeviceDataList_Connection,
	public Model_MountTable_Connection,
	public ContentParserFactory_Connection,
	public Mapper_EntryName_Connection,
	public Model_Env_Connection
{
	Model_SettingsManagerData* settingsOnDisk; //buffer for the existing settings
	Model_ListCfg* savedListCfg;
	ContentParser* currentContentParser;

	bool config_has_been_different_on_startup_but_unsaved;
	bool is_loading;
	CmdExecException thrownException; //to be used from the die() function

	void _rAppendRule(Model_Rule& rule, Model_Rule* parentRule = NULL);
	bool _listHasPlaintextRules(std::list<Rule*> const& rules);
	bool _listHasAllCurrentSystemRules(std::list<Rule*> const& rules);
	std::list<Rule*> _populateSelection(std::list<Rule*> rules);
	void _populateSelection(std::list<Rule*>& rules, Model_Rule* currentRule, int direction, bool checkScript);
	int _countRulesUntilNextRealRule(Model_Rule* baseRule, int direction);
	std::list<Rule*> _removePlaceholdersFromSelection(std::list<Rule*> rules);
	bool _ruleAffectsCurrentDefaultOs(Model_Rule* rule, std::string const& currentRulePath, std::string const& currentDefaultRulePath);
	void _updateCurrentDefaultOs(Model_Rule* rule, std::string const& currentRulePath, std::string currentDefaultRulePath);

public:
	void setSettingsBuffer(Model_SettingsManagerData& settings);
	void setSavedListCfg(Model_ListCfg& savedListCfg);

	ThreadController& getThreadController();
	Model_FbResolutionsGetter& getFbResolutionsGetter();

	//init functions
	void init();
	void init(Model_Env::Mode mode, bool initEnv = true);
	void initAction();
	void reInitAction(bool burgMode);
	void showEnvEditorAction(bool resetPartitionChooser = false);
	void cancelBurgSwitcherAction();

	void reloadAction();
	void loadThreadedAction(bool preserveConfig = false);
	void saveAction();
	void saveThreadedAction();
	void showConfigSavingErrorAction(std::string errorMessage);
	MainControllerImpl();
	
public:
	void renameEntry(Model_Rule* rule, std::string const& newName);
	void reset();
	
	void showInstallerAction();
	
	void showEntryEditorAction(Rule* rule);
	void showEntryCreatorAction();
	
	//dispatchers
	void dieAction();
	void updateList();
	
	void exitAction(bool force = false);
	
	void removeRulesAction(std::list<Rule*> rules, bool force = false);
	void renameRuleAction(Rule* entry, std::string const& newText);
	void moveAction(std::list<Rule*> rules, int direction);
	void createSubmenuAction(std::list<Rule*> childItems);
	void removeSubmenuAction(std::list<Rule*> childItems);
	
	void revertAction();

	void showProxyInfo(Model_Proxy* proxy);

	void showAboutAction();

	void syncLoadStateThreadedAction();
	void syncSaveStateThreadedAction();

	void syncSaveStateAction();
	void syncLoadStateAction();

	void showSettingsAction();
	void initModeAction(bool burgChosen);
	void addEntriesAction(std::list<Rule*> entries);
	void activateSettingsAction();
	void showReloadRecommendationAction();
	void selectRulesAction(std::list<Rule*> rules);
	void selectRuleAction(Rule* rule, bool startEdit = false);
	void refreshTabAction(unsigned int pos);
	void setViewOptionAction(ViewOption option, bool value);
	void entryStateToggledAction(Rule* entry, bool state);
	void updateSelectionAction(std::list<Rule*> selectedRules);
};

#endif
