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

#ifndef MUTEX_H_INCLUDED
#define MUTEX_H_INCLUDED
#include <cstdlib>

class Mutex {
public:
	virtual inline ~Mutex() {};

	virtual void lock() = 0;
	virtual bool trylock() = 0;
	virtual void unlock() = 0;
};

class Mutex_Connection {
protected:
	Mutex* mutex;
public:
	Mutex_Connection() : mutex(NULL) {}

	void setMutex(Mutex& mutex) {
		this->mutex = &mutex;
	}
};

#endif
