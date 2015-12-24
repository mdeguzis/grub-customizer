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

#include "TrashControllerImpl.h"

TrashControllerImpl::TrashControllerImpl()
	: ControllerAbstract("trash")
{
}

void TrashControllerImpl::_refresh() {
	assert(this->contentParserFactory != NULL);
	assert(this->deviceDataList != NULL);

	this->view->clear();

	this->data = this->grublistCfg->getRemovedEntries();

	this->_refreshView(NULL);
}

void TrashControllerImpl::_refreshView(Model_Rule* parent) {
	std::list<Model_Rule>& list = parent ? parent->subRules : this->data;
	for (std::list<Model_Rule>::iterator iter = list.begin(); iter != list.end(); iter++) {
		Model_Script* script = iter->dataSource ? this->grublistCfg->repository.getScriptByEntry(*iter->dataSource) : NULL;

		std::string name = iter->outputName;
		if (iter->dataSource && script) {
			name = this->entryNameMapper->map(iter->dataSource, name, iter->type != Model_Rule::SUBMENU);
		}

		View_Model_ListItem<Rule, Script> listItem;
		listItem.name = name;
		listItem.entryPtr = &*iter;
		listItem.scriptPtr = NULL;
		listItem.is_placeholder = iter->type == Model_Rule::OTHER_ENTRIES_PLACEHOLDER || iter->type == Model_Rule::PLAINTEXT;
		listItem.is_submenu = iter->type == Model_Rule::SUBMENU;
		listItem.scriptName = script ? script->name : "";
		listItem.isVisible = true;
		listItem.parentEntry = parent;
		if (iter->dataSource) {
			listItem.options = Controller_Helper_DeviceInfo::fetch(iter->dataSource->content, *this->contentParserFactory, *this->deviceDataList);
		}

		this->view->addItem(listItem);

		if (iter->subRules.size()) {
			this->_refreshView(&*iter);
		}
	}
}

// returns true, all selected entries are deletable
bool TrashControllerImpl::_isDeletable(std::list<Rule*> const& selectedEntries) {
	if (selectedEntries.size() == 0) {
		return false;
	}

	for (std::list<Rule*>::const_iterator iter = selectedEntries.begin(); iter != selectedEntries.end(); iter++) {
		if (Model_Rule::fromPtr(*iter).type != Model_Rule::NORMAL || Model_Rule::fromPtr(*iter).dataSource == NULL) {
			return false;
		}
		Model_Script* script = this->grublistCfg->repository.getScriptByEntry(*Model_Rule::fromPtr(*iter).dataSource);
		assert(script != NULL);
		if (!script->isCustomScript) {
			return false;
		}
	}

	return true;
}

void TrashControllerImpl::updateAction(std::map<ViewOption, bool> const& viewOptions){
	this->logActionBegin("update");
	try {
		this->view->setOptions(viewOptions);
		this->_refresh();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::applyAction(){
	this->logActionBegin("apply");
	try {
		std::list<Rule*> entries = view->getSelectedEntries();
		this->getAllControllers().mainController->addEntriesAction(entries);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}


void TrashControllerImpl::hideAction() {
	this->logActionBegin("hide");
	try {
		this->view->hide();
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::deleteCustomEntriesAction() {
	this->logActionBegin("delete-custom-entries");
	try {
		std::list<Rule*> deletableEntries = this->view->getSelectedEntries();
		for (std::list<Rule*>::iterator iter = deletableEntries.begin(); iter != deletableEntries.end(); iter++) {
			assert(Model_Rule::fromPtr(*iter).dataSource != NULL);
			this->grublistCfg->deleteEntry(*Model_Rule::fromPtr(*iter).dataSource);
		}
		this->_refresh();
		this->updateSelectionAction(std::list<Rule*>());
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::selectEntriesAction(std::list<Entry*> const& entries) {
	this->logActionBegin("select-entries");
	try {
		// first look for rules in local data, linking to the the given entries
		std::list<Rule*> rules;
		for (std::list<Entry*>::const_iterator entryIter = entries.begin(); entryIter != entries.end(); entryIter++) {
			for (std::list<Model_Rule>::iterator ruleIter = this->data.begin(); ruleIter != this->data.end(); ruleIter++) {
				if (*entryIter == ruleIter->dataSource) {
					rules.push_back(&*ruleIter);
				}
			}
		}
		this->view->selectEntries(rules);
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}

void TrashControllerImpl::updateSelectionAction(std::list<Rule*> const& selectedEntries) {
	this->logActionBegin("update-selection");
	try {
		if (selectedEntries.size()) {
			this->getAllControllers().mainController->selectRulesAction(std::list<Rule*>());
			this->view->setRestoreButtonSensitivity(true);
			this->view->setDeleteButtonVisibility(this->_isDeletable(selectedEntries));
		} else {
			this->view->setRestoreButtonSensitivity(false);
			this->view->setDeleteButtonVisibility(false);
		}
	} catch (Exception const& e) {
		this->getAllControllers().errorController->errorAction(e);
	}
	this->logActionEnd();
}
