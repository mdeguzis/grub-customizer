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

#ifndef CONTENTPARSERFACTORYIMPL_H_
#define CONTENTPARSERFACTORYIMPL_H_
#include "../../lib/ContentParserFactory.h"
#include "../../lib/ContentParser.h"
#include <list>
#include "../../lib/assert.h"
#include "../Exception.h"

class ContentParser_FactoryImpl : public ContentParserFactory {
	std::list<ContentParser*> parsers;
	std::list<std::string> names;
public:
	void registerParser(ContentParser& parser, std::string const& name);
	ContentParser* create(std::string const& sourceCode);
	ContentParser* createByName(std::string const& name);
	std::list<std::string> const& getNames() const;
	std::string getNameByInstance(ContentParser const& instance) const;
};

#endif /* CONTENTPARSERFACTORYIMPL_H_ */
