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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>
#include <sstream>
#include <typeinfo>

class Exception {
	std::string _message;
	std::string _file;
	int _line;
public:
	Exception(std::string const& message, std::string const& file = "", int line = -1);
	virtual inline ~Exception(){}
	virtual operator std::string() const;
	virtual std::string getMessage() const;
	virtual operator bool() const;
};

class LogicException : public Exception {
	public: inline LogicException(std::string const& message, std::string const& file = "", int line = -1)
	   : Exception(message, file, line) {}
};

class HandleNotOpenedException : public LogicException {
	public: inline HandleNotOpenedException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class HandleNotClosedException : public LogicException {
	public: inline HandleNotClosedException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class EndOfFileException : public LogicException {
	public: inline EndOfFileException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class MustBeProxyException : public LogicException {
	public: inline MustBeProxyException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class ItemNotFoundException : public LogicException {
	public: inline ItemNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class ParserException : public LogicException {
	public: inline ParserException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class RegExNotMatchedException : public LogicException {
	public: inline RegExNotMatchedException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class NoMoveTargetException : public LogicException {
	public: inline NoMoveTargetException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class ParserNotFoundException : public ItemNotFoundException {
	public: inline ParserNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : ItemNotFoundException(message, file, line) {}
};

class MountpointNotFoundException : public ItemNotFoundException {
	public: inline MountpointNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : ItemNotFoundException(message, file, line) {}
};

class TechnicalException : public Exception {
	public: inline TechnicalException(std::string const& message, std::string const& file = "", int line = -1)
	   : Exception(message, file, line) {}
};

class AssertException : public TechnicalException {
	public: inline AssertException(std::string const& message, std::string const& file = "", int line = -1)
	   : TechnicalException(message, file, line) {}
};

class NullPointerException : public TechnicalException {
	public: inline NullPointerException(std::string const& message, std::string const& file = "", int line = -1)
	   : TechnicalException(message, file, line) {}
};

class ConfigException : public TechnicalException {
	public: inline ConfigException(std::string const& message, std::string const& file = "", int line = -1)
	   : TechnicalException(message, file, line) {}
};

class SystemException : public TechnicalException {
	public: inline SystemException(std::string const& message, std::string const& file = "", int line = -1)
	   : TechnicalException(message, file, line) {}
};

class FileReadException : public SystemException {
	public: inline FileReadException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class FileSaveException : public SystemException {
	public: inline FileSaveException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class DirectoryNotFoundException : public SystemException {
	public: inline DirectoryNotFoundException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class CmdExecException : public SystemException {
	public: inline CmdExecException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class MountException : public SystemException {
	public: inline MountException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class UMountException : public SystemException {
	public: inline UMountException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class MissingFstabException : public SystemException {
	public: inline MissingFstabException(std::string const& message, std::string const& file = "", int line = -1)
	   : SystemException(message, file, line) {}
};

class BadCastException : public TechnicalException {
public: inline BadCastException(std::string const& message, std::string const& file = "", int line = -1)
   : TechnicalException(message, file, line) {}
};

class InvalidStringFormatException : public LogicException {
	public: inline InvalidStringFormatException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class InvalidFileTypeException : public LogicException {
	public: inline InvalidFileTypeException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

class NotImplementedException : public LogicException {
	public: inline NotImplementedException(std::string const& message, std::string const& file = "", int line = -1)
	   : LogicException(message, file, line) {}
};

// Template:
//class CLASSNAME : public BASECLASS {
//	public: inline CLASSNAME(std::string const& message, std::string const& file = "", int line = -1)
//	   : BASECLASS(message, file, line) {}
//};


#endif /* EXCEPTION_H_ */
