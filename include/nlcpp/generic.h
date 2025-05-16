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
};

}
