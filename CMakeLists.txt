project(GrubCustomizer)
cmake_minimum_required(VERSION 2.6.2) # may be lower


find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(GTHREAD gthread-2.0)
pkg_check_modules(OPENSSL openssl)
pkg_check_modules(LIBARCHIVE libarchive)

if ( LIB_INSTALL_DIR )
else()
set ( LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/grub-customizer )
endif()

if ( BINARY_INSTALL_DIR )
else()
set ( BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin )
endif()

if ( LOCALE_INSTALL_DIR )
else()
set ( LOCALE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/locale )
endif()

if ( PARTCHOOSER_MOUNTPOINT_DIR )
else()
set ( PARTCHOOSER_MOUNTPOINT_DIR /media/grub-customizer_recovery_root_mountpoint )
endif()

if ( ERROR_LOG_FILE )
else()
set ( ERROR_LOG_FILE /tmp/grub-customizer_mkconfig_output.log )
endif()


link_directories(
    ${GTKMM_LIBRARY_DIRS} ${OPENSSL_LIBRARY_DIRS} ${LIBARCHIVE_LIBRARY_DIRS} )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

add_executable(grub-customizer
	src/Model/Entry.cpp
	src/Model/ListCfg.cpp
	src/Model/ProxyScriptData.cpp
	src/Model/MountTable.cpp
	src/Model/Proxy.cpp
	src/Model/Proxylist.cpp
	src/Model/PscriptnameTranslator.cpp
	src/Model/Repository.cpp
	src/Model/Rule.cpp
	src/Model/Script.cpp
	src/Model/DeviceDataList.cpp
	src/Model/Env.cpp
	src/Model/SettingsManagerData.cpp
	src/Model/SettingsStore.cpp
	src/Model/FbResolutionsGetter.cpp
	src/Model/Installer.cpp
	src/Model/EntryPathBuilderImpl.cpp
	src/Model/DeviceMap.cpp
	src/Model/SmartFileHandle.cpp
	src/Mapper/EntryNameImpl.cpp
	src/main/gtk-client.cpp
	src/lib/md5.cpp
	src/lib/str_replace.cpp
	src/lib/ContentParser/FactoryImpl.cpp
	src/lib/ContentParser/Linux.cpp
	src/lib/ContentParser/Chainloader.cpp
	src/lib/ContentParser/LinuxIso.cpp
	src/lib/ContentParser/Memtest.cpp
	src/lib/ContentParser/Abstract.cpp
	src/lib/Mutex/GLib.cpp
	src/Controller/GLib/ThreadController.cpp
	src/lib/Logger/Stream.cpp
	src/lib/regex.cpp
	src/View/Gtk/Element/PartitionChooser.cpp
	src/View/Gtk/About.cpp
	src/View/Gtk/EntryEditor.cpp
	src/View/Gtk/EnvEditor.cpp
	src/View/Gtk/Installer.cpp
	src/View/Gtk/Main.cpp
	src/View/Gtk/Settings.cpp
	src/View/Gtk/Trash.cpp
	src/View/Gtk/Error.cpp
	src/Controller/EntryEditControllerImpl.cpp
	src/Controller/MainControllerImpl.cpp
	src/Controller/ControllerAbstract.cpp
	src/Controller/SettingsControllerImpl.cpp
	src/Controller/EnvEditorControllerImpl.cpp
	src/Controller/TrashControllerImpl.cpp
	src/Controller/InstallerControllerImpl.cpp
	src/Controller/AboutControllerImpl.cpp
	src/Controller/ErrorControllerImpl.cpp
	src/Controller/ThemeControllerImpl.cpp
	src/lib/Exception.cpp
	src/lib/assert.cpp
	src/lib/ArrayStructure.cpp
	src/lib/trim.cpp
	src/Controller/Helper/DeviceInfo.cpp
	src/lib/csv.cpp
	src/Model/ScriptSourceMap.cpp
	src/View/Gtk/Theme.cpp
	src/Model/ThemeFile.cpp
	src/Model/Theme.cpp
	src/Model/ThemeManager.cpp
	src/lib/FileSystem.cpp
)


add_executable(grubcfg-proxy
	src/Model/Entry.cpp
	src/Model/Rule.cpp
	src/Model/Script.cpp
	src/Model/Proxy.cpp
	src/main/proxy.cpp
	src/lib/md5.cpp
	src/lib/str_replace.cpp
	# multi:
	src/Model/ListCfg.cpp
	src/Model/Env.cpp
	src/Model/SettingsStore.cpp
	src/Model/MountTable.cpp
	src/Model/Proxylist.cpp
	src/Model/Repository.cpp
	src/Model/PscriptnameTranslator.cpp
	src/Model/ProxyScriptData.cpp
	src/Model/EntryPathBuilderImpl.cpp
	src/lib/Exception.cpp
	src/lib/assert.cpp
	src/lib/ArrayStructure.cpp
	src/lib/trim.cpp
	src/Model/ScriptSourceMap.cpp
	src/lib/csv.cpp
	src/lib/FileSystem.cpp
)

target_link_libraries(grub-customizer 
    ${GTKMM_LIBRARIES} ${GTHREAD_LIBRARIES} ${OPENSSL_LIBRARIES} ${LIBARCHIVE_LIBRARIES})

target_link_libraries(grubcfg-proxy 
    ${OPENSSL_LIBRARIES})

configure_file ("config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h")

configure_file ("misc/pkexec_policy.in" "${CMAKE_CURRENT_BINARY_DIR}/net.launchpad.danielrichter2007.pkexec.grub-customizer.policy")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/net.launchpad.danielrichter2007.pkexec.grub-customizer.policy
    DESTINATION /usr/share/polkit-1/actions)


install(TARGETS grubcfg-proxy 
	DESTINATION ${LIB_INSTALL_DIR} )

install(TARGETS grub-customizer 
	DESTINATION bin)

install(FILES misc/manpage.gz
	DESTINATION share/man/man1
	RENAME grub-customizer.1.gz)

#install(FILES misc/copyright
#	DESTINATION share/doc/grub-customizer)

install(FILES translation/translation-de.mo
	DESTINATION share/locale/de/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-bg.mo
	DESTINATION share/locale/bg/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-el.mo
	DESTINATION share/locale/el/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-gl.mo
	DESTINATION share/locale/gl/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-hr.mo
	DESTINATION share/locale/hr/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-pl.mo
	DESTINATION share/locale/pl/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-pt_BR.mo
	DESTINATION share/locale/pt_BR/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ru.mo
	DESTINATION share/locale/ru/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-tr.mo
	DESTINATION share/locale/tr/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-uk.mo
	DESTINATION share/locale/uk/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-it.mo
	DESTINATION share/locale/it/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-es.mo
	DESTINATION share/locale/es/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-fr.mo
	DESTINATION share/locale/fr/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-zh_CN.mo
	DESTINATION share/locale/zh_CN/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-zh_TW.mo
	DESTINATION share/locale/zh_TW/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-cs.mo
	DESTINATION share/locale/cs/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-cy.mo
	DESTINATION share/locale/cy/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-hu.mo
	DESTINATION share/locale/hu/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-sk.mo
	DESTINATION share/locale/sk/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ast.mo
	DESTINATION share/locale/ast/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-kk.mo
	DESTINATION share/locale/kk/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-nl.mo
	DESTINATION share/locale/nl/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ca.mo
	DESTINATION share/locale/ca/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ar.mo
	DESTINATION share/locale/ar/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-vi.mo
	DESTINATION share/locale/vi/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-fi.mo
	DESTINATION share/locale/fi/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-lt.mo
	DESTINATION share/locale/lt/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-bs.mo
	DESTINATION share/locale/bs/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-et.mo
	DESTINATION share/locale/et/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-eu.mo
	DESTINATION share/locale/eu/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-en_GB.mo
	DESTINATION share/locale/en_GB/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES translation/translation-ja.mo
	DESTINATION share/locale/ja/LC_MESSAGES/
	RENAME grub-customizer.mo)

install(FILES misc/grub-customizer.desktop
	DESTINATION /usr/share/applications
)


#install icons
install(FILES misc/icons/16x16.png DESTINATION /usr/share/icons/hicolor/16x16/apps RENAME grub-customizer.png)
install(FILES misc/icons/22x22.png DESTINATION /usr/share/icons/hicolor/22x22/apps RENAME grub-customizer.png)
install(FILES misc/icons/24x24.png DESTINATION /usr/share/icons/hicolor/24x24/apps RENAME grub-customizer.png)
install(FILES misc/icons/32x32.png DESTINATION /usr/share/icons/hicolor/32x32/apps RENAME grub-customizer.png)
install(FILES misc/icons/48x48.png DESTINATION /usr/share/icons/hicolor/48x48/apps RENAME grub-customizer.png)
install(FILES misc/icons/64x64.png DESTINATION /usr/share/icons/hicolor/64x64/apps RENAME grub-customizer.png)
install(FILES misc/icons/128x128.png DESTINATION /usr/share/icons/hicolor/128x128/apps RENAME grub-customizer.png)


# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
