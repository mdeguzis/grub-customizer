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

#ifndef CONTENTPARSERFACTORY_H_
#define CONTENTPARSERFACTORY_H_
#include "ContentParser.h"
#include <list>
#include <string>
#include "Exception.h"

class ContentParserFactory {
public:
	virtual inline ~ContentParserFactory() {};

	virtual ContentParser* create(std::string const& sourceCode) = 0;
	virtual ContentParser* createByName(std::string const& name) = 0;
	virtual std::list<std::string> const& getNames() const = 0;
	virtual std::string getNameByInstance(ContentParser const& instance) const = 0;
};

class ContentParserFactory_Connection {
protected:
	ContentParserFactory* contentParserFactory;
public:
	ContentParserFactory_Connection() : contentParserFactory(NULL) {}

	void setContentParserFactory(ContentParserFactory& contentParserFactory) {
		this->contentParserFactory = &contentParserFactory;
	}
};

#endif /* CONTENTPARSERFACTORY_H_ */
