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

#ifndef ENTRYEDITCONTROLLER_H_
#define ENTRYEDITCONTROLLER_H_

#include <string>
#include "../lib/Type.h"

class EntryEditController {
public:
	virtual inline ~EntryEditController() {};
	virtual void showAction(Rule* rule) = 0;
	virtual void showCreatorAction() = 0;
	virtual void applyAction() = 0;
	virtual void syncOptionsAction() = 0;
	virtual void syncSourceAction() = 0;
	virtual void switchTypeAction(std::string const& newType) = 0;
};


#endif /* ENTRYEDITCONTROLLER_H_ */
