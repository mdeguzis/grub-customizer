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

#include "MainControllerImpl.h"

MainControllerImpl::MainControllerImpl()
	: ControllerAbstract("main"),
	  settingsOnDisk(NULL),
	  savedListCfg(NULL),
	 config_has_been_different_on_startup_but_unsaved(false),
	 is_loading(false),
	 currentContentParser(NULL),
	 thrownException("")
{
}


void MainControllerImpl::setSettingsBuffer(Model_SettingsManagerData& settings){
	this->settingsOnDisk = &settings;
}

void MainControllerImpl::setSavedListCfg(Model_ListCfg& savedListCfg){
	this->savedListCfg = &savedListCfg;
}

ThreadController& MainControllerImpl::getThreadController() {
	if (this->threadController == NULL) {
		throw ConfigException("missing ThreadController", __FILE__, __LINE__);
	}
	return *this->threadController;
}

Model_FbResolutionsGetter& MainControllerImpl::getFbResolutionsGetter() {
	return *this->fbResolutionsGetter;
}

void MainControllerImpl::updateList() {
	this->view->clear();

	for (std::list<Model_Proxy>::iterator iter = this->grublistCfg->proxies.begin(); iter != this->grublistCfg->proxies.end(); iter++){
		std::string name = iter->getScriptName();
		if ((name != "header" && name != "debian_theme" && name != "grub-customizer_menu_color_helper") || iter->isModified()) {
			View_Model_ListItem<Rule, Proxy> listItem;
			listItem.name = name;
			listItem.scriptPtr = &*iter;
			listItem.is_submenu = true;
			listItem.defaultName = name;
			listItem.isVisible = true;
			this->view->appendEntry(listItem);
			for (std::list<Model_Rule>::iterator ruleIter = iter->rules.begin(); ruleIter != iter->rules.end(); ruleIter++){
				this->_rAppendRule(*ruleIter);
			}
		}
	}
}

void MainControllerImpl::init(){
	if   ( !grublistCfg
		or !view
		or !settings
		or !settingsOnDisk
		or !savedListCfg
		or !fbResolutionsGetter
		or !deviceDataList
		or !mountTable
		or !contentParserFactory
		or !threadController
		or !entryNameMapper
	) {
		throw ConfigException("init(): missing some objects", __FILE__, __LINE__);
	}
	this->log("initializing (w/o specified bootloader type)…", Logger::IMPORTANT_EVENT);
	savedListCfg->verbose = false;

	this->log("reading partition info…", Logger::EVENT);
	FILE* blkidProc = popen("blkid", "r");
	if (blkidProc){
		deviceDataList->clear();
		deviceDataList->loadData(blkidProc);
		pclose(blkidProc);
	}

	mountTable->loadData("");
	mountTable->loadData(PARTCHOOSER_MOUNTPOINT);


	this->env->rootDeviceName = mountTable->getEntryByMountpoint("").device;


	this->log("Loading Framebuffer resolutions (background process)", Logger::EVENT);
	//loading the framebuffer resolutions in background…
	this->getThreadController().startFramebufferResolutionLoader();

	//dir_prefix may be set by partition chooser (if not, the root partition is used)

	this->log("Finding out if this is a live CD", Logger::EVENT);
	//aufs is the virtual root fileSystem used by live cds
	if (mountTable->getEntryByMountpoint("").isLiveCdFs() && env->cfg_dir_prefix == ""){
		this->log("is live CD", Logger::INFO);
		this->env->init(Model_Env::GRUB_MODE, "");
		this->showEnvEditorAction();
	} else {
		this->log("running on an installed system", Logger::INFO);
		std::list<Model_Env::Mode> modes = this->env->getAvailableModes();
		if (modes.size() == 2) {
			this->view->showBurgSwitcher();
		} else if (modes.size() == 1) {
			this->init(modes.front());
		} else if (modes.size() == 0) {
			this->showEnvEditorAction();
		}
	}
}

void MainControllerImpl::init(Model_Env::Mode mode, bool initEnv){
	this->log("initializing (w/ specified bootloader type)…", Logger::IMPORTANT_EVENT);
	if (initEnv) {
		this->env->init(mode, env->cfg_dir_prefix);
	}
	this->view->setLockState(1|4|8);
	this->view->setIsBurgMode(mode == Model_Env::BURG_MODE);
	this->view->show();
	this->view->hideBurgSwitcher();
	this->view->hideScriptUpdateInfo();

	this->log("Checking if the config directory is clean", Logger::EVENT);
	if (this->grublistCfg->cfgDirIsClean() == false) {
		this->log("cleaning up config dir", Logger::IMPORTANT_EVENT);
		this->grublistCfg->cleanupCfgDir();
	}

	this->log("loading configuration", Logger::IMPORTANT_EVENT);
	this->getThreadController().startLoadThread(false);
}

void MainControllerImpl::initAction() {
	this->logActionBegin("init");
	try {
		this->init();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::reInitAction(bool burgMode) {
	this->logActionBegin("re-init");
	try {
		Model_Env::Mode mode = burgMode ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE;
		this->init(mode, false);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::cancelBurgSwitcherAction(){
	this->logActionBegin("cancel-burg-switcher");
	try {
		if (!this->view->isVisible()) {
			this->getThreadController().stopApplication();
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::reloadAction(){
	this->logActionBegin("reload");
	try {
		this->getAllControllers().settingsController->syncAction();
		this->getAllControllers().themeController->syncAction();
		this->view->hideReloadRecommendation();
		this->view->setLockState(1|4|8);
		this->getThreadController().startLoadThread(true);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

//threaded!
void MainControllerImpl::loadThreadedAction(bool preserveConfig){
	this->logActionBeginThreaded("load-threaded");
	try {
		if (!is_loading){ //allow only one load thread at the same time!
			this->log(std::string("loading - preserveConfig: ") + (preserveConfig ? "yes" : "no"), Logger::IMPORTANT_EVENT);
			is_loading = true;
			this->env->activeThreadCount++;

			try {
				this->view->setOptions(this->env->loadViewOptions());
			} catch (FileReadException e) {
				this->log("view options not found", Logger::INFO);
			}
	
			if (!preserveConfig){
				this->log("unsetting saved config", Logger::EVENT);
				this->grublistCfg->reset();
				this->savedListCfg->reset();
				//load the burg/grub settings file
				this->log("loading settings", Logger::IMPORTANT_EVENT);
				this->settings->load();
				this->getThreadController().enableSettings();
			} else {
				this->log("switching settings", Logger::IMPORTANT_EVENT);
				this->settingsOnDisk->load();
				this->settings->save();
			}

			try {
				this->log("loading grub list", Logger::IMPORTANT_EVENT);
				this->grublistCfg->load(preserveConfig);
				this->log("grub list completely loaded", Logger::IMPORTANT_EVENT);
			} catch (CmdExecException const& e){
				this->log("error while loading the grub list", Logger::ERROR);
				this->thrownException = e;
				this->getThreadController().showThreadDiedError();
				return; //cancel
			}

			if (!preserveConfig){
				this->log("loading saved grub list", Logger::IMPORTANT_EVENT);
				if (this->savedListCfg->loadStaticCfg()) {
					this->config_has_been_different_on_startup_but_unsaved = !this->grublistCfg->compare(*this->savedListCfg);
				} else {
					this->log("saved grub list not found", Logger::WARNING);
					this->config_has_been_different_on_startup_but_unsaved = false;
				}
			}
			if (preserveConfig){
				this->log("restoring settings", Logger::IMPORTANT_EVENT);
				this->settingsOnDisk->save();
			}
			this->env->activeThreadCount--;
			this->is_loading = false;
		} else {
			this->log("ignoring load request (only one load thread allowed at the same time)", Logger::WARNING);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}

void MainControllerImpl::saveAction(){
	this->logActionBegin("save");
	try {
		this->config_has_been_different_on_startup_but_unsaved = false;
		this->env->modificationsUnsaved = false; //deprecated
		this->view->hideScriptUpdateInfo();

		this->view->setLockState(1|4|8);
		this->env->activeThreadCount++; //not in save_thead() to be faster set
		this->getThreadController().startSaveThread();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::saveThreadedAction(){
	this->logActionBeginThreaded("save-threaded");
	try {
		this->env->createBackup();
		this->log("writing settings file", Logger::IMPORTANT_EVENT);
		this->settings->save();
		if (this->settings->color_helper_required) {
			this->grublistCfg->addColorHelper();
		}
		this->getAllControllers().themeController->saveAction();
		this->log("writing grub list configuration", Logger::IMPORTANT_EVENT);
		try {
			this->grublistCfg->save();
		} catch (CmdExecException const& e){
			this->threadController->showConfigSavingError(e.getMessage());
		}
		this->env->activeThreadCount--;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}

void MainControllerImpl::showConfigSavingErrorAction(std::string errorMessage) {
	this->logActionBeginThreaded("show-config-saving-error");
	try {
		this->view->showConfigSavingError(errorMessage);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}

void MainControllerImpl::renameEntry(Model_Rule* rule, std::string const& newName){
	if (rule->type != Model_Rule::PLAINTEXT) {

		std::string currentRulePath = this->grublistCfg->getRulePath(*rule);
		std::string currentDefaultRulePath = this->settings->getValue("GRUB_DEFAULT");
		bool updateDefault = this->_ruleAffectsCurrentDefaultOs(rule, currentRulePath, currentDefaultRulePath);

		this->grublistCfg->renameRule(rule, newName);

		if (updateDefault) {
			this->_updateCurrentDefaultOs(rule, currentRulePath, currentDefaultRulePath);
		}

		if (rule->dataSource && this->grublistCfg->repository.getScriptByEntry(*rule->dataSource)->isCustomScript) {
			rule->dataSource->name = newName;
		}

		this->syncLoadStateAction();
		this->view->selectRule(rule);
	}
}

void MainControllerImpl::reset(){
	this->grublistCfg->reset();
	this->settings->clear();
}


void MainControllerImpl::showAboutAction(){
	this->logActionBegin("show-about");
	try {
		this->getAllControllers().aboutController->showAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::showInstallerAction(){
	this->logActionBegin("show-installer");
	try {
		this->getAllControllers().installerController->showAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::showEntryEditorAction(Rule* rule) {
	this->logActionBegin("show-entry-editor");
	try {
		this->getAllControllers().entryEditController->showAction(rule);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::showEntryCreatorAction() {
	this->logActionBegin("show-entry-creator");
	try {
		this->getAllControllers().entryEditController->showCreatorAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::_rAppendRule(Model_Rule& rule, Model_Rule* parentRule){
	bool is_other_entries_ph = rule.type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER;
	bool is_plaintext = rule.dataSource && rule.dataSource->type == Model_Entry::PLAINTEXT;
	bool is_submenu = rule.type == Model_Rule::SUBMENU;

	if (rule.dataSource || is_submenu){
		std::string name = this->entryNameMapper->map(rule.dataSource, rule.outputName, true);

		bool isSubmenu = rule.type == Model_Rule::SUBMENU;
		std::string scriptName = "", defaultName = "";
		if (rule.dataSource) {
			Model_Script* script = this->grublistCfg->repository.getScriptByEntry(*rule.dataSource);
			assert(script != NULL);
			scriptName = script->name;
			if (!is_other_entries_ph && !is_plaintext) {
				defaultName = rule.dataSource->name;
			}
		}
		bool isEditable = rule.type == Model_Rule::NORMAL || rule.type == Model_Rule::PLAINTEXT;
		bool isModified = rule.dataSource && rule.dataSource->isModified;

		// parse content to show additional informations
		std::map<std::string, std::string> options;
		if (rule.dataSource) {
			options = Controller_Helper_DeviceInfo::fetch(rule.dataSource->content, *this->contentParserFactory, *deviceDataList);
		}

		Model_Proxy* proxy = this->grublistCfg->proxies.getProxyByRule(&rule);

		View_Model_ListItem<Rule, Proxy> listItem;
		listItem.name = name;
		listItem.entryPtr = &rule;
		listItem.is_placeholder = is_other_entries_ph || is_plaintext;
		listItem.is_submenu = isSubmenu;
		listItem.scriptName = scriptName;
		listItem.defaultName = defaultName;
		listItem.isEditable = isEditable;
		listItem.isModified = isModified;
		listItem.options = options;
		listItem.isVisible = rule.isVisible;
		listItem.parentEntry = parentRule;
		listItem.parentScript = proxy;
		this->view->appendEntry(listItem);

		if (rule.type == Model_Rule::SUBMENU) {
			for (std::list<Model_Rule>::iterator subruleIter = rule.subRules.begin(); subruleIter != rule.subRules.end(); subruleIter++) {
				this->_rAppendRule(*subruleIter, &rule);
			}
		}
	}
}

bool MainControllerImpl::_listHasPlaintextRules(std::list<Rule*> const& rules) {
	for (std::list<Rule*>::const_iterator iter = rules.begin(); iter != rules.end(); iter++) {
		const Model_Rule* rule = &Model_Rule::fromPtr(*iter);
		if (rule->type == Model_Rule::PLAINTEXT) {
			return true;
		}
	}
	return false;
}

bool MainControllerImpl::_listHasAllCurrentSystemRules(std::list<Rule*> const& rules) {
	int visibleSystemRulesCount = 0;
	int selectedSystemRulesCount = 0;

	Model_Script* linuxScript = NULL;

	// count selected entries related to linux script
	for (std::list<Rule*>::const_iterator iter = rules.begin(); iter != rules.end(); iter++) {
		const Model_Rule* rule = &Model_Rule::fromPtr(*iter);
		if (rule->type == Model_Rule::NORMAL) {
			assert(rule->dataSource != NULL);
			Model_Script* script = this->grublistCfg->repository.getScriptByEntry(*rule->dataSource);
			if (script->name == "linux") {
				selectedSystemRulesCount++;

				linuxScript = script;
			}
		}
	}

	// count all entries and compare counters if there are linux entries
	if (linuxScript != NULL) {
		// check whether it's the last remaining entry
		std::list<Model_Proxy*> proxies = this->grublistCfg->proxies.getProxiesByScript(*linuxScript);
		bool visibleRulesFound = false;
		for (std::list<Model_Proxy*>::iterator proxyIter = proxies.begin(); proxyIter != proxies.end(); proxyIter++) {
			visibleSystemRulesCount += (*proxyIter)->getVisibleRulesByType(Model_Rule::NORMAL).size();
		}

		if (selectedSystemRulesCount == visibleSystemRulesCount) {
			return true;
		}
	}

	return false;
}

std::list<Rule*> MainControllerImpl::_populateSelection(std::list<Rule*> rules) {
	std::list<Rule*> result;
	for (std::list<Rule*>::iterator ruleIter = rules.begin(); ruleIter != rules.end(); ruleIter++) {
		this->_populateSelection(result, &Model_Rule::fromPtr(*ruleIter), -1, *ruleIter == rules.front());
		result.push_back(*ruleIter);
		this->_populateSelection(result, &Model_Rule::fromPtr(*ruleIter), 1, *ruleIter == rules.back());
	}
	// remove duplicates
	std::list<Rule*> result2;
	std::map<Rule*, Rule*> duplicateIndex; // key: pointer to the rule, value: always NULL
	for (std::list<Rule*>::iterator ruleIter = result.begin(); ruleIter != result.end(); ruleIter++) {
		if (duplicateIndex.find(*ruleIter) == duplicateIndex.end()) {
			duplicateIndex[*ruleIter] = NULL;
			result2.push_back(*ruleIter);
		}
	}
	return result2;
}

void MainControllerImpl::_populateSelection(std::list<Rule*>& rules, Model_Rule* baseRule, int direction, bool checkScript) {
	assert(direction == 1 || direction == -1);
	bool placeholderFound = false;
	Model_Rule* currentRule = baseRule;
	do {
		try {
			currentRule = &*this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);
			if (currentRule->dataSource == NULL || baseRule->dataSource == NULL) {
				break;
			}
			Model_Script* scriptCurrent = this->grublistCfg->repository.getScriptByEntry(*currentRule->dataSource);
			Model_Script* scriptBase    = this->grublistCfg->repository.getScriptByEntry(*baseRule->dataSource);

			if ((scriptCurrent == scriptBase || !checkScript) && (currentRule->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || currentRule->type == Model_Rule::PLAINTEXT)) {
				if (direction == 1) {
					rules.push_back(currentRule);
				} else {
					rules.push_front(currentRule);
				}
				placeholderFound = true;
			} else {
				placeholderFound = false;
			}
		} catch (NoMoveTargetException const& e) {
			placeholderFound = false;
		}
	} while (placeholderFound);
}

int MainControllerImpl::_countRulesUntilNextRealRule(Model_Rule* baseRule, int direction) {
	int result = 1;
	bool placeholderFound = false;
	Model_Rule* currentRule = baseRule;
	do {
		try {
			currentRule = &*this->grublistCfg->proxies.getNextVisibleRule(currentRule, direction);

			if (currentRule->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || currentRule->type == Model_Rule::PLAINTEXT) {
				result++;
				placeholderFound = true;
			} else {
				placeholderFound = false;
			}
		} catch (NoMoveTargetException const& e) {
			placeholderFound = false;
		}
	} while (placeholderFound);
	return result;
}

std::list<Rule*> MainControllerImpl::_removePlaceholdersFromSelection(std::list<Rule*> rules) {
	std::list<Rule*> result;
	for (std::list<Rule*>::iterator ruleIter = rules.begin(); ruleIter != rules.end(); ruleIter++) {
		Model_Rule* rule = &Model_Rule::fromPtr(*ruleIter);
		if (!(rule->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || rule->type == Model_Rule::PLAINTEXT)) {
			result.push_back(rule);
		}
	}
	return result;
}

bool MainControllerImpl::_ruleAffectsCurrentDefaultOs(Model_Rule* rule, std::string const& currentRulePath, std::string const& currentDefaultRulePath) {
	bool result = false;

	if (rule->type == Model_Rule::SUBMENU) {
		if (currentDefaultRulePath.substr(0, currentRulePath.length() + 1) == currentRulePath + ">") {
			result = true;
		}
	} else {
		if (this->settings->getValue("GRUB_DEFAULT") == currentRulePath) {
			result = true;
		}
	}
	return result;
}

void MainControllerImpl::_updateCurrentDefaultOs(Model_Rule* rule, std::string const& oldRulePath, std::string oldDefaultRulePath) {
	oldDefaultRulePath.replace(0, oldRulePath.length(), this->grublistCfg->getRulePath(*rule));
	this->settings->setValue("GRUB_DEFAULT", oldDefaultRulePath);
}

void MainControllerImpl::dieAction(){
	this->logActionBegin("die");
	try {
		this->is_loading = false;
		this->env->activeThreadCount = 0;
		bool showEnvSettings = false;
		if (this->thrownException){
			showEnvSettings = this->view->askForEnvironmentSettings(this->env->mkconfig_cmd, this->grublistCfg->getGrubErrorMessage());
		}
		if (showEnvSettings) {
			this->showEnvEditorAction();
		} else {
			this->exitAction(true); //exit
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::exitAction(bool force){
	this->logActionBegin("exit");
	try {
		if (force){
			this->view->setLockState(~0);
			if (this->mountTable->getEntryByMountpoint(PARTCHOOSER_MOUNTPOINT))
				this->mountTable->umountAll(PARTCHOOSER_MOUNTPOINT);
			this->getThreadController().stopApplication();
		}
		else {
			int dlgResponse = this->view->showExitConfirmDialog(this->config_has_been_different_on_startup_but_unsaved*2 + this->env->modificationsUnsaved);
			if (dlgResponse == 1){
				this->saveAction(); //starts a thread that delays the application exiting
			}

			if (dlgResponse != 0){
				if (this->env->activeThreadCount != 0){
					this->env->quit_requested = true;
					this->grublistCfg->cancelThreads();
				}
				else {
					this->exitAction(true); //re-run with force option
				}
			}
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::removeRulesAction(std::list<Rule*> rules, bool force){
	this->logActionBegin("remove-rules");
	try {
		if (!force && this->_listHasAllCurrentSystemRules(rules)) {
			this->view->showSystemRuleRemoveWarning();
		} else if (!force && this->_listHasPlaintextRules(rules)) {
			this->view->showPlaintextRemoveWarning();
		} else {
			std::list<Entry*> entriesOfRemovedRules;
			std::map<Model_Proxy*, Nothing> emptyProxies;
			for (std::list<Rule*>::iterator iter = rules.begin(); iter != rules.end(); iter++) {
				Model_Rule* rule = &Model_Rule::fromPtr(*iter);
				rule->setVisibility(false);
				entriesOfRemovedRules.push_back(rule->dataSource);
				if (!this->grublistCfg->proxies.getProxyByRule(rule)->hasVisibleRules()) {
					emptyProxies[this->grublistCfg->proxies.getProxyByRule(rule)] = Nothing();
				}
			}

			for (std::map<Model_Proxy*, Nothing>::iterator iter = emptyProxies.begin(); iter != emptyProxies.end(); iter++) {
				this->grublistCfg->proxies.deleteProxy(iter->first);
				this->log("proxy removed", Logger::INFO);
			}

			this->syncLoadStateAction();
			this->getAllControllers().trashController->selectEntriesAction(entriesOfRemovedRules);
			this->env->modificationsUnsaved = true;
			this->getAllControllers().settingsController->updateSettingsDataAction();
			this->getAllControllers().themeController->updateSettingsDataAction();
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void MainControllerImpl::renameRuleAction(Rule* entry, std::string const& newText){
	this->logActionBegin("rename-rule");
	try {
		Model_Rule* entry2 = &Model_Rule::fromPtr(entry);
		std::string oldName = entry2->outputName;
	//	std::string newName = this->view->getRuleName(entry);
		if (newText == ""){
			this->view->showErrorMessage(gettext("Name the Entry"));
			this->view->setRuleName(entry, oldName);
		}
		else {
			this->renameEntry(entry2, newText);
		}
		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void MainControllerImpl::moveAction(std::list<Rule*> rules, int direction){
	this->logActionBegin("move");
	try {
		bool stickyPlaceholders = !this->view->getOptions().at(VIEW_SHOW_PLACEHOLDERS);
		try {
			assert(direction == -1 || direction == 1);
			int distance = 1;
			if (stickyPlaceholders) {
				rules = this->_populateSelection(rules);
				rules = this->grublistCfg->getNormalizedRuleOrder(rules);
				distance = this->_countRulesUntilNextRealRule(&Model_Rule::fromPtr(direction == -1 ? rules.front() : rules.back()), direction);
			}

			std::list<Rule*> movedRules;

			for (int j = 0; j < distance; j++) { // move the range multiple times
				int ruleCount = rules.size();
				Model_Rule* rulePtr = &Model_Rule::fromPtr(direction == -1 ? rules.front() : rules.back());
				for (int i = 0; i < ruleCount; i++) { // move multiple rules
					std::string currentRulePath = this->grublistCfg->getRulePath(*rulePtr);
					std::string currentDefaultRulePath = this->settings->getValue("GRUB_DEFAULT");
					bool updateDefault = this->_ruleAffectsCurrentDefaultOs(rulePtr, currentRulePath, currentDefaultRulePath);

					rulePtr = &this->grublistCfg->moveRule(rulePtr, direction);

					if (updateDefault) {
						this->_updateCurrentDefaultOs(rulePtr, currentRulePath, currentDefaultRulePath);
					}

					if (i < ruleCount - 1) {
						bool isEndOfList = false;
						bool targetFound = false;
						try {
							rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
						} catch (NoMoveTargetException const& e) {
							isEndOfList = true;
							rulePtr = this->grublistCfg->proxies.getProxyByRule(rulePtr)->getParentRule(rulePtr);
						}
						if (!isEndOfList && rulePtr->type == Model_Rule::SUBMENU) {
							rulePtr = direction == -1 ? &rulePtr->subRules.front() : &rulePtr->subRules.back();
							if (rulePtr->isVisible) {
								targetFound = true;
							}
						}

						if (!targetFound) {
							rulePtr = &*this->grublistCfg->proxies.getNextVisibleRule(rulePtr, -direction);
						}
					}
				}

				movedRules.clear();
				movedRules.push_back(rulePtr);
				for (int i = 1; i < ruleCount; i++) {
					movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(&Model_Rule::fromPtr(movedRules.back()), direction));
				}
				movedRules = this->grublistCfg->getNormalizedRuleOrder(movedRules);

				rules = movedRules;
			}

			this->syncLoadStateAction();
			if (stickyPlaceholders) {
				movedRules = this->_removePlaceholdersFromSelection(movedRules);
			}
			this->view->selectRules(movedRules);
			this->env->modificationsUnsaved = true;
		} catch (NoMoveTargetException const& e) {
			this->view->showErrorMessage(gettext("cannot move this entry"));
			this->syncLoadStateAction();
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void MainControllerImpl::createSubmenuAction(std::list<Rule*> childItems) {
	this->logActionBegin("create-submenu");
	try {
		Model_Rule* firstRule = &Model_Rule::fromPtr(childItems.front());
		Model_Rule* newItem = this->grublistCfg->createSubmenu(firstRule);
		this->syncLoadStateAction();
		this->moveAction(childItems, -1);
		this->threadController->startEdit(newItem);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::removeSubmenuAction(std::list<Rule*> childItems) {
	this->logActionBegin("remove-submenu");
	try {
		Model_Rule* firstItem = this->grublistCfg->splitSubmenu(&Model_Rule::fromPtr(childItems.front()));
		std::list<Rule*> movedRules;
		movedRules.push_back(firstItem);
		for (int i = 1; i < childItems.size(); i++) {
			movedRules.push_back(&*this->grublistCfg->proxies.getNextVisibleRule(&Model_Rule::fromPtr(movedRules.back()), 1));
		}

		this->moveAction(movedRules, -1);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::revertAction() {
	this->logActionBegin("revert");
	try {
		this->grublistCfg->revert();
		this->syncLoadStateAction();
		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::showProxyInfo(Model_Proxy* proxy){
	this->view->setStatusText("");
}


void MainControllerImpl::syncLoadStateThreadedAction() {
	this->logActionBeginThreaded("sync-load-state-threaded");
	try {
		this->getThreadController().syncEntryList();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}
void MainControllerImpl::syncSaveStateThreadedAction() {
	this->logActionBeginThreaded("sync-save-state-threaded");
	try {
		this->getThreadController().updateSaveProgress();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorThreadedAction(e);
	}
	this->logActionEndThreaded();
}

void MainControllerImpl::syncSaveStateAction(){
	this->logActionBegin("sync-save-state");
	try {
		this->log("running MainControllerImpl::syncListView_save", Logger::INFO);
		this->view->progress_pulse();
		if (this->grublistCfg->getProgress() == 1){
			if (this->grublistCfg->error_proxy_not_found){
				this->view->showProxyNotFoundMessage();
				this->grublistCfg->error_proxy_not_found = false;
			}
			if (this->env->quit_requested)
				this->exitAction(true);

			this->view->setLockState(0);

			this->view->hideProgressBar();
			this->view->setStatusText(gettext("Configuration has been saved"));

			this->updateList();
		}
		else {
			this->view->setStatusText(gettext("updating configuration"));
		}
		this->log("MainControllerImpl::syncListView_save completed", Logger::INFO);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::syncLoadStateAction() {
	this->logActionBegin("sync-load-state");
	try {
		this->log("running MainControllerImpl::syncListView_load", Logger::INFO);
		this->view->setLockState(1|4);
		double progress = this->grublistCfg->getProgress();
		if (progress != 1) {
			this->view->setProgress(progress);
			this->view->setStatusText(this->grublistCfg->getProgress_name(), this->grublistCfg->getProgress_pos(), this->grublistCfg->getProgress_max());
		} else {
			if (this->env->quit_requested) {
				this->exitAction(true);
			}
			this->view->hideProgressBar();
			this->view->setStatusText("");
		}

		if (progress == 1 && this->grublistCfg->hasScriptUpdates()) {
			this->grublistCfg->applyScriptUpdates();
			this->env->modificationsUnsaved = true;
			this->view->showScriptUpdateInfo();
		}

		//if grubConfig is locked, it will be cancelled as early as possible
		if (this->grublistCfg->lock_if_free()) {
			this->updateList();
			this->grublistCfg->unlock();
		}

		if (progress == 1){
			this->getAllControllers().settingsController->updateSettingsDataAction();
			this->getAllControllers().themeController->updateSettingsDataAction();

			this->getAllControllers().trashController->updateAction(this->view->getOptions());

			bool placeholdersVisible = this->view->getOptions().at(VIEW_SHOW_PLACEHOLDERS);
			bool hiddenEntriesVisible = this->view->getOptions().at(VIEW_SHOW_HIDDEN_ENTRIES);
			this->view->setTrashPaneVisibility(
				this->grublistCfg->getRemovedEntries(NULL, !placeholdersVisible).size() >= 1 && !hiddenEntriesVisible
			);
			this->view->setLockState(0);
		}
		this->log("MainControllerImpl::syncListView_load completed", Logger::INFO);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::showSettingsAction() {
	this->logActionBegin("show-settings");
	try {
		this->getAllControllers().settingsController->showAction(env->burgMode);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::showEnvEditorAction(bool resetPartitionChooser) {
	this->logActionBegin("show-env-editor");
	try {
		if (this->env->modificationsUnsaved) {
			bool proceed = this->view->confirmUnsavedSwitch();
			if (!proceed) {
				return;
			}
		}

		this->view->hide();

		this->getAllControllers().envEditController->showAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::initModeAction(bool burgChosen) {
	this->logActionBegin("init-mode");
	try {
		this->init(burgChosen ? Model_Env::BURG_MODE : Model_Env::GRUB_MODE);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::addEntriesAction(std::list<Rule*> entries) {
	this->logActionBegin("add-entries");
	try {
		std::list<Rule*> addedRules;
		for (std::list<Rule*>::iterator iter = entries.begin(); iter != entries.end(); iter++) {
			Model_Entry* entry = Model_Rule::fromPtr(*iter).dataSource;
			assert(entry != NULL);
			addedRules.push_back(this->grublistCfg->addEntry(*entry, Model_Rule::fromPtr(*iter).type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER));
		}

		this->syncLoadStateAction();

		this->view->selectRules(addedRules);

		this->env->modificationsUnsaved = true;
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::activateSettingsAction() {
	this->logActionBegin("activate-settings");
	try {
		this->view->setLockState(1);
		this->getAllControllers().settingsController->syncAction();
		this->getAllControllers().themeController->loadThemesAction();
		this->getAllControllers().themeController->updateFontSizeAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void MainControllerImpl::showReloadRecommendationAction() {
	this->logActionBegin("show-reload-recommendation");
	try {
		this->view->showReloadRecommendation();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::selectRulesAction(std::list<Rule*> rules) {
	this->logActionBegin("select-rules");
	try {
		this->view->selectRules(rules);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::selectRuleAction(Rule* rule, bool startEdit) {
	this->logActionBegin("select-rule");
	try {
		this->view->selectRule(rule, startEdit);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::refreshTabAction(unsigned int pos) {
	this->logActionBegin("refresh-tab");
	try {
		if (pos != 0) { // list
			this->getAllControllers().settingsController->syncAction();
			this->getAllControllers().themeController->syncAction();
		}
		this->view->updateLockState();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::setViewOptionAction(ViewOption option, bool value) {
	this->logActionBegin("set-view-option");
	try {
		this->view->setOption(option, value);
		try {
			this->env->saveViewOptions(this->view->getOptions());
		} catch (FileSaveException e) {
			this->log("option saving failed", Logger::ERROR);
		}
		this->syncLoadStateAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void MainControllerImpl::entryStateToggledAction(Rule* entry, bool state) {
	this->logActionBegin("entry-state-toggled");
	try {
		Model_Rule::fromPtr(entry).setVisibility(state);
		this->syncLoadStateAction();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void MainControllerImpl::updateSelectionAction(std::list<Rule*> selectedRules) {
	this->logActionBegin("update-selection");
	try {
		if (selectedRules.size()) {
			this->getAllControllers().trashController->selectEntriesAction(std::list<Entry*>());
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}
