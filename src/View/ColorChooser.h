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

#ifndef COLOR_CHOOSER_INCLUDED
#define COLOR_CHOOSER_INCLUDED
#include <string>
/**
 * base class to be implemented by color chooser controls
 */
class View_ColorChooser {
public:
	virtual inline ~View_ColorChooser() {};

	//choose one of the colors provided by this control
	virtual void selectColor(std::string const& codeName)=0;

	//determine which color is currently selected
	virtual std::string getSelectedColor() const=0;
};

#endif
