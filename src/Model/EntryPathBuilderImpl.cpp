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

#include "EntryPathBuilderImpl.h"

Model_EntryPathBuilderImpl::Model_EntryPathBuilderImpl(Model_Script const& mainScript)
	: prefixLength(0), mainScript(NULL)
{
	this->setMainScript(mainScript);
}

void Model_EntryPathBuilderImpl::setMainScript(Model_Script const& mainScript) {
	this->mainScript = &mainScript;
}

void Model_EntryPathBuilderImpl::setEntrySourceMap(std::map<Model_Entry const*, Model_Script const*> const& entrySourceMap) {
	this->entrySourceMap = entrySourceMap;
}
void Model_EntryPathBuilderImpl::setScriptTargetMap(std::map<Model_Script const*, std::string> const& scriptTargetMap) {
	this->scriptTargetMap = scriptTargetMap;
}

void Model_EntryPathBuilderImpl::setPrefixLength(int length) {
	this->prefixLength = length;
}

std::list<std::string> Model_EntryPathBuilderImpl::buildPath(Model_Entry const& entry) const {
	Model_Script const* script = entrySourceMap.find(&entry) != entrySourceMap.end() ? entrySourceMap.find(&entry)->second : this->mainScript;
	return script->buildPath(entry);
}
std::string Model_EntryPathBuilderImpl::buildPathString(Model_Entry const& entry, bool withOtherEntriesPlaceholder) const {
	Model_Script const* script = entrySourceMap.find(&entry) != entrySourceMap.end() ? entrySourceMap.find(&entry)->second : this->mainScript;
	return script->buildPathString(entry, withOtherEntriesPlaceholder);
}

std::string Model_EntryPathBuilderImpl::buildScriptPath(Model_Entry const& entry) const {
	Model_Script const* script = entrySourceMap.find(&entry) != entrySourceMap.end() ? entrySourceMap.find(&entry)->second : NULL;
	return script ? this->scriptTargetMap.find(script)->second.substr(this->prefixLength) : "";
}
