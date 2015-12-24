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

#ifndef GRUBENVEDITOR_H_
#define GRUBENVEDITOR_H_
#include <map>

class View_EnvEditor {
public:
	virtual inline ~View_EnvEditor() {};

	enum MountExceptionType {
		MOUNT_FAILED,
		UMOUNT_FAILED,
		MOUNT_ERR_NO_FSTAB,
		SUB_MOUNT_FAILED,
		SUB_UMOUNT_FAILED
	};
	virtual void show(bool resetPartitionChooser = false) = 0;
	virtual void hide() = 0;
	virtual void removeAllSubmountpoints() = 0;
	virtual void addSubmountpoint(std::string const& name, bool isActive) = 0;
	virtual void setEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) = 0;
	virtual std::map<std::string, std::string> getEnvSettings() = 0;
	virtual void setRootDeviceName(std::string const& rootDeviceName) = 0;
	virtual int getBootloaderType() const = 0;
	virtual void setSubmountpointSelectionState(std::string const& submountpoint, bool new_isSelected) = 0;
	virtual void showErrorMessage(MountExceptionType type)=0;
};

#endif /* GRUBENVEDITOR_H_ */
