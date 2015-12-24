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

#ifndef FB_RESOLUTIONS_GETTER
#define FB_RESOLUTIONS_GETTER
#include <string>
#include <list>
#include <cstdio>
#include "../Controller/SettingsController.h"
#include "../Controller/Trait/ControllerAware.h"
#include "../lib/Trait/LoggerAware.h"

class Model_FbResolutionsGetter : public Trait_LoggerAware, public Trait_ControllerAware<SettingsController> {
	std::list<std::string> data;
	bool _isLoading;
public:
	Model_FbResolutionsGetter();
	const std::list<std::string>& getData() const;
	void load();
};

class Model_FbResolutionsGetter_Connection {
protected:
	Model_FbResolutionsGetter* fbResolutionsGetter;
public:
	Model_FbResolutionsGetter_Connection() : fbResolutionsGetter(NULL) {}

	void setFbResolutionsGetter(Model_FbResolutionsGetter& fbResolutionsGetter){
		this->fbResolutionsGetter = &fbResolutionsGetter;
	}
};

#endif
