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

#ifndef PARTITIONCHOOSER_DROPDOWN_H_
#define PARTITIONCHOOSER_DROPDOWN_H_
#include <gtkmm.h>
#include <string>
#include "../../../Model/DeviceDataListInterface.h"
#include <libintl.h>

class View_Gtk_Element_PartitionChooser : public Gtk::ComboBoxText {
	std::map<std::string, std::string> uuid_map;
	Glib::ustring activePartition_uuid;
	Model_DeviceDataListInterface const* deviceDataList;
	bool prependCurrentPartition;
	std::string currentPartitionName;
public:
	View_Gtk_Element_PartitionChooser(Glib::ustring const& activePartition_uuid, Model_DeviceDataListInterface const& deviceDataList, bool prependCurrentPartition = false, std::string const& currentPartitionName = "");
	void load();
	std::string getSelectedUuid() const;
	static std::string strToLower(std::string str);
};

#endif
