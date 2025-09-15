#pragma once

#include <nlcpp/generic.h>

#include <optional>
#include <string>
#include <vector>

namespace nl {

using std::optional;
using std::string;
using std::vector;

struct MulticastGroup
{
	string name;
	int id;
};

struct GenericFamily
{
	string name;
	int id;
	vector<MulticastGroup> multicastGroups;
};

class GenericSocket::Ctrl
{
public:
	explicit Ctrl(GenericSocket & genl);

	/// Get generic netlink family name
	const string & familyName() const;
	/// Get generic netlink family id
	int familyId() const;

	optional<GenericFamily> getFamily(const string & name);
	vector<GenericFamily> getFamilies();

private:
	static void parseGenericFamily(GenericFamily &, const GenericNetlinkMessage &);

	GenericSocket & genl;
};

}
