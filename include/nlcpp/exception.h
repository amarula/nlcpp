/** @file
 *  @brief Classes representing netlink-specific exceptions
 */

#pragma once

#include <exception>
#include <string>

namespace nl {

using std::string;

/// Base exception class for netlink errors
class Exception : public std::exception
{
protected:
	Exception(const string & prefix, int code);
public:
	const char * what() const noexcept;

	static void throwCode(const string & prefix, int code);

private:
	string msg;
};

class AttributeNotAvailable : public Exception
{
public:
	AttributeNotAvailable(const string & prefix);
};

class MissingAttribute : public Exception
{
public:
	MissingAttribute(const string & prefix);
};

class NoAddress : public Exception
{
public:
	NoAddress(const string & prefix);
};

}
