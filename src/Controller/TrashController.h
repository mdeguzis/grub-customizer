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

#ifndef TRASHCONTROLLER_H_
#define TRASHCONTROLLER_H_
#include "../lib/Type.h"
#include <map>

class TrashController {
public:
	virtual inline ~TrashController(){}
	virtual void applyAction() = 0;
	virtual void updateAction(std::map<ViewOption, bool> const& viewOptions) = 0;
	virtual void hideAction() = 0;
	virtual void deleteCustomEntriesAction() = 0;
	virtual void selectEntriesAction(std::list<Entry*> const& entries) = 0;
	virtual void updateSelectionAction(std::list<Rule*> const& selectedEntries) = 0;
};


#endif /* TRASHCONTROLLER_H_ */
