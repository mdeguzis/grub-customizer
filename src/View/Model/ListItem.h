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

#ifndef MODEL_VIEW_LISTITEM_H_INCLUDED
#define MODEL_VIEW_LISTITEM_H_INCLUDED
#include <string>
#include <map>

template <typename TItem, typename TWrapper>
struct View_Model_ListItem {
	std::string name;
	TItem* entryPtr;
	TWrapper* scriptPtr;
	bool is_placeholder;
	bool is_submenu;
	std::string scriptName;
	std::string defaultName;
	bool isEditable;
	bool isModified;
	std::map<std::string, std::string> options;
	bool isVisible;
	TItem* parentEntry;
	TWrapper* parentScript;

	View_Model_ListItem() :
		entryPtr(NULL),
		scriptPtr(NULL),
		parentEntry(NULL),
		parentScript(NULL),
		is_placeholder(false),
		is_submenu(false),
		isEditable(false),
		isModified(false),
		isVisible(false)
	{}
};


#endif /* MODEL_VIEW_LISTITEM_H_INCLUDED */
