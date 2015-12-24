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

#include "ListCfg.h"

Model_ListCfg::Model_ListCfg()
 : error_proxy_not_found(false),
 progress(0),
 cancelThreadsRequested(false), verbose(true),
 errorLogFile(ERROR_LOG_FILE), ignoreLock(false), progress_pos(0), progress_max(0)
{}

void Model_ListCfg::setLogger(Logger& logger) {
	this->Trait_LoggerAware::setLogger(logger);
	this->proxies.setLogger(logger);
	this->repository.setLogger(logger);
	this->scriptSourceMap.setLogger(logger);
}

void Model_ListCfg::setEnv(Model_Env& env) {
	this->Model_Env_Connection::setEnv(env);
	this->scriptSourceMap.setEnv(env);
}

void Model_ListCfg::lock(){
	if (this->ignoreLock)
		return;
	if (this->mutex == NULL)
		throw ConfigException("missing mutex", __FILE__, __LINE__);
	this->mutex->lock();
}
bool Model_ListCfg::lock_if_free(){
	if (this->ignoreLock)
		return true;
	if (this->mutex == NULL)
		throw ConfigException("missing mutex", __FILE__, __LINE__);
	return this->mutex->trylock();
}
void Model_ListCfg::unlock(){
	if (this->ignoreLock)
		return;
	if (this->mutex == NULL)
		throw ConfigException("missing mutex", __FILE__, __LINE__);
	this->mutex->unlock();
}

bool Model_ListCfg::createScriptForwarder(std::string const& scriptName) const {
	//replace: $cfg_dir/proxifiedScripts/ -> $cfg_dir/LS_
	std::string scriptNameNoPath = scriptName.substr((this->env->cfg_dir+"/proxifiedScripts/").length());
	std::string outputFilePath = this->env->cfg_dir+"/LS_"+scriptNameNoPath;
	FILE* existingScript = fopen(outputFilePath.c_str(), "r");
	if (existingScript == NULL){
		assert_filepath_empty(outputFilePath, __FILE__, __LINE__);
		FILE* fwdScript = fopen(outputFilePath.c_str(), "w");
		if (fwdScript){
			fputs("#!/bin/sh\n", fwdScript);
			fputs(("'"+scriptName.substr(env->cfg_dir_prefix.length())+"'").c_str(), fwdScript);
			fclose(fwdScript);
			chmod(outputFilePath.c_str(), 0755);
			return true;
		}
		else
			return false;
	}
	else {
		fclose(existingScript);
		return false;
	}
}

bool Model_ListCfg::removeScriptForwarder(std::string const& scriptName) const {
	std::string scriptNameNoPath = scriptName.substr((this->env->cfg_dir+"/proxifiedScripts/").length());
	std::string filePath = this->env->cfg_dir+"/LS_"+scriptNameNoPath;
	return unlink(filePath.c_str()) == 0;
}

std::string Model_ListCfg::readScriptForwarder(std::string const& scriptForwarderFilePath) const {
	std::string result;
	FILE* scriptForwarderFile = fopen(scriptForwarderFilePath.c_str(), "r");
	if (scriptForwarderFile){
		int c;
		while ((c = fgetc(scriptForwarderFile)) != EOF && c != '\n'){} //skip first line
		if (c != EOF)
			while ((c = fgetc(scriptForwarderFile)) != EOF && c != '\n'){result += char(c);} //read second line (=path)
		fclose(scriptForwarderFile);
	}
	if (result.length() >= 3) {
		return result.substr(1, result.length()-2);
	} else {
		return "";
	}
}

void Model_ListCfg::load(bool preserveConfig){
	if (!preserveConfig){
		send_new_load_progress(0);

		DIR* hGrubCfgDir = opendir(this->env->cfg_dir.c_str());

		if (!hGrubCfgDir){
			throw DirectoryNotFoundException("grub cfg dir not found", __FILE__, __LINE__);
		}

		//load scripts
		this->log("loading scripts…", Logger::EVENT);
		this->lock();
		repository.load(this->env->cfg_dir, false);
		repository.load(this->env->cfg_dir+"/proxifiedScripts", true);
		this->unlock();
		send_new_load_progress(0.05);
	
	
		//load proxies
		this->log("loading proxies…", Logger::EVENT);
		this->lock();
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(hGrubCfgDir))){
			stat((this->env->cfg_dir+"/"+entry->d_name).c_str(), &fileProperties);
			if ((fileProperties.st_mode & S_IFMT) != S_IFDIR){ //ignore directories
				if (entry->d_name[2] == '_'){ //check whether it's an script (they should be named XX_scriptname)…
					this->proxies.push_back(Model_Proxy());
					this->proxies.back().fileName = this->env->cfg_dir+"/"+entry->d_name;
					this->proxies.back().index = (entry->d_name[0]-'0')*10 + (entry->d_name[1]-'0');
					this->proxies.back().permissions = fileProperties.st_mode & ~S_IFMT;
				
					FILE* proxyFile = fopen((this->env->cfg_dir+"/"+entry->d_name).c_str(), "r");
					Model_ProxyScriptData data(proxyFile);
					fclose(proxyFile);
					if (data){
						this->proxies.back().dataSource = repository.getScriptByFilename(this->env->cfg_dir_prefix+data.scriptCmd);
						this->proxies.back().importRuleString(data.ruleString.c_str(), this->env->cfg_dir_prefix);
					}
					else {
						this->proxies.back().dataSource = repository.getScriptByFilename(this->env->cfg_dir+"/"+entry->d_name);
						this->proxies.back().importRuleString("+*", this->env->cfg_dir_prefix); //it's no proxy, so accept all
					}
				
				}
			}
		}
		closedir(hGrubCfgDir);
		this->proxies.sort();
		this->unlock();


		//clean up proxy configuration
		this->log("cleaning up proxy configuration…", Logger::EVENT);
		this->lock();

		bool proxyRemoved = false;
		do {
			for (std::list<Model_Proxy>::iterator pIter = this->proxies.begin(); pIter != this->proxies.end(); pIter++) {
				if (!pIter->isExecutable() || !pIter->hasVisibleRules()) {
					this->log(pIter->fileName + " has no visible entries and will be removed / disabled", Logger::INFO);
					this->proxies.deleteProxy(&*pIter);
					proxyRemoved = true;
					break;
				}
			}
			proxyRemoved = false;
		} while (proxyRemoved);

		this->unlock();
	}
	else {
		this->lock();
		proxies.unsync_all();
		repository.deleteAllEntries();
		this->unlock();
	}

	//create proxifiedScript links & chmod other files
	this->log("creating proxifiedScript links & chmodding other files…", Logger::EVENT);

	this->lock();
	for (Model_Repository::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++){
		if (iter->isInScriptDir(env->cfg_dir)){
			//createScriptForwarder & disable proxies
			createScriptForwarder(iter->fileName);
			std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*iter);
			for (std::list<Model_Proxy*>::iterator piter = relatedProxies.begin(); piter != relatedProxies.end(); piter++){
				int res = chmod((*piter)->fileName.c_str(), 0644);
			}
		} else {
			//enable scripts (unproxified), in this case, Proxy::fileName == Script::fileName
			chmod(iter->fileName.c_str(), 0755);
		}
	}
	this->unlock();
	send_new_load_progress(0.1);


	if (!preserveConfig){
		//load script map
		this->scriptSourceMap.load();
		if (!this->scriptSourceMap.fileExists() && this->getProxifiedScripts().size() > 0) {
			this->generateScriptSourceMap();
		}
		this->populateScriptSourceMap();
	}

	//run mkconfig
	this->log("running " + this->env->mkconfig_cmd, Logger::EVENT);
	FILE* mkconfigProc = popen((this->env->mkconfig_cmd + " 2> " + this->errorLogFile).c_str(), "r");
	readGeneratedFile(mkconfigProc);
	
	int success = pclose(mkconfigProc);
	if (success != 0 && !cancelThreadsRequested){
		throw CmdExecException("failed running " + this->env->mkconfig_cmd, __FILE__, __LINE__);
	} else {
		remove(errorLogFile.c_str()); //remove file, if everything was ok
	}
	this->log("mkconfig successfull completed", Logger::INFO);

	this->send_new_load_progress(0.9);

	mkconfigProc = NULL;
	
	this->env->useDirectBackgroundProps = this->repository.getScriptByName("debian_theme") == NULL;
	if (this->env->useDirectBackgroundProps) {
		this->log("using simple background image settings", Logger::INFO);
	} else {
		this->log("using /usr/share/desktop-base/grub_background.sh to configure colors and the background image", Logger::INFO);
	}

	
	//restore old configuration
	this->log("restoring grub configuration", Logger::EVENT);
	this->lock();
	for (Model_Repository::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++){
		if (iter->isInScriptDir(env->cfg_dir)){
			//removeScriptForwarder & reset proxy permissions
			bool result = removeScriptForwarder(iter->fileName);
			if (!result) {
				this->log("removing of script forwarder not successful!", Logger::ERROR);
			}
		}
		std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*iter);
		for (std::list<Model_Proxy*>::iterator piter = relatedProxies.begin(); piter != relatedProxies.end(); piter++){
			chmod((*piter)->fileName.c_str(), (*piter)->permissions);
		}
	}
	this->unlock();
	
	//remove invalid proxies from list (no file system action here)
	this->log("removing invalid proxies from list", Logger::EVENT);
	std::string invalidProxies = "";
	bool foundInvalidScript = false;
	do {
		foundInvalidScript = false;
		for (std::list<Model_Proxy>::iterator pIter = this->proxies.begin(); pIter != this->proxies.end(); pIter++){
			if (pIter->dataSource == NULL) {
				this->proxies.trash.push_back(*pIter); // mark for deletion
				this->proxies.erase(pIter);
				foundInvalidScript = true;
				invalidProxies += pIter->fileName + ",";
				break;
			}
		}
	} while (foundInvalidScript);

	if (invalidProxies != "") {
		this->log("found invalid proxies: " + rtrim(invalidProxies, ","), Logger::INFO);
	}

	//fix conflicts (same number, same name but one script with "-proxy" the other without
	if (this->proxies.hasConflicts()) {
		this->log("found conflicts - renumerating", Logger::INFO);
		this->renumerate();
	}

	this->log("loading completed", Logger::EVENT);
	send_new_load_progress(1);
}


void Model_ListCfg::readGeneratedFile(FILE* source, bool createScriptIfNotFound, bool createProxyIfNotFound){
	Model_Entry_Row row;
	Model_Script* script;
	int i = 0;
	bool inScript = false;
	std::string plaintextBuffer = "";
	int innerCount = 0;
	double progressbarScriptSpace = 0.7 / this->repository.size();
	while (!cancelThreadsRequested && (row = Model_Entry_Row(source))){
		std::string rowText = ltrim(row.text);
		if (!inScript && rowText.substr(0,10) == ("### BEGIN ") && rowText.substr(rowText.length()-4,4) == " ###"){
			this->lock();
			if (script) {
				if (plaintextBuffer != "" && !script->isModified()) {
					Model_Entry newEntry("#text", "", plaintextBuffer, Model_Entry::PLAINTEXT);
					if (this->hasLogger()) {
						newEntry.setLogger(this->getLogger());
					}
					script->entries().push_front(newEntry);
				}
				this->proxies.sync_all(true, true, script);
			}
			plaintextBuffer = "";
			std::string scriptName = rowText.substr(10, rowText.length()-14);
			std::string prefix = this->env->cfg_dir_prefix;
			std::string realScriptName = prefix+scriptName;
			if (realScriptName.substr(0, (this->env->cfg_dir+"/LS_").length()) == this->env->cfg_dir+"/LS_"){
				realScriptName = prefix+readScriptForwarder(realScriptName);
			}
			script = repository.getScriptByFilename(realScriptName, createScriptIfNotFound);
			if (createScriptIfNotFound && createProxyIfNotFound){ //for the compare-configuration
				this->proxies.push_back(Model_Proxy(*script));
			}
			this->unlock();
			if (script){
				this->send_new_load_progress(0.1 + (progressbarScriptSpace * ++i + (progressbarScriptSpace/10*innerCount)), script->name, i, this->repository.size());
			}
			inScript = true;
		} else if (inScript && rowText.substr(0,8) == ("### END ") && rowText.substr(rowText.length()-4,4) == " ###") {
			inScript = false;
			innerCount = 0;
		} else if (script != NULL && rowText.substr(0, 10) == "menuentry ") {
			this->lock();
			if (innerCount < 10) {
				innerCount++;
			}
			Model_Entry newEntry(source, row, this->getLoggerPtr());
			if (!script->isModified()) {
				script->entries().push_back(newEntry);
			}
			this->proxies.sync_all(false, false, script);
			this->unlock();
			this->send_new_load_progress(0.1 + (progressbarScriptSpace * i + (progressbarScriptSpace/10*innerCount)), script->name, i, this->repository.size());
		} else if (script != NULL && rowText.substr(0, 8) == "submenu ") {
			this->lock();
			Model_Entry newEntry(source, row, this->getLoggerPtr());
			script->entries().push_back(newEntry);
			this->proxies.sync_all(false, false, script);
			this->unlock();
			this->send_new_load_progress(0.1 + (progressbarScriptSpace * i + (progressbarScriptSpace/10*innerCount)), script->name, i, this->repository.size());
		} else if (inScript) { //Plaintext
			plaintextBuffer += row.text + "\n";
		}
	}
	this->lock();
	if (script) {
		if (plaintextBuffer != "" && !script->isModified()) {
			Model_Entry newEntry("#text", "", plaintextBuffer, Model_Entry::PLAINTEXT);
			if (this->hasLogger()) {
				newEntry.setLogger(this->getLogger());
			}
			script->entries().push_front(newEntry);
		}
		this->proxies.sync_all(true, true, script);
	}

	// sync all (including foreign entries)
	this->proxies.sync_all(true, true, NULL, this->repository.getScriptPathMap());

	this->unlock();
}

void Model_ListCfg::save(){
	send_new_save_progress(0);
	std::map<std::string, int> samename_counter;
	proxies.deleteAllProxyscriptFiles();  //delete all proxies to get a clean file system
	proxies.clearTrash(); //delete all files of removed proxies
	repository.clearTrash();
	
	std::map<Model_Script*, std::string> scriptFilenameMap; // stores original filenames
	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		scriptFilenameMap[&*scriptIter] = scriptIter->fileName;
	}

	// create virtual custom scripts on file system
	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		if (scriptIter->isCustomScript && scriptIter->fileName == "") {
			scriptIter->fileName = this->env->cfg_dir + "/IN_" + scriptIter->name;
			this->repository.createScript(*scriptIter, "");
		}
	}

	for (Model_Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++)
		script_iter->moveToBasedir(this->env->cfg_dir);

	send_new_save_progress(0.1);

	int mkdir_result = mkdir((this->env->cfg_dir+"/proxifiedScripts").c_str(), 0755); //create this directory if it doesn't already exist

	// get new script locations
	std::map<Model_Script const*, std::string> scriptTargetMap; // scripts and their target directories
	for (Model_Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++) {
		std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
		if (proxies.proxyRequired(*script_iter)){
			scriptTargetMap[&*script_iter] = this->env->cfg_dir+"/proxifiedScripts/"+Model_PscriptnameTranslator::encode(script_iter->name, samename_counter[script_iter->name]++);
		} else {
			std::ostringstream nameStream;
			nameStream << std::setw(2) << std::setfill('0') << relatedProxies.front()->index << "_" << script_iter->name;
			std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
			scriptTargetMap[&*script_iter] = this->env->cfg_dir+"/"+nameStream.str();
		}
	}

	// move scripts and create proxies
	int proxyCount = 0;
	for (Model_Repository::iterator script_iter = repository.begin(); script_iter != repository.end(); script_iter++){
		std::list<Model_Proxy*> relatedProxies = proxies.getProxiesByScript(*script_iter);
		if (proxies.proxyRequired(*script_iter)){
			script_iter->moveFile(scriptTargetMap[&*script_iter], 0755);
			for (std::list<Model_Proxy*>::iterator proxy_iter = relatedProxies.begin(); proxy_iter != relatedProxies.end(); proxy_iter++){
				std::map<Model_Entry const*, Model_Script const*> entrySourceMap = this->getEntrySources(**proxy_iter);
				std::ostringstream nameStream;
				nameStream << std::setw(2) << std::setfill('0') << (*proxy_iter)->index << "_" << script_iter->name << "_proxy";
				(*proxy_iter)->generateFile(this->env->cfg_dir+"/"+nameStream.str(), this->env->cfg_dir_prefix.length(), this->env->cfg_dir_noprefix, entrySourceMap, scriptTargetMap);
				proxyCount++;
			}
		}
		else {
			if (relatedProxies.size() == 1){
				script_iter->moveFile(scriptTargetMap[&*script_iter], relatedProxies.front()->permissions);
				relatedProxies.front()->fileName = script_iter->fileName; // update filename
			}
			else {
				this->log("GrublistCfg::save: cannot move proxy… only one expected!", Logger::ERROR);
			}
		}	
	}
	send_new_save_progress(0.2);

	// register in script source map
	for (std::map<Model_Script*, std::string>::iterator sMapIter = scriptFilenameMap.begin(); sMapIter != scriptFilenameMap.end(); sMapIter++) {
		this->scriptSourceMap.registerMove(sMapIter->second, sMapIter->first->fileName);
	}
	this->scriptSourceMap.save();

	//remove "proxifiedScripts" dir, if empty
	
	{
		int proxifiedScriptCount = 0;
		struct dirent *entry;
		struct stat fileProperties;
		DIR* hScriptDir = opendir((this->env->cfg_dir+"/proxifiedScripts").c_str());
		while ((entry = readdir(hScriptDir))) {
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != ".."){
				proxifiedScriptCount++;
			}
		}
		closedir(hScriptDir);
		
		if (proxifiedScriptCount == 0)
			rmdir((this->env->cfg_dir+"/proxifiedScripts").c_str());
	}

	//add or remove proxy binary
	
	FILE* proxyBin = fopen((this->env->cfg_dir+"/bin/grubcfg_proxy").c_str(), "r");
	bool proxybin_exists = proxyBin != NULL;
	if (proxyBin) {
		fclose(proxyBin);
	}
	std::string dummyproxy_code = "#!/bin/sh\ncat\n";
	
	/**
	 * copy the grub customizer proxy, if required
	 */
	if (proxyCount != 0){
		// create the bin subdirectory - may already exist
		int bin_mk_success = mkdir((this->env->cfg_dir+"/bin").c_str(), 0755);

		FILE* proxyBinSource = fopen((std::string(LIBDIR)+"/grubcfg-proxy").c_str(), "r");
		
		if (proxyBinSource){
			FILE* proxyBinTarget = fopen((this->env->cfg_dir+"/bin/grubcfg_proxy").c_str(), "w");
			if (proxyBinTarget){
				int c;
				while ((c = fgetc(proxyBinSource)) != EOF){
					fputc(c, proxyBinTarget);
				}
				fclose(proxyBinTarget);
				chmod((this->env->cfg_dir+"/bin/grubcfg_proxy").c_str(), 0755);
			} else {
				this->log("could not open proxy output file!", Logger::ERROR);
			}
			fclose(proxyBinSource);
		} else {
			this->log("proxy could not be copied, generating dummy!", Logger::ERROR);
			FILE* proxyBinTarget = fopen((this->env->cfg_dir+"/bin/grubcfg_proxy").c_str(), "w");
			if (proxyBinTarget){
				fputs(dummyproxy_code.c_str(), proxyBinTarget);
				error_proxy_not_found = true;
				fclose(proxyBinTarget);
				chmod((this->env->cfg_dir+"/bin/grubcfg_proxy").c_str(), 0755);
			} else {
				this->log("coundn't create proxy!", Logger::ERROR);
			}
		}
	}
	else if (proxyCount == 0 && proxybin_exists){
		//the following commands are only cleanup… no problem, when they fail
		unlink((this->env->cfg_dir+"/bin/grubcfg_proxy").c_str());
		rmdir((this->env->cfg_dir+"/bin").c_str());
	}

	//update modified "custom" scripts
	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		if (scriptIter->isCustomScript && scriptIter->isModified()) {
			this->log("modifying script \"" + scriptIter->name + "\"", Logger::INFO);
			assert(scriptIter->fileName != "");
			Model_Proxy dummyProxy(*scriptIter);
			std::ofstream scriptStream(scriptIter->fileName.c_str());
			scriptStream << CUSTOM_SCRIPT_SHEBANG << "\n" << CUSTOM_SCRIPT_PREFIX << "\n";
			for (std::list<Model_Rule>::iterator ruleIter = dummyProxy.rules.begin(); ruleIter != dummyProxy.rules.end(); ruleIter++) {
				ruleIter->print(scriptStream);
				if (ruleIter->dataSource) {
					ruleIter->dataSource->isModified = false;
				}
			}
		}
	}


	int saveProcSuccess = 0;
	std::string saveProcOutput;

	//run update-grub
	FILE* saveProc = popen((env->update_cmd + " 2>&1").c_str(), "r");
	if (saveProc) {
		int c;
		std::string row = "";
		while ((c = fgetc(saveProc)) != EOF) {
			saveProcOutput += char(c);
			if (c == '\n') {
				send_new_save_progress(0.5); //a gui should use pulse() instead of set_fraction
				this->log(row, Logger::INFO);
				row = "";
			} else {
				row += char(c);
			}
		}
		saveProcSuccess = pclose(saveProc);
	}

	// correct pathes of foreign rules (to make sure re-syncing works)
	std::list<Model_Rule*> foreignRules = this->proxies.getForeignRules();
	for (std::list<Model_Rule*>::iterator foreignRuleIter = foreignRules.begin(); foreignRuleIter != foreignRules.end(); foreignRuleIter++) {
		Model_Entry* entry = (*foreignRuleIter)->dataSource;
		assert(entry != NULL);
		Model_Script* script = this->repository.getScriptByEntry(*entry);
		assert(script != NULL);
		(*foreignRuleIter)->__sourceScriptPath = script->fileName;
	}

	send_new_save_progress(1);

	if ((saveProcSuccess != 0 || saveProcOutput.find("Syntax errors are detected in generated GRUB config file") != -1)){
		throw CmdExecException("failed running '" + env->update_cmd + "' output:\n" + saveProcOutput, __FILE__, __LINE__);
	}
}

std::map<Model_Entry const*, Model_Script const*> Model_ListCfg::getEntrySources(Model_Proxy const& proxy, Model_Rule const* parent) const {
	std::list<Model_Rule> const& list = parent ? parent->subRules : proxy.rules;
	std::map<Model_Entry const*, Model_Script const*> result;
	assert(proxy.dataSource != NULL);
	for (std::list<Model_Rule>::const_iterator iter = list.begin(); iter != list.end(); iter++) {
		if (iter->dataSource && !proxy.ruleIsFromOwnScript(*iter)) {
			Model_Script const* script = this->repository.getScriptByEntry(*iter->dataSource);
			if (script != NULL) {
				result[iter->dataSource] = script;
			} else {
				this->log("error finding the associated script! (" + iter->outputName + ")", Logger::WARNING);
			}
		} else if (iter->type == Model_Rule::SUBMENU) {
			std::map<Model_Entry const*, Model_Script const*> subResult = this->getEntrySources(proxy, &*iter);
			if (subResult.size()) {
				result.insert(subResult.begin(), subResult.end());
			}
		}
	}
	return result;
}

bool Model_ListCfg::loadStaticCfg(){
	FILE* oldConfigFile = fopen(env->output_config_file.c_str(), "r");
	if (oldConfigFile){
		this->readGeneratedFile(oldConfigFile, true, true);
		fclose(oldConfigFile);
		return true;
	}
	return false;
}

void Model_ListCfg::renameRule(Model_Rule* rule, std::string const& newName){
	rule->outputName = newName;
}

std::string Model_ListCfg::getRulePath(Model_Rule& rule) {
	Model_Proxy* proxy = this->proxies.getProxyByRule(&rule);
	std::stack<std::string> ruleNameStack;
	ruleNameStack.push(rule.outputName);

	Model_Rule* currentRule = &rule;
	while ((currentRule = proxy->getParentRule(currentRule))) {
		ruleNameStack.push(currentRule->outputName);
	}

	std::string output = ruleNameStack.top();
	ruleNameStack.pop();
	while (ruleNameStack.size()) {
		output += ">" + ruleNameStack.top();
		ruleNameStack.pop();
	}
	return output;
}

std::string Model_ListCfg::getGrubErrorMessage() const {
	FILE* errorLogFile = fopen(this->errorLogFile.c_str(), "r");
	std::string errorMessage;
	int c;
	while ((c = fgetc(errorLogFile)) != EOF) {
		errorMessage += char(c);
	}
	fclose(errorLogFile);
	return errorMessage;
}

bool Model_ListCfg::compare(Model_ListCfg const& other) const {
	std::list<const Model_Rule*> rlist[2];
	for (int i = 0; i < 2; i++){
		const Model_ListCfg* gc = i == 0 ? this : &other;
		for (Model_Proxylist::const_iterator piter = gc->proxies.begin(); piter != gc->proxies.end(); piter++){
			assert(piter->dataSource != NULL);
			if (piter->isExecutable() && piter->dataSource){
				if (piter->dataSource->fileName == "") { // if the associated file isn't found
					return false;
				}
				std::string fname = piter->dataSource->fileName.substr(other.env->cfg_dir.length()+1);
				if (i == 0 || (fname[0] >= '1' && fname[0] <= '9' && fname[1] >= '0' && fname[1] <= '9' && fname[2] == '_')) {
					std::list<Model_Rule const*> comparableRules = this->getComparableRules(piter->rules);
					rlist[i].splice(rlist[i].end(), comparableRules);
				}
			}
		}
	}
	return Model_ListCfg::compareLists(rlist[0], rlist[1]);
}

std::list<Model_Rule const*> Model_ListCfg::getComparableRules(std::list<Model_Rule> const& list) {
	std::list<Model_Rule const*> result;
	for (std::list<Model_Rule>::const_iterator riter = list.begin(); riter != list.end(); riter++){
		if (((riter->type == Model_Rule::NORMAL && riter->dataSource) || (riter->type == Model_Rule::SUBMENU && riter->hasRealSubrules())) && riter->isVisible){
			result.push_back(&*riter);
		}
	}
	return result;
}

bool Model_ListCfg::compareLists(std::list<Model_Rule const*> a, std::list<Model_Rule const*> b) {
	if (a.size() != b.size()) {
		return false;
	}

	std::list<const Model_Rule*>::iterator self_iter = a.begin(), other_iter = b.begin();
	while (self_iter != a.end() && other_iter != b.end()){
		if ((*self_iter)->type != (*other_iter)->type) {
			return false;
		}
		assert((*self_iter)->type == (*other_iter)->type);
		//check this Rule
		if ((*self_iter)->outputName != (*other_iter)->outputName)
			return false;
		if ((*self_iter)->dataSource) {
			if ((*self_iter)->dataSource->extension != (*other_iter)->dataSource->extension)
				return false;
			if ((*self_iter)->dataSource->content != (*other_iter)->dataSource->content)
				return false;
			if ((*self_iter)->dataSource->type != (*other_iter)->dataSource->type)
				return false;
		}
		//check rules inside the submenu
		if ((*self_iter)->type == Model_Rule::SUBMENU && !Model_ListCfg::compareLists(Model_ListCfg::getComparableRules((*self_iter)->subRules), Model_ListCfg::getComparableRules((*other_iter)->subRules))) {
			return false;
		}
		self_iter++;
		other_iter++;
	}
	return true;
}


void Model_ListCfg::send_new_load_progress(double newProgress, std::string scriptName, int current, int max){
	if (this->controller != NULL){
		this->progress = newProgress;
		this->progress_name = scriptName;
		this->progress_pos = current;
		this->progress_max = max;
		this->controller->syncLoadStateThreadedAction();
	}
	else if (this->verbose) {
		this->log("cannot show updated load progress - no UI connected!", Logger::ERROR);
	}
}

void Model_ListCfg::send_new_save_progress(double newProgress){
	if (this->controller != NULL){
		this->progress = newProgress;
		this->controller->syncSaveStateThreadedAction();
	}
	else if (this->verbose) {
		this->log("cannot show updated save progress - no UI connected!", Logger::ERROR);
	}
}

void Model_ListCfg::cancelThreads(){
	cancelThreadsRequested = true;
}


void Model_ListCfg::reset(){
	this->lock();
	this->repository.clear();
	this->repository.trash.clear();
	this->proxies.clear();
	this->proxies.trash.clear();
	this->unlock();
}

double Model_ListCfg::getProgress() const {
	return progress;
}

std::string Model_ListCfg::getProgress_name() const {
	return progress_name;
}
int Model_ListCfg::getProgress_pos() const {
	return progress_pos;
}
int Model_ListCfg::getProgress_max() const {
	return progress_max;
}

void Model_ListCfg::renumerate(bool favorDefaultOrder){
	short int i = 0;
	for (Model_Proxylist::iterator iter = this->proxies.begin(); iter != this->proxies.end(); iter++){
		bool isDefaultNumber = false;
		if (favorDefaultOrder && iter->dataSource) {
			std::string sourceFileName = this->scriptSourceMap.getSourceName(iter->dataSource->fileName);
			try {
				int prefixNum = Model_Script::extractIndexFromPath(sourceFileName, this->env->cfg_dir);
				if (prefixNum >= i) {
					i = prefixNum;
					isDefaultNumber = true;
				}
			} catch (InvalidStringFormatException const& e) {
				this->log(e, Logger::ERROR);
			}
		}

		bool retry = false;
		do {
			retry = false;

			iter->index = i;

			if (!isDefaultNumber && iter->dataSource) {
				// make sure that scripts never get a filePath that matches a script source (unless it's the source script)
				std::ostringstream fullFileName;
				fullFileName << this->env->cfg_dir << "/" << std::setw(2) << std::setfill('0') << i << "_" << iter->dataSource->name;
				if (this->scriptSourceMap.has(fullFileName.str())) {
					i++;
					retry = true;
				}
			}
		} while (retry);

		i++;
	}
	this->proxies.sort();

	if (favorDefaultOrder && i > 100) { // if positions are out of range...
		this->renumerate(false); // retry without favorDefaultOrder
	}
}

Model_Rule& Model_ListCfg::moveRule(Model_Rule* rule, int direction){
	try {
		return this->proxies.getProxyByRule(rule)->moveRule(rule, direction);
	} catch (NoMoveTargetException const& e) {
		Model_Proxy* proxy = this->proxies.getProxyByRule(rule);
		Model_Rule* parent = NULL;
		try {
			parent = proxy->getParentRule(rule);
		} catch (ItemNotFoundException const& e) {/* do nothing */}

		try {
			std::list<Model_Rule>::iterator nextRule = this->proxies.getNextVisibleRule(proxy->getListIterator(*rule, proxy->getRuleList(parent)), direction);
			if (nextRule->type != Model_Rule::SUBMENU) { // create new proxy
				std::list<Model_Rule>::iterator targetRule = proxy->rules.end();
				bool targetRuleFound = false;
				try {
					targetRule = this->proxies.getNextVisibleRule(nextRule, direction);
					targetRuleFound = true;
				} catch (NoMoveTargetException const& e) {
					// ignore GrublistCfg::NO_MOVE_TARGET_FOUND - occurs when previousRule is not found. But this isn't a problem
				}

				if (targetRuleFound && this->proxies.getProxyByRule(&*targetRule)->dataSource == this->proxies.getProxyByRule(&*rule)->dataSource) {
					Model_Proxy* targetProxy = this->proxies.getProxyByRule(&*targetRule);
					targetProxy->removeEquivalentRules(*rule);
					Model_Rule* newRule = NULL;
					if (direction == -1) {
						targetProxy->rules.push_back(*rule);
						newRule = &targetProxy->rules.back();
					} else {
						targetProxy->rules.push_front(*rule);
						newRule = &targetProxy->rules.front();
					}
					rule->setVisibility(false);

					try {
						std::list<Model_Rule>::iterator previousRule = this->proxies.getNextVisibleRule(proxy->getListIterator(*rule, proxy->getRuleList(parent)), -direction);
						if (this->proxies.getProxyByRule(&*nextRule)->dataSource == this->proxies.getProxyByRule(&*previousRule)->dataSource) {
							this->proxies.getProxyByRule(&*previousRule)->removeEquivalentRules(*nextRule);
							if (direction == 1) {
								this->proxies.getProxyByRule(&*previousRule)->rules.push_back(*nextRule);
							} else {
								this->proxies.getProxyByRule(&*previousRule)->rules.push_front(*nextRule);
							}
							nextRule->setVisibility(false);
							if (!this->proxies.getProxyByRule(&*nextRule)->hasVisibleRules()) {
								this->proxies.deleteProxy(this->proxies.getProxyByRule(&*nextRule));
							}
						}
					} catch (NoMoveTargetException const& e) {
						// ignore NoMoveTargetException - occurs when previousRule is not found. But this isn't a problem
					}

					// cleanup
					if (!proxy->hasVisibleRules()) {
						this->proxies.deleteProxy(proxy);
					}

					return *newRule;
				} else {
					std::list<Model_Rule>::iterator movedRule = this->proxies.moveRuleToNewProxy(*rule, direction);

					Model_Proxy* currentProxy = this->proxies.getProxyByRule(&*movedRule);

					std::list<Model_Rule>::iterator movedRule2 = this->proxies.moveRuleToNewProxy(*nextRule, -direction);
					this->renumerate();
					this->swapProxies(currentProxy, this->proxies.getProxyByRule(&*movedRule2));

					try {
						std::list<Model_Rule>::iterator prevPrevRule = this->proxies.getNextVisibleRule(movedRule2, -direction);

						if (this->proxies.getProxyByRule(&*prevPrevRule)->dataSource == this->proxies.getProxyByRule(&*movedRule2)->dataSource) {
							Model_Proxy* prevprev = this->proxies.getProxyByRule(&*prevPrevRule);
							prevprev->removeEquivalentRules(*movedRule2);
							if (direction == 1) {
								prevprev->rules.push_back(*movedRule2);
							} else {
								prevprev->rules.push_front(*movedRule2);
							}
							movedRule2->setVisibility(false);
							if (!this->proxies.getProxyByRule(&*movedRule2)->hasVisibleRules()) {
								this->proxies.deleteProxy(this->proxies.getProxyByRule(&*movedRule2));
							}
						}
					} catch (NoMoveTargetException const& e) {
						// ignore NoMoveTargetException - occurs when prevPrevRule is not found. But this isn't a problem
					}
					this->renumerate();

					return *movedRule;
				}
			} else { // convert existing proxy to multiproxy
				this->log("convert to multiproxy", Logger::INFO);
				std::list<Model_Proxy>::iterator proxyIter = this->proxies.getIter(proxy);

				Model_Rule* movedRule = NULL;
				Model_Proxy* target = NULL;
				if (direction == -1 && proxyIter != this->proxies.begin()) {
					proxyIter--;
					target = &*proxyIter;
					target->removeEquivalentRules(*rule);
					nextRule->subRules.push_back(*rule);
					if (rule->type == Model_Rule::SUBMENU) {
						proxy->removeForeignChildRules(*rule);
					}
					if ((rule->type == Model_Rule::SUBMENU && rule->subRules.size() != 0) || (rule->type != Model_Rule::SUBMENU && proxy->ruleIsFromOwnScript(*rule))) {
						rule->setVisibility(false);
					} else {
						proxy->rules.pop_front();
					}
					movedRule = &nextRule->subRules.back();
					proxyIter++;
					proxyIter++; // go to the next proxy
				} else if (direction == 1 && proxyIter != this->proxies.end() && &*proxyIter != &this->proxies.back()) {
					proxyIter++;
					target = &*proxyIter;
					target->removeEquivalentRules(*rule);
					nextRule->subRules.push_front(*rule);
					if (rule->type == Model_Rule::SUBMENU) {
						proxy->removeForeignChildRules(*rule);
					}
					if ((rule->type == Model_Rule::SUBMENU && rule->subRules.size() != 0) || (rule->type != Model_Rule::SUBMENU && proxy->ruleIsFromOwnScript(*rule))) {
						rule->setVisibility(false);
					} else {
						proxy->rules.pop_back();
					}
					movedRule = &nextRule->subRules.front();

					proxyIter--;
					proxyIter--; // go to the previous proxy
				} else {
					throw NoMoveTargetException("cannot move this rule", __FILE__, __LINE__);
				}

				if (!proxy->hasVisibleRules()) {
					if (proxyIter != this->proxies.end() && target->dataSource == proxyIter->dataSource) {
						target->merge(*proxyIter, direction);
						this->proxies.deleteProxy(&*proxyIter);
					}

					this->proxies.deleteProxy(proxy);
				}
				return *movedRule;
			}
		} catch (NoMoveTargetException const& e) {
			throw e;
		}
	} catch (MustBeProxyException const& e) {
		Model_Proxy* proxy = this->proxies.getProxyByRule(rule);
		Model_Rule* parent = NULL;
		try {
			parent = proxy->getParentRule(rule);
		} catch (ItemNotFoundException const& e) {/* do nothing */}

		Model_Rule* parentSubmenu = parent;
		try {
			std::list<Model_Rule>::iterator nextRule = this->proxies.getNextVisibleRule(proxy->getListIterator(*parent, proxy->rules), direction); // go forward

			if (this->proxies.getProxyByRule(&*nextRule) == this->proxies.getProxyByRule(&*rule)) {
				this->proxies.splitProxy(proxy, &*nextRule, direction);
			}
		} catch (NoMoveTargetException const& e) {
			// there's no next rule… no split required
		}

		std::list<Model_Proxy>::iterator nextProxy = this->proxies.getIter(this->proxies.getProxyByRule(&*rule));
		if (direction == 1) {
			nextProxy++;
		} else {
			nextProxy--;
		}

		Model_Rule* movedRule = NULL;
		if (nextProxy != this->proxies.end() && nextProxy->dataSource == this->repository.getScriptByEntry(*rule->dataSource)) {
			nextProxy->removeEquivalentRules(*rule);
			if (direction == 1) {
				nextProxy->rules.push_front(*rule);
				movedRule = &nextProxy->rules.front();
			} else {
				nextProxy->rules.push_back(*rule);
				movedRule = &nextProxy->rules.back();
			}
		} else {
			movedRule = &*this->proxies.moveRuleToNewProxy(*rule, direction, this->repository.getScriptByEntry(*rule->dataSource));
		}

		if (this->proxies.hasProxy(proxy)) { // check whether the proxy pointer is still valid
			proxy->removeEquivalentRules(*rule);
		}
		return *movedRule;
	}
	throw NoMoveTargetException("no move target found", __FILE__, __LINE__);
}

void Model_ListCfg::swapProxies(Model_Proxy* a, Model_Proxy* b){
	if (a->index == b->index) { // swapping has no effect if the indexes are identical
		this->renumerate();
	}
	int index1 = a->index;
	a->index = b->index;
	b->index = index1;
	this->proxies.sort();
	this->renumerate();
}

Model_Rule* Model_ListCfg::createSubmenu(Model_Rule* position) {
	return this->proxies.getProxyByRule(position)->createSubmenu(position);
}

Model_Rule* Model_ListCfg::splitSubmenu(Model_Rule* child) {
	return this->proxies.getProxyByRule(child)->splitSubmenu(child);
}

bool Model_ListCfg::cfgDirIsClean(){
	DIR* hGrubCfgDir = opendir(this->env->cfg_dir.c_str());
	if (hGrubCfgDir){
		struct dirent *entry;
		struct stat fileProperties;
		while ((entry = readdir(hGrubCfgDir))){
			std::string fname = entry->d_name;
			if ((fname.length() >= 4 && fname.substr(0,3) == "LS_") || fname.substr(0,3) == "PS_" || fname.substr(0,3) == "DS_")
				return false;
		}
		closedir(hGrubCfgDir);
	}
	return true;
}
void Model_ListCfg::cleanupCfgDir(){
	this->log("cleaning up cfg dir!", Logger::IMPORTANT_EVENT);
	
	DIR* hGrubCfgDir = opendir(this->env->cfg_dir.c_str());
	if (hGrubCfgDir){
		struct dirent *entry;
		struct stat fileProperties;
		std::list<std::string> lsfiles, dsfiles, psfiles;
		std::list<std::string> proxyscripts;
		while ((entry = readdir(hGrubCfgDir))){
			std::string fname = entry->d_name;
			if (fname.length() >= 4){
				if (fname.substr(0,3) == "LS_")
					lsfiles.push_back(fname);
				else if (fname.substr(0,3) == "DS_")
					dsfiles.push_back(fname);
				else if (fname.substr(0,3) == "PS_")
					psfiles.push_back(fname);

				else if (fname[0] >= '1' && fname[0] <= '9' && fname[1] >= '0' && fname[1] <= '9' && fname[2] == '_')
					proxyscripts.push_back(fname);
			}
		}
		closedir(hGrubCfgDir);
		
		for (std::list<std::string>::iterator iter = lsfiles.begin(); iter != lsfiles.end(); iter++){
			this->log("deleting " + *iter, Logger::EVENT);
			unlink((this->env->cfg_dir+"/"+(*iter)).c_str());
		}
		//proxyscripts will be disabled before loading the config. While the provious mode will only be saved on the objects, every script should be made executable
		for (std::list<std::string>::iterator iter = proxyscripts.begin(); iter != proxyscripts.end(); iter++){
			this->log("re-activating " + *iter, Logger::EVENT);
			chmod((this->env->cfg_dir+"/"+(*iter)).c_str(), 0755);
		}

		//remove the DS_ prefix  (DS_10_foo -> 10_foo)
		for (std::list<std::string>::iterator iter = dsfiles.begin(); iter != dsfiles.end(); iter++) {
			this->log("renaming " + *iter, Logger::EVENT);
			std::string newPath = this->env->cfg_dir+"/"+iter->substr(3);
			assert_filepath_empty(newPath, __FILE__, __LINE__);
			rename((this->env->cfg_dir+"/"+(*iter)).c_str(), newPath.c_str());
		}

		//remove the PS_ prefix and add index prefix (PS_foo -> 10_foo)
		int i = 20; //prefix
		for (std::list<std::string>::iterator iter = psfiles.begin(); iter != psfiles.end(); iter++) {
			this->log("renaming " + *iter, Logger::EVENT);
			std::string out = *iter;
			out.replace(0, 2, (std::string("") + char('0' + (i/10)%10) + char('0' + i%10)));
			std::string newPath = this->env->cfg_dir+"/"+out;
			assert_filepath_empty(newPath, __FILE__, __LINE__);
			rename((this->env->cfg_dir+"/"+(*iter)).c_str(), newPath.c_str());
			i++;
		}
	}
}

void Model_ListCfg::addColorHelper() {
	Model_Script* newScript = NULL;
	if (this->repository.getScriptByName("grub-customizer_menu_color_helper") == NULL) {
		Model_Script* newScript = this->repository.createScript("grub-customizer_menu_color_helper", this->env->cfg_dir + "06_grub-customizer_menu_color_helper", "#!/bin/sh\n\
\n\
if [ \"x${GRUB_BACKGROUND}\" != \"x\" ] ; then\n\
	if [ \"x${GRUB_COLOR_NORMAL}\" != \"x\" ] ; then\n\
	echo \"set color_normal=${GRUB_COLOR_NORMAL}\"\n\
	fi\n\
\n\
	if [ \"x${GRUB_COLOR_HIGHLIGHT}\" != \"x\" ] ; then\n\
	echo \"set color_highlight=${GRUB_COLOR_HIGHLIGHT}\"\n\
	fi\n\
fi\n\
");
		assert(newScript != NULL);
		Model_Proxy newProxy(*newScript);
		newProxy.index = 6;
		this->proxies.push_back(newProxy);
	}
}

std::list<Model_Rule> Model_ListCfg::getRemovedEntries(Model_Entry* parent, bool ignorePlaceholders) {
	std::list<Model_Rule> result;
	if (parent == NULL) {
		for (std::list<Model_Script>::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++) {
			std::list<Model_Rule> subResult = this->getRemovedEntries(&iter->root, ignorePlaceholders);
			result.insert(result.end(), subResult.begin(), subResult.end());
		}
	} else {
		if (parent->type == Model_Entry::SUBMENU || parent->type == Model_Entry::SCRIPT_ROOT) {
			for (std::list<Model_Entry>::iterator entryIter = parent->subEntries.begin(); entryIter != parent->subEntries.end(); entryIter++) {
				std::list<Model_Rule> subResult = this->getRemovedEntries(&*entryIter, ignorePlaceholders);
				Model_Rule* currentSubmenu = NULL;
				if (subResult.size()) {
					Model_Rule submenu(Model_Rule::SUBMENU, std::list<std::string>(), entryIter->name, true);
					submenu.subRules = subResult;
					submenu.dataSource = &*entryIter;
					result.push_back(submenu);
					currentSubmenu = &result.back();
				}

				if ((entryIter->type == Model_Entry::MENUENTRY || !ignorePlaceholders) && !this->proxies.getVisibleRuleForEntry(*entryIter)) {
					Model_Rule::RuleType ruleType = Model_Rule::NORMAL;
					switch (entryIter->type) {
					case Model_Entry::MENUENTRY:
						ruleType = Model_Rule::NORMAL;
						break;
					case Model_Entry::PLAINTEXT:
						ruleType = Model_Rule::PLAINTEXT;
						break;
					case Model_Entry::SUBMENU:
						ruleType = Model_Rule::OTHER_ENTRIES_PLACEHOLDER;
						break;
					}
					Model_Rule newRule(ruleType, std::list<std::string>(), entryIter->name, true);
					newRule.dataSource = &*entryIter;
					if (currentSubmenu) {
						currentSubmenu->subRules.push_front(newRule);
					} else {
						result.push_back(newRule);
					}
				}
			}
		}
	}
	return result;
}

Model_Rule* Model_ListCfg::addEntry(Model_Entry& entry, bool insertAsOtherEntriesPlaceholder) {
	Model_Script* sourceScript = this->repository.getScriptByEntry(entry);
	assert(sourceScript != NULL);

	Model_Proxy* targetProxy = NULL;
	if (this->proxies.size() && this->proxies.back().dataSource == sourceScript) {
		targetProxy = &this->proxies.back();
		targetProxy->set_isExecutable(true);
	} else {
		this->proxies.push_back(Model_Proxy(*sourceScript, false));
		targetProxy = &this->proxies.back();
		this->renumerate();
	}

	Model_Rule rule;
	if (insertAsOtherEntriesPlaceholder) {
		rule = Model_Rule(Model_Rule::OTHER_ENTRIES_PLACEHOLDER, sourceScript->buildPath(entry), true);
		rule.dataSource = &entry;
	} else {
		rule = Model_Rule(entry, true, *sourceScript, std::list<std::list<std::string> >(), sourceScript->buildPath(entry));
	}

	targetProxy->removeEquivalentRules(rule);
	targetProxy->rules.push_back(rule);
	return &targetProxy->rules.back();
}

/**
 * deletes an entry and its rules
 */
void Model_ListCfg::deleteEntry(Model_Entry const& entry) {
	for (std::list<Model_Proxy>::iterator proxyIter = this->proxies.begin(); proxyIter != this->proxies.end(); proxyIter++) {
		Model_Rule* rule = proxyIter->getRuleByEntry(entry, proxyIter->rules, Model_Rule::NORMAL);
		if (rule) {
			proxyIter->removeRule(rule);
		}
	}
	this->repository.getScriptByEntry(entry)->deleteEntry(entry);
}

/**
 * sorts the gives rules so that it matches the real order
 */
std::list<Rule*> Model_ListCfg::getNormalizedRuleOrder(std::list<Rule*> rules) {
	if (rules.size() == 0 || rules.size() == 1) {
		return rules;
	}
	std::list<Rule*> result;

	Model_Rule* firstRuleOfList = &Model_Rule::fromPtr(rules.front());
	std::list<Model_Rule>::iterator currentRule;

	Model_Rule* parentRule = this->proxies.getProxyByRule(firstRuleOfList)->getParentRule(firstRuleOfList);
	if (parentRule) {
		currentRule = parentRule->subRules.begin();
	} else {
		currentRule = this->proxies.front().rules.begin();
	}

	try {
		while (true) {
			for (std::list<Rule*>::iterator iter = rules.begin(); iter != rules.end(); iter++) {
				if (&*currentRule == *iter) {
					result.push_back(*iter);
					break;
				}
			}
			currentRule = this->proxies.getNextVisibleRule(currentRule, 1);
		}
	} catch (NoMoveTargetException const& e) {
		// loop finished
	}

	return result;
}

std::list<Model_Script*> Model_ListCfg::getProxifiedScripts() {
	std::list<Model_Script*> result;

	for (std::list<Model_Script>::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++) {
		if (this->proxies.proxyRequired(*iter)) {
			result.push_back(&*iter);
		}
	}

	return result;
}

void Model_ListCfg::generateScriptSourceMap() {
	std::map<std::string, int> defaultScripts; // only for non-static scripts - so 40_custom is ignored
	defaultScripts["header"]                            =  0;
	defaultScripts["debian_theme"]                      =  5;
	defaultScripts["grub-customizer_menu_color_helper"] =  6;
	defaultScripts["linux"]                             = 10;
	defaultScripts["linux_xen"]                         = 20;
	defaultScripts["memtest86+"]                        = 20;
	defaultScripts["os-prober"]                         = 30;
	defaultScripts["custom"]                            = 41;

	std::string proxyfiedScriptPath = this->env->cfg_dir + "/proxifiedScripts";

	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		std::string currentPath = scriptIter->fileName;
		std::string defaultPath;
		int pos = -1;

		if (scriptIter->isCustomScript) {
			defaultPath = this->env->cfg_dir + "/40_custom";
		} else if (defaultScripts.find(scriptIter->name) != defaultScripts.end()) {
			pos = defaultScripts[scriptIter->name];
			std::ostringstream str;
			str << this->env->cfg_dir << "/" << std::setw(2) << std::setfill('0') << pos << "_" << scriptIter->name;
			defaultPath = str.str();
		}

		if (defaultPath != "" && defaultPath != currentPath) {
			this->scriptSourceMap[defaultPath] = currentPath;
		}
	}
}

void Model_ListCfg::populateScriptSourceMap() {
	std::string proxyfiedScriptPath = this->env->cfg_dir + "/proxifiedScripts";
	for (std::list<Model_Script>::iterator scriptIter = this->repository.begin(); scriptIter != this->repository.end(); scriptIter++) {
		if (scriptIter->fileName.substr(0, proxyfiedScriptPath.length()) != proxyfiedScriptPath
				&& this->scriptSourceMap.getSourceName(scriptIter->fileName) == "") {
			this->scriptSourceMap.addScript(scriptIter->fileName);
		}
	}
}

bool Model_ListCfg::hasScriptUpdates() const {
	return this->scriptSourceMap.getUpdates().size() > 0;
}

void Model_ListCfg::applyScriptUpdates() {
	std::list<std::string> newScriptPathes = this->scriptSourceMap.getUpdates();
	for (std::list<std::string>::iterator newScriptPathIter = newScriptPathes.begin(); newScriptPathIter != newScriptPathes.end(); newScriptPathIter++) {
		std::string oldScriptPath = this->scriptSourceMap[*newScriptPathIter];
		Model_Script* oldScript = this->repository.getScriptByFilename(oldScriptPath);
		Model_Script* newScript = this->repository.getScriptByFilename(*newScriptPathIter);
		if (!oldScript || !newScript) {
			this->log("applyScriptUpdates failed for " + oldScriptPath + " (" + *newScriptPathIter + ")", Logger::ERROR);
			continue;
		}

		// unsync proxies of newScript
		std::list<Model_Proxy*> newProxies = this->proxies.getProxiesByScript(*newScript);
		for (std::list<Model_Proxy*>::iterator newProxyIter = newProxies.begin(); newProxyIter != newProxies.end(); newProxyIter++) {
			(*newProxyIter)->unsync();
			this->proxies.deleteProxy(&**newProxyIter);
		}

		// copy entries of custom scripts
		if (oldScript->isCustomScript && newScript->isCustomScript && oldScript->entries().size()) {
			for (std::list<Model_Entry>::iterator entryIter = oldScript->entries().begin(); entryIter != oldScript->entries().end(); entryIter++) {
				if (entryIter->type == Model_Entry::PLAINTEXT && newScript->getPlaintextEntry()) {
					newScript->getPlaintextEntry()->content = entryIter->content; // copy plaintext instead of adding another entry
					newScript->getPlaintextEntry()->isModified = true;
				} else {
					newScript->entries().push_back(*entryIter);
					newScript->entries().back().isModified = true;
				}
			}
		}

		// connect proxies of oldScript with newScript, resync
		std::list<Model_Proxy*> oldProxies = this->proxies.getProxiesByScript(*oldScript);
		for (std::list<Model_Proxy*>::iterator oldProxyIter = oldProxies.begin(); oldProxyIter != oldProxies.end(); oldProxyIter++) {
			// connect old proxy to new script
			(*oldProxyIter)->unsync();
			(*oldProxyIter)->dataSource = newScript;
			if ((*oldProxyIter)->fileName == oldScript->fileName) {
				(*oldProxyIter)->fileName = newScript->fileName; // set the new fileName
			}
		}

		std::list<Model_Rule*> foreignRules = this->proxies.getForeignRules();

		for (std::list<Model_Rule*>::iterator ruleIter = foreignRules.begin(); ruleIter != foreignRules.end(); ruleIter++) {
			if (this->repository.getScriptByEntry(*(*ruleIter)->dataSource) == oldScript) {
				(*ruleIter)->__sourceScriptPath = newScript->fileName;
			}
		}

		this->repository.removeScript(*oldScript);
	}
	this->scriptSourceMap.deleteUpdates();

	this->proxies.unsync_all();
	this->proxies.sync_all(true, true, NULL, this->repository.getScriptPathMap());
}

void Model_ListCfg::revert() {
	int remaining = this->proxies.size();
	while (remaining) {
		this->proxies.deleteProxy(&this->proxies.front());
		assert(this->proxies.size() < remaining); // make sure that the proxy has really been deleted to prevent an endless loop
		remaining = this->proxies.size();
	}
	std::list<std::string> usedIndices;
	int i = 50; // unknown scripts starting at position 50
	for (std::list<Model_Script>::iterator iter = this->repository.begin(); iter != this->repository.end(); iter++) {
		Model_Proxy newProxy(*iter);
		std::string sourceFileName = this->scriptSourceMap.getSourceName(iter->fileName);
		try {
			newProxy.index = Model_Script::extractIndexFromPath(sourceFileName, this->env->cfg_dir);
		} catch (InvalidStringFormatException const& e) {
			newProxy.index = i++;
			this->log(e, Logger::ERROR);
		}

		// avoid duplicates
		std::ostringstream uniqueIndex;
		uniqueIndex << newProxy.index << iter->name;

		if (std::find(usedIndices.begin(), usedIndices.end(), uniqueIndex.str()) != usedIndices.end()) {
			newProxy.index = i++;
		}

		usedIndices.push_back(uniqueIndex.str());

		this->proxies.push_back(newProxy);
	}
	this->proxies.sort();
}

Model_ListCfg::operator ArrayStructure() const {
	ArrayStructure result;
	result["eventListener"] = this->controller;
	result["proxies"] = ArrayStructure(this->proxies);
	result["repository"] = ArrayStructure(this->repository);
	result["progress"] = this->progress;
	result["progress_name"] = this->progress_name;
	result["progress_pos"] = this->progress_pos;
	result["progress_max"] = this->progress_max;
	result["mutex"] = this->mutex;
	result["errorLogFile"] = this->errorLogFile;
	result["verbose"] = this->verbose;
	result["error_proxy_not_found"] = this->error_proxy_not_found;
	if (this->env) {
		result["env"] = ArrayStructure(*this->env);
	} else {
		result["env"] = ArrayStructureItem(NULL);
	}
	result["ignoreLock"] = this->ignoreLock;
	result["cancelThreadsRequested"] = this->cancelThreadsRequested;
	return result;
}






