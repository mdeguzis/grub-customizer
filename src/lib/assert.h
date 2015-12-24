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

#ifndef ASSERT_H_
#define ASSERT_H_

/**
 * a custom assert implementation throwing exceptions instead of aborting directly
 */

#include "Exception.h"
#include <cstdio>

# define ASSERT_VOID_CAST static_cast<void>

void assert_fail(std::string const& expr, std::string const& file, int line, std::string const& func);

# define assert(expr)							\
  ((expr)								\
   ? ASSERT_VOID_CAST (0)						\
   : assert_fail (__STRING(expr), __FILE__, __LINE__, __func__))

void assert_filepath_empty(std::string const& filepath, std::string const& sourceCodeFile, int line);

#endif /* ASSERT_H_ */
