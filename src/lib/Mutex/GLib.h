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

#ifndef GLIBMUTEX_H_
#define GLIBMUTEX_H_
#include "../Mutex.h"
#include <glibmm/thread.h>
#include "../Trait/LoggerAware.h"

class Mutex_GLib : public Mutex, public Trait_LoggerAware {
protected:
	Glib::Mutex mutex;
public:
	void lock();
	bool trylock();
	void unlock();
};

#endif /* GLIBMUTEX_H_ */
