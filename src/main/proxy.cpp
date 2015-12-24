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

#include "../Model/Script.h"
#include "../Model/Proxy.h"
#include "../Model/Entry.h"
#include "../Model/Rule.h"
#include "../Model/ListCfg.h" // multi

#include <iostream>

int main(int argc, char** argv){
	if (argc == 2) {
		Model_Script script("noname", "");
		Model_Entry newEntry;
		std::string plaintextBuffer;
		while (newEntry = Model_Entry(stdin, Model_Entry_Row(), NULL, &plaintextBuffer)) {
			script.entries().push_back(newEntry);
		}
		if (plaintextBuffer.size()) {
			script.entries().push_front(Model_Entry("#text", "", plaintextBuffer, Model_Entry::PLAINTEXT));
		}

		Model_Proxy proxy;
		proxy.importRuleString(argv[1], "");

		proxy.dataSource = &script;
		proxy.sync(true, true);
		
		for (std::list<Model_Rule>::iterator iter = proxy.rules.begin(); iter != proxy.rules.end(); iter++){
			iter->print(std::cout);
		}
		return 0;
	} else if (argc == 3 && std::string(argv[2]) == "multi") {
		Model_Env env;
		Model_ListCfg scriptSource;
		scriptSource.setEnv(env);
		scriptSource.ignoreLock = true;
		{ // this scope prevents access to the unused proxy variable - push_back takes a copy!
			Model_Proxy proxy;
			proxy.importRuleString(argv[1], env.cfg_dir_prefix);
			scriptSource.proxies.push_back(proxy);
		}
		scriptSource.readGeneratedFile(stdin, true, false);

		scriptSource.proxies.front().dataSource = &scriptSource.repository.front(); // the first Script is always the main script

		std::map<std::string, Model_Script*> map = scriptSource.repository.getScriptPathMap();
		scriptSource.proxies.front().sync(true, true, map);

		for (std::list<Model_Rule>::iterator iter = scriptSource.proxies.front().rules.begin(); iter != scriptSource.proxies.front().rules.end(); iter++){
			iter->print(std::cout);
		}
	} else {
		std::cerr << "wrong argument count. You have to give the config as parameter 1!" << std::endl;
		return 1;
	}
}
