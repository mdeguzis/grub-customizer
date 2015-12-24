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

#ifndef SETTINGSDLG_H_
#define SETTINGSDLG_H_

/**
 * Interface to be implemented by settings dialogs
 */
class View_Settings {
public:
	virtual inline ~View_Settings() {};

	enum DefEntryType {
		DEF_ENTRY_PREDEFINED,
		DEF_ENTRY_SAVED
	};
	struct CustomOption {
		std::string name, old_name, value;
		bool isActive;
	};
	//show this dialog
	virtual void show(bool burgMode)=0;
	//hide this dialog
	virtual void hide()=0;
	//reads the selection row from the custom options list
	virtual std::string getSelectedCustomOption()=0;
	//adds an entry to the end of the default entry chooser
	virtual void addEntryToDefaultEntryChooser(std::string const& labelPathValue, std::string const& labelPathLabel, std::string const& numericPathValue, std::string const& numericPathLabel)=0;
	//removes all items from the default entry chooser
	virtual void clearDefaultEntryChooser()=0;
	//removes all item from the resolution chooser
	virtual void clearResolutionChooser()=0;
	//adds an item to the end of the resolution chooser
	virtual void addResolution(std::string const& resolution)=0;
	//gets the name of the default menu entry
	virtual std::string getSelectedDefaultGrubValue()=0;
	//adds an option the the generic setting list
	virtual void addCustomOption(bool isActive, std::string const& name, std::string const& value)=0;
	//select the specified custom option entry
	virtual void selectCustomOption(std::string const& name)=0;
	//removes all generic setting rows
	virtual void removeAllSettingRows()=0;
	//reads the given generic option
	virtual CustomOption getCustomOption(std::string const& name)=0;
	//sets which type of default entry to use
	virtual void setActiveDefEntryOption(DefEntryType option)=0;
	//determines which type of default entry should be used
	virtual DefEntryType getActiveDefEntryOption()=0;
	//sets the default entry
	virtual void setDefEntry(std::string const& defEntry)=0;
	//sets whether the show menu checkbox should be active or not
	virtual void setShowMenuCheckboxState(bool isActive)=0;
	//determines whether the show menu checkbox is active or not
	virtual bool getShowMenuCheckboxState()=0;
	//sets whether the os-prober checkbox should be active or not
	virtual void setOsProberCheckboxState(bool isActive)=0;
	//sets whether the os-prober checkbox is active or not
	virtual bool getOsProberCheckboxState()=0;
	//shows the information about a conflict between os-prober and hidden menus
	virtual void showHiddenMenuOsProberConflictMessage()=0;
	//sets the grub menu timeout
	virtual void setTimeoutValue(int value)=0;
	//sets weather the grub menu timeout is active
	virtual void setTimeoutActive(bool active)=0;
	//reads the grub menu timeout
	virtual int getTimeoutValue()=0;
	//reads the grub menu timeout as string
	virtual std::string getTimeoutValueString()=0;
	//says wheather the timeout checkbox is activates
	virtual bool getTimeoutActive()=0;
	//sets kernel params
	virtual void setKernelParams(std::string const& params)=0;
	//reads kernel params
	virtual std::string getKernelParams()=0;
	//sets whether the recovery checkbox should be active or not
	virtual void setRecoveryCheckboxState(bool isActive)=0;
	//determines whether the recovery checkbox is active or not
	virtual bool getRecoveryCheckboxState()=0;
	//sets whether the resolution should be active or not
	virtual void setResolutionCheckboxState(bool isActive)=0;
	//determines whether the resolution is active or not
	virtual bool getResolutionCheckboxState()=0;
	//sets the selected resolution
	virtual void setResolution(std::string const& resolution)=0;
	//reads the selected resolution
	virtual std::string getResolution()=0;
};

#endif
