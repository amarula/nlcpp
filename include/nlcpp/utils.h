#pragma once

#include <string>
#include <vector>

namespace nl
{

using std::string;
using std::vector;

template<class T>
class GenericFlags
{
public:
	GenericFlags(unsigned int value_): value(value_) {}
	GenericFlags(string);
	GenericFlags(std::initializer_list<GenericFlags>);

	operator unsigned int() const { return value; }
	operator string() const;
	operator vector<unsigned int>() const;
	operator vector<string>() const;

	GenericFlags operator&(GenericFlags other) const
	{ return GenericFlags(value & other.value); }
	GenericFlags operator&(unsigned int other) const
	{ return GenericFlags(value & other); }

	GenericFlags operator|(GenericFlags other) const
	{ return GenericFlags(value | other.value); }
	GenericFlags operator|(unsigned int other) const
	{ return GenericFlags(value | other); }

	GenericFlags operator^(GenericFlags other) const
	{ return GenericFlags(value ^ other.value); }
	GenericFlags operator^(unsigned int other) const
	{ return GenericFlags(value ^ other); }

	GenericFlags operator~() const { return GenericFlags(~value); }

	GenericFlags operator&=(GenericFlags other)
	{ value &= other.value; return *this; }
	GenericFlags operator&=(unsigned int other)
	{ value &= other; return *this; }

	GenericFlags operator|=(GenericFlags other)
	{ value |= other.value; return *this; }
	GenericFlags operator|=(unsigned int other)
	{ value |= other; return *this; }

private:
	unsigned int value;
};

}
