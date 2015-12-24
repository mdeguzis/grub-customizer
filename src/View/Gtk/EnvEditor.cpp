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

#include "EnvEditor.h"

View_Gtk_EnvEditor::View_Gtk_EnvEditor()
	: pChooser(NULL), lblPartition(gettext("_Partition:"), true),
	  lblType(gettext("_Type:"), true), eventLock(true), lblSubmountpoints(gettext("Submountpoints:")),
	  cbSaveConfig(gettext("save this configuration"))
{
	this->set_title("Grub Customizer environment setup");
	this->set_icon_name("grub-customizer");

	Gtk::Box& box = *this->get_vbox();
	box.add(this->vbContent);
	this->vbContent.add(this->tblLayout);
	this->vbContent.add(this->bbxSaveConfig);

	this->bbxSaveConfig.pack_start(this->cbSaveConfig);

	this->tblLayout.attach(this->lblPartition, 0, 1, 0, 1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);

	this->tblLayout.attach(this->lblSubmountpoints, 0, 1, 1, 2);
	this->tblLayout.attach(this->scrSubmountpoints, 1, 2, 1, 2);

	this->tblLayout.attach(this->lblType, 0, 1, 2, 3, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
	this->tblLayout.attach(this->cbType, 1, 2, 2, 3, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

	this->tblLayout.attach(this->separator, 0, 2, 3, 4, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);

	this->scrSubmountpoints.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	this->scrSubmountpoints.add(this->vbSubmountpoints);
	this->scrSubmountpoints.set_size_request(-1, 50);

	this->lblSubmountpoints.set_no_show_all(true);
	this->scrSubmountpoints.set_no_show_all(true);

	this->cbType.append(gettext("Grub 2"));
	this->cbType.append(gettext("BURG"));
	this->cbType.set_active(0);
	this->cbType.signal_changed().connect(sigc::mem_fun(this, &View_Gtk_EnvEditor::signal_bootloaderType_changed));

	lblPartition.set_alignment(Pango::ALIGN_RIGHT);
	lblType.set_alignment(Pango::ALIGN_RIGHT);
	lblSubmountpoints.set_alignment(Pango::ALIGN_RIGHT);

	this->tblLayout.set_spacings(10);
	this->tblLayout.set_border_width(10);

	this->add_button(Gtk::Stock::QUIT, Gtk::RESPONSE_CLOSE);
	this->add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);

	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_EnvEditor::signal_response_action));

	this->eventLock = false;
}

View_Gtk_EnvEditor::~View_Gtk_EnvEditor() {
	if (this->pChooser) {
		this->tblLayout.remove(*this->pChooser);
		delete this->pChooser;
		this->pChooser = NULL;
	}
}

void View_Gtk_EnvEditor::setRootDeviceName(std::string const& rootDeviceName) {
	this->rootDeviceName = rootDeviceName;
}

void View_Gtk_EnvEditor::setEnvSettings(std::map<std::string, std::string> const& props, std::list<std::string> const& requiredProps, std::list<std::string> const& validProps) {
	this->eventLock = true;
	int pos = 4;
	for (std::map<std::string, std::string>::const_iterator iter = props.begin(); iter != props.end(); iter++) {
		Gtk::Label* label = NULL;
		if (this->labelMap.find(iter->first) == this->labelMap.end()) {
			label = Gtk::manage(new Gtk::Label(iter->first + ":"));
			label->set_alignment(Pango::ALIGN_RIGHT);
			this->tblLayout.attach(*label, 0, 1, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
			this->labelMap[iter->first] = label;
		} else {
			label = this->labelMap[iter->first];
		}

		Gtk::Entry* entry = NULL;
		bool entryCreated = false;
		if (this->optionMap.find(iter->first) == this->optionMap.end()) {
			entry = Gtk::manage(new Gtk::Entry());
			entryCreated = true;
			this->tblLayout.attach(*entry, 1, 2, pos, pos+1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
			label->set_mnemonic_widget(*entry);
			entry->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_EnvEditor::signal_optionModified));
			this->optionMap[iter->first] = entry;
		} else {
			entry = this->optionMap[iter->first];
		}

		if (entry->get_text() != iter->second) {
			entry->set_text(iter->second);
		}

		Gtk::Image* img = NULL;
		if (this->imageMap.find(iter->first) == this->imageMap.end()) {
			img = Gtk::manage(new Gtk::Image());
			this->tblLayout.attach(*img, 2, 3, pos, pos+1, Gtk::SHRINK | Gtk::FILL, Gtk::SHRINK);
			this->imageMap[iter->first] = img;
		} else {
			img = this->imageMap[iter->first];
		}

		Glib::RefPtr<Gdk::Pixbuf> icon;
		if (std::find(validProps.begin(), validProps.end(), iter->first) != validProps.end()) {
			icon = this->render_icon_pixbuf(Gtk::Stock::OK, Gtk::ICON_SIZE_BUTTON);
		} else if (std::find(requiredProps.begin(), requiredProps.end(), iter->first) != requiredProps.end()) {
			icon = this->render_icon_pixbuf(Gtk::Stock::DIALOG_ERROR, Gtk::ICON_SIZE_BUTTON);
		} else {
			icon = this->render_icon_pixbuf(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_BUTTON);
		}

		img->set(icon);

		if (this->get_visible()) {
			this->tblLayout.show_all();
		}
		pos++;
	}
	this->eventLock = false;
}

std::map<std::string, std::string> View_Gtk_EnvEditor::getEnvSettings() {
	std::map<std::string, std::string> result;
	for (std::map<std::string, Gtk::Entry*>::iterator iter = this->optionMap.begin(); iter != this->optionMap.end(); iter++) {
		result[iter->first] = iter->second->get_text();
	}
	return result;
}

int View_Gtk_EnvEditor::getBootloaderType() const {
	return this->cbType.get_active_row_number();
}

void View_Gtk_EnvEditor::show(bool resetPartitionChooser) {
	this->eventLock = true;
	if (this->pChooser != NULL) {
		this->tblLayout.remove(*pChooser);
	}

	if (!this->pChooser) {
		this->pChooser = new View_Gtk_Element_PartitionChooser("", *this->deviceDataList, true, this->rootDeviceName);
		this->pChooser->signal_changed().connect(sigc::mem_fun(this, &View_Gtk_EnvEditor::signal_partitionChanged));
	}
	if (resetPartitionChooser) {
		this->pChooser->set_active(0);
	}

	this->tblLayout.attach(*pChooser, 1, 2, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
	this->lblPartition.set_mnemonic_widget(*pChooser);

	this->show_all();

	this->eventLock = false;
}
void View_Gtk_EnvEditor::hide() {
	Gtk::Dialog::hide();
}

void View_Gtk_EnvEditor::removeAllSubmountpoints() {
	for (std::map<std::string, Gtk::CheckButton*>::iterator iter = this->subMountpoints.begin(); iter != this->subMountpoints.end(); iter++) {
		this->vbSubmountpoints.remove(*iter->second);
		delete iter->second;
	}
	this->subMountpoints.clear();

	this->scrSubmountpoints.hide();
	this->lblSubmountpoints.hide();
}

void View_Gtk_EnvEditor::addSubmountpoint(std::string const& name, bool isActive) {
	Gtk::CheckButton* cb = new Gtk::CheckButton(name);
	cb->set_active(isActive);
	cb->signal_toggled().connect(sigc::bind<Gtk::CheckButton&>(sigc::mem_fun(this, &View_Gtk_EnvEditor::signal_submountpointToggled), *cb));
	this->vbSubmountpoints.pack_start(*cb, Gtk::PACK_SHRINK);
	this->subMountpoints[name] = cb;

	this->scrSubmountpoints.show();
	vbSubmountpoints.show_all();
	this->lblSubmountpoints.show();
}

void View_Gtk_EnvEditor::setSubmountpointSelectionState(std::string const& submountpoint, bool new_isSelected) {
	this->subMountpoints[submountpoint]->set_active(new_isSelected);
}

void View_Gtk_EnvEditor::showErrorMessage(MountExceptionType type){
	switch (type){
		case MOUNT_FAILED:       Gtk::MessageDialog(gettext("Mount failed!")).run(); break;
		case UMOUNT_FAILED:      Gtk::MessageDialog(gettext("umount failed!")).run(); break;
		case MOUNT_ERR_NO_FSTAB: Gtk::MessageDialog(gettext("This seems not to be a root file system (no fstab found)")).run(); break;
		case SUB_MOUNT_FAILED:   Gtk::MessageDialog(gettext("Couldn't mount the selected partition")).run(); break;
		case SUB_UMOUNT_FAILED:  Gtk::MessageDialog(gettext("Couldn't umount the selected partition")).run(); break;
	}
}

Gtk::Widget& View_Gtk_EnvEditor::getContentBox() {
	this->get_vbox()->remove(this->vbContent);
	return this->vbContent;
}

void View_Gtk_EnvEditor::signal_partitionChanged() {
	if (!this->eventLock) {
		std::string selectedUuid = this->pChooser->getSelectedUuid();
		if (selectedUuid != "") {
			selectedUuid = "UUID=" + selectedUuid;
		}
		this->controller->switchPartitionAction(selectedUuid);
	}
}

void View_Gtk_EnvEditor::signal_bootloaderType_changed() {
	if (!this->eventLock) {
		this->controller->switchBootloaderTypeAction(this->cbType.get_active_row_number());
	}
}

void View_Gtk_EnvEditor::signal_optionModified() {
	if (!this->eventLock) {
		this->controller->updateGrubEnvOptionsAction();
	}
}

void View_Gtk_EnvEditor::signal_response_action(int response_id) {
	if (response_id == Gtk::RESPONSE_CLOSE || response_id == Gtk::RESPONSE_DELETE_EVENT) {
		this->controller->exitAction();
	} else if (response_id == Gtk::RESPONSE_APPLY) {
		this->controller->applyAction(this->cbSaveConfig.get_active());
	}
}

void View_Gtk_EnvEditor::signal_submountpointToggled(Gtk::CheckButton& sender) {
	if (!eventLock) {
		if (sender.get_active()) {
			this->controller->mountSubmountpointAction(sender.get_label());
		} else {
			this->controller->umountSubmountpointAction(sender.get_label());
		}
	}
}
