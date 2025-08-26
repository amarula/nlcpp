/** @file
 *  @brief Nl80211 generic netlink family
 */

#pragma once

#include <nlcpp/generic.h>

namespace nl {

class GenericSocket::Nl80211
{
public:
	explicit Nl80211(GenericSocket & genl);

private:
	GenericSocket & genl;
	const int family;
};

}
