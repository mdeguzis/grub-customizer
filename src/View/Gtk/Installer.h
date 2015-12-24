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

#ifndef GRUB_INSTALL_DLG_GTK_INCLUDED
#define GRUB_INSTALL_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../../Controller/InstallerController.h"
#include "../../Controller/Trait/ControllerAware.h"
#include <libintl.h>
#include "../Installer.h"
#include "../../lib/Trait/LoggerAware.h"

class View_Gtk_Installer :
	public Gtk::Dialog,
	public View_Installer,
	public Trait_LoggerAware,
	public Trait_ControllerAware<InstallerController>
{
	Gtk::Label lblDescription;
	Gtk::HBox hbDevice;
	Gtk::Label lblDevice, lblInstallInfo;
	Gtk::Entry txtDevice;
	Glib::Dispatcher disp_grub_install_ready;
	void func_disp_grub_install_ready();
	void signal_grub_install_dialog_response(int response_id);
	Glib::ustring install_result;
	public:
	View_Gtk_Installer();
	void show();
	void showMessageGrubInstallCompleted(std::string const& msg);
};
#endif
