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

#include "../Model/Env.h"
#include "../View/Gtk/Main.h"
#include "../View/Gtk/About.h"
#include "../View/Gtk/Installer.h"
#include "../View/Gtk/Trash.h"
#include "../View/Gtk/EntryEditor.h"
#include "../View/Gtk/Settings.h"
#include "../View/Gtk/EnvEditor.h"
#include "../View/Gtk/Error.h"
#include "../View/Gtk/Theme.h"
#include "../lib/Mutex/GLib.h"
#include "../Controller/GLib/ThreadController.h"
#include "../lib/Logger/Stream.h"
#include <iostream>
#include "../lib/ContentParser/FactoryImpl.h"
#include "../lib/ContentParser/Linux.h"
#include "../lib/ContentParser/LinuxIso.h"
#include "../lib/ContentParser/Chainloader.h"
#include "../lib/ContentParser/Memtest.h"
#include "../Controller/EntryEditControllerImpl.h"
#include "../Controller/MainControllerImpl.h"
#include "../Controller/SettingsControllerImpl.h"
#include "../Controller/EnvEditorControllerImpl.h"
#include "../Controller/TrashControllerImpl.h"
#include "../Controller/InstallerControllerImpl.h"
#include "../Controller/AboutControllerImpl.h"
#include "../Controller/ControllerCollection.h"
#include "../Controller/ErrorControllerImpl.h"
#include "../Controller/ThemeControllerImpl.h"
#include "../Mapper/EntryNameImpl.h"
#include "../lib/assert.h"
#include "../lib/ArrayStructure.h"
#include "../Model/ThemeManager.h"


int main(int argc, char** argv){
	if (getuid() != 0 && (argc == 1 || argv[1] != std::string("no-fork"))) {
		return system((std::string("pkexec ") + argv[0] + (argc == 2 ? std::string(" ") + argv[1] : "") + " no-fork").c_str());
	}
	setlocale( LC_ALL, "");
	bindtextdomain( "grub-customizer", LOCALEDIR);
	textdomain( "grub-customizer" );

	Logger_Stream logger(std::cout);

	try {
		Gtk::Main app(argc, argv);

		Glib::thread_init();

		Model_Env env;

		Model_ListCfg listcfg;
		View_Gtk_Main listCfgView;
		Model_SettingsManagerData settings;
		Model_SettingsManagerData settingsOnDisk;
		Model_Installer installer;
		View_Gtk_Installer installDlg;
		View_Gtk_Trash trashView;
		View_Gtk_EntryEditor entryEditDlg;
		Model_MountTable mountTable;
		Model_ListCfg savedListCfg;
		Model_FbResolutionsGetter fbResolutionsGetter;
		View_Gtk_Settings settingsDlg;
		Model_DeviceDataList deviceDataList;
		View_Gtk_About aboutDialog;
		Mutex_GLib listCfgMutex1;
		Mutex_GLib listCfgMutex2;
		ContentParser_FactoryImpl contentParserFactory;
		View_Gtk_EnvEditor envEditor;
		View_Gtk_Error errorView;
		Mapper_EntryNameImpl entryNameMapper;
		View_Gtk_Theme themeEditor;
		Model_ThemeManager themeManager;
		Model_DeviceMap deviceMap;

		entryNameMapper.setView(listCfgView);

		entryEditDlg.setDeviceDataList(deviceDataList);
		envEditor.setDeviceDataList(deviceDataList);

		EntryEditControllerImpl entryEditController;
		entryEditController.setContentParserFactory(contentParserFactory);
		entryEditController.setView(entryEditDlg);
		entryEditController.setDeviceDataList(deviceDataList);
		entryEditController.setListCfg(listcfg);

		MainControllerImpl mainController;
		mainController.setListCfg(listcfg);
		mainController.setSettingsManager(settings);
		mainController.setSettingsBuffer(settingsOnDisk);
		mainController.setSavedListCfg(savedListCfg);
		mainController.setFbResolutionsGetter(fbResolutionsGetter);
		mainController.setDeviceDataList(deviceDataList);
		mainController.setMountTable(mountTable);
		mainController.setContentParserFactory(contentParserFactory);
		mainController.setView(listCfgView);
		mainController.setEntryNameMapper(entryNameMapper);

		SettingsControllerImpl settingsController;
		settingsController.setListCfg(listcfg);
		settingsController.setView(settingsDlg);
		settingsController.setSettingsManager(settings);
		settingsController.setFbResolutionsGetter(fbResolutionsGetter);

		EnvEditorControllerImpl envEditController;
		envEditController.setMountTable(mountTable);
		envEditController.setView(envEditor);
		envEditController.setDeviceMap(deviceMap);

		TrashControllerImpl trashController;
		trashController.setEntryNameMapper(entryNameMapper);
		trashController.setListCfg(listcfg);
		trashController.setDeviceDataList(deviceDataList);
		trashController.setContentParserFactory(contentParserFactory);
		trashController.setView(trashView);

		InstallerControllerImpl installController;
		installController.setInstaller(installer);
		installController.setView(installDlg);

		AboutControllerImpl aboutController;
		aboutController.setView(aboutDialog);

		ErrorControllerImpl errorController;
		errorController.setView(errorView);

		ThemeControllerImpl themeController;
		themeController.setView(themeEditor);
		themeController.setThemeManager(themeManager);
		themeController.setSettingsManager(settings);
		themeController.setListCfg(listcfg);

		ControllerCollection controllerCollection;
		controllerCollection.entryEditController = &entryEditController;
		controllerCollection.mainController = &mainController;
		controllerCollection.settingsController = &settingsController;
		controllerCollection.envEditController = &envEditController;
		controllerCollection.trashController = &trashController;
		controllerCollection.installerController = &installController;
		controllerCollection.aboutController = &aboutController;
		controllerCollection.errorController = &errorController;
		controllerCollection.themeController = &themeController;

		entryEditController.setControllerCollection(controllerCollection);
		mainController.setControllerCollection(controllerCollection);
		settingsController.setControllerCollection(controllerCollection);
		envEditController.setControllerCollection(controllerCollection);
		trashController.setControllerCollection(controllerCollection);
		installController.setControllerCollection(controllerCollection);
		aboutController.setControllerCollection(controllerCollection);
		errorController.setControllerCollection(controllerCollection);
		themeController.setControllerCollection(controllerCollection);

		GLib_ThreadController threadC(controllerCollection);
		mainController.setThreadController(threadC);
		settingsController.setThreadController(threadC);
		installController.setThreadController(threadC);
		errorController.setThreadController(threadC);
		entryEditController.setThreadController(threadC);
		themeController.setThreadController(threadC);

		listCfgView.putSettingsDialog(settingsDlg.getCommonSettingsPane(), settingsDlg.getAppearanceSettingsPane());
		listCfgView.putTrashList(trashView.getList());
		settingsDlg.putThemeSelector(themeEditor.getThemeSelector());
		settingsDlg.putThemeEditArea(themeEditor.getEditorBox());

		//assign event listener
		listCfgView.setController(mainController);
		installDlg.setController(installController);
		trashView.setController(trashController);
		entryEditDlg.setController(entryEditController);
		settingsDlg.setController(settingsController);
		listcfg.setController(mainController);
		installer.setController(installController);
		fbResolutionsGetter.setController(settingsController);
		envEditor.setController(envEditController);
		errorView.setController(errorController);
		themeEditor.setController(themeController);

		//assign logger
		listcfg.setLogger(logger);
		listCfgView.setLogger(logger);
		settings.setLogger(logger);
		settingsOnDisk.setLogger(logger);
		installer.setLogger(logger);
		installDlg.setLogger(logger);
		trashView.setLogger(logger);
		entryEditDlg.setLogger(logger);
		mountTable.setLogger(logger);
		savedListCfg.setLogger(logger);
		fbResolutionsGetter.setLogger(logger);
		settingsDlg.setLogger(logger);
		deviceDataList.setLogger(logger);
		aboutDialog.setLogger(logger);
		listCfgMutex1.setLogger(logger);
		listCfgMutex2.setLogger(logger);
		threadC.setLogger(logger);
		env.setLogger(logger);
		envEditor.setLogger(logger);
		mainController.setLogger(logger);
		entryEditController.setLogger(logger);
		settingsController.setLogger(logger);
		envEditController.setLogger(logger);
		trashController.setLogger(logger);
		errorController.setLogger(logger);
		installController.setLogger(logger);
		aboutController.setLogger(logger);
		themeEditor.setLogger(logger);
		themeController.setLogger(logger);

		// configure logger
		logger.setLogLevel(Logger_Stream::LOG_EVENT);
		if (argc > 1) {
			std::string logParam = argv[1];
			if (logParam == "debug") {
				logger.setLogLevel(Logger_Stream::LOG_DEBUG_ONLY);
			} else if (logParam == "log-important") {
				logger.setLogLevel(Logger_Stream::LOG_IMPORTANT);
			} else if (logParam == "quiet") {
				logger.setLogLevel(Logger_Stream::LOG_NOTHING);
			} else if (logParam == "verbose") {
				logger.setLogLevel(Logger_Stream::LOG_VERBOSE);
			}
		}

		//configure contentParser factory
		ContentParser_Linux linuxParser(deviceMap);
		ContentParser_LinuxIso linuxIsoParser(deviceMap);
		ContentParser_Chainloader chainloadParser(deviceMap);
		ContentParser_Memtest memtestParser(deviceMap);

		contentParserFactory.registerParser(linuxParser, gettext("Linux"));
		contentParserFactory.registerParser(linuxIsoParser, gettext("Linux-ISO"));
		contentParserFactory.registerParser(chainloadParser, gettext("Chainloader"));
		contentParserFactory.registerParser(memtestParser, gettext("Memtest"));

		entryEditDlg.setAvailableEntryTypes(contentParserFactory.getNames());

		//set env
		listcfg.setEnv(env);
		savedListCfg.setEnv(env);
		settings.setEnv(env);
		settingsOnDisk.setEnv(env);
		installer.setEnv(env);
		themeManager.setEnv(env);
		entryEditController.setEnv(env);
		mainController.setEnv(env);
		settingsController.setEnv(env);
		envEditController.setEnv(env);
		trashController.setEnv(env);
		installController.setEnv(env);
		themeController.setEnv(env);
		deviceMap.setEnv(env);

		//set mutex
		listcfg.setMutex(listCfgMutex1);
		savedListCfg.setMutex(listCfgMutex2);

		mainController.initAction();
		errorController.setApplicationStarted(true);
		app.run();
	} catch (Exception const& e) {
		logger.log(e, Logger::ERROR);
		return 1;
	}
}

