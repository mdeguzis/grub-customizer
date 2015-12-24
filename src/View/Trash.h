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

#ifndef ENTRYADDDLG_H_
#define ENTRYADDDLG_H_

#include <list>
#include <string>
#include "../lib/Type.h"
#include "Model/ListItem.h"

/**
 * Interface to be implemented by dialogs which lets the user adding scripts
 */
class View_Trash {
public:
	virtual inline ~View_Trash() {};

	//clear the script menu
	virtual void clear()=0;
	//gets the index of the selected script item
	virtual std::list<Rule*> getSelectedEntries()=0;
	//adds a new item
	virtual void addItem(View_Model_ListItem<Rule, Script> const& listItem)=0;
	//whether to active the delete button
	virtual void setDeleteButtonEnabled(bool val) = 0;
	//show this dialog
	virtual void show()=0;
	//thise this dialog
	virtual void hide()=0;

	virtual void askForDeletion(std::list<std::string> const& names) = 0;

	virtual void setOptions(std::map<ViewOption, bool> const& viewOptions) = 0;

	virtual void selectEntries(std::list<Rule*> const& entries) = 0;

	virtual void setRestoreButtonSensitivity(bool sensitivity) = 0;

	virtual void setDeleteButtonVisibility(bool visibility) = 0;
};

#endif
