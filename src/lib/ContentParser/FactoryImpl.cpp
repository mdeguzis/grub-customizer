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
#include "FactoryImpl.h"

void ContentParser_FactoryImpl::registerParser(ContentParser& parser, std::string const& name) {
	assert(this->parsers.size() == this->names.size());
	this->parsers.push_back(&parser);
	this->names.push_back(name);
}

ContentParser* ContentParser_FactoryImpl::create(std::string const& sourceCode) {
	for (std::list<ContentParser*>::iterator iter = this->parsers.begin(); iter != this->parsers.end(); iter++) {
		try {
			(*iter)->parse(sourceCode);
			return *iter;
		} catch (ParserException const& e) {
			continue;
		}
	}
	throw ParserNotFoundException("no matching parser found", __FILE__, __LINE__);
}


ContentParser* ContentParser_FactoryImpl::createByName(std::string const& name) {
	assert(this->parsers.size() == this->names.size());

	std::list<std::string>::iterator namesIter = this->names.begin();
	for (std::list<ContentParser*>::iterator parsersIter = this->parsers.begin(); parsersIter != this->parsers.end(); parsersIter++) {
		if (name == *namesIter) {
			return *parsersIter;
		}
		namesIter++;
	}
	throw ItemNotFoundException("no parser found by name '" + name + "'", __FILE__, __LINE__);
}

std::list<std::string> const& ContentParser_FactoryImpl::getNames() const {
	return this->names;
}

std::string ContentParser_FactoryImpl::getNameByInstance(ContentParser const& instance) const {
	assert(this->parsers.size() == this->names.size());

	std::list<std::string>::const_iterator namesIter = this->names.begin();
	for (std::list<ContentParser*>::const_iterator parsersIter = this->parsers.begin(); parsersIter != this->parsers.end(); parsersIter++) {
		if (&instance == *parsersIter) {
			return *namesIter;
		}
		namesIter++;
	}
	throw ItemNotFoundException("no parser found by instance pointer", __FILE__, __LINE__);
}
