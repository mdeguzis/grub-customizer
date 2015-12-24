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

#ifndef ENVEDITORCONTROLLER_H_
#define ENVEDITORCONTROLLER_H_

class EnvEditorController {
public:
	virtual inline ~EnvEditorController(){};

	virtual void showAction(bool resetPartitionChooser = false) = 0;
	virtual void mountSubmountpointAction(std::string const& submountpoint) = 0;
	virtual void umountSubmountpointAction(std::string const& submountpoint) = 0;
	virtual void switchPartitionAction(std::string const& newPartition) = 0;
	virtual void switchBootloaderTypeAction(int newTypeIndex) = 0;
	virtual void updateGrubEnvOptionsAction() = 0;
	virtual void applyAction(bool saveConfig) = 0;

	virtual void exitAction() = 0;
};


#endif /* ENVEDITORCONTROLLER_H_ */
