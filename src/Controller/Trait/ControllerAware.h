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

#ifndef TRAIT_CONTROLLERAWARE_H_
#define TRAIT_CONTROLLERAWARE_H_

template <typename T>
class Trait_ControllerAware {
protected:
	T* controller;
public:
	Trait_ControllerAware() : controller(NULL) {}
	void setController(T& controller) {
		this->controller = &controller;
	}
};

#endif /* TRAIT_CONTROLLERAWARE_H_ */
