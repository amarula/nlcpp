/** @file
 *  @brief Generic netlink support
 */

#pragma once

#include <nlcpp/netlink.h>

namespace nl {

/// Socket opened for the generic netlink protocol
class GenericSocket : protected Socket
{
public:
	GenericSocket();

	int resolve(const string & name);
	int resolveGroup(const string & family, const string & group);

	class Nl80211;
	/// Interface for nl80211 netlink family. Returned object can be used
	/// only as long as this GenericSocket object exists.
	Nl80211 nl80211();
};

}
