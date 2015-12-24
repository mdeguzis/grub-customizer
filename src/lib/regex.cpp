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

#include "regex.h"

std::vector<std::string> Regex::match(std::string const& pattern, std::string const& str){
	std::vector<std::string> result;
	GMatchInfo* mi = NULL;
	GRegex* gr = g_regex_new(pattern.c_str(), GRegexCompileFlags(0), GRegexMatchFlags(0), NULL);
	bool success = g_regex_match(gr, str.c_str(), GRegexMatchFlags(0), &mi);
	if (!success)
		throw RegExNotMatchedException("RegEx doesn't match", __FILE__, __LINE__);

	gint match_count = g_match_info_get_match_count(mi);
	for (gint i = 0; i < match_count; i++){
		gchar* matched_string = g_match_info_fetch(mi, i);
		result.push_back(std::string(matched_string));
		delete matched_string;
	}

	g_match_info_free(mi);
	g_regex_unref(gr);
	return result;
}
std::string Regex::replace(std::string const& pattern, std::string const& str, std::map<int, std::string> const& newValues){
	std::string result = str;
	GMatchInfo* mi = NULL;
	GRegex* gr = g_regex_new(pattern.c_str(), GRegexCompileFlags(0), GRegexMatchFlags(0), NULL);
	bool success = g_regex_match(gr, str.c_str(), GRegexMatchFlags(0), &mi);
	if (!success)
		throw RegExNotMatchedException("RegEx doesn't match", __FILE__, __LINE__);

	gint match_count = g_match_info_get_match_count(mi);
	gint offset = 0;
	for (std::map<int, std::string>::const_iterator iter = newValues.begin(); iter != newValues.end(); iter++){
		gint start_pos, end_pos;
		g_match_info_fetch_pos(mi, iter->first, &start_pos, &end_pos);
		if (start_pos != -1 && end_pos != -1) { //ignore unmatched (optional) values
			result.replace(start_pos+offset, end_pos-start_pos, iter->second);
			offset += iter->second.length() - (end_pos-start_pos);
		}
	}

	g_match_info_free(mi);
	g_regex_unref(gr);
	return result;
}
