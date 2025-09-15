/** @file
 *  @brief Nl80211 generic netlink family
 */

#pragma once

#include <nlcpp/generic.h>

#include <linux/if_ether.h>

#include <array>
#include <optional>
#include <vector>

namespace nl {

using std::array;
using std::optional;
using std::vector;

namespace nl80211 {

enum class InterfaceType
{
	UNSPECIFIED,
	ADHOC,
	STATION,
	AP,
	AP_VLAN,
	WDS,
	MONITOR,
	MESH_POINT,
	P2P_CLIENT,
	P2P_GO,
	P2P_DEVICE,
	OCB,
	NAN_,
};

struct InterfaceConfiguration
{
	uint32_t ifindex;
	uint32_t wiphy;
	InterfaceType iftype;
	array<uint8_t, ETH_ALEN> mac;
	bool use4addr;
};

}

class GenericSocket::Nl80211
{
public:
	explicit Nl80211(GenericSocket & genl);

	/// Get generic netlink family name
	const string & familyName() const;
	/// Get generic netlink family id
	int familyId() const;

	optional<nl80211::InterfaceConfiguration> getInterface(uint32_t ifindex);
	vector<nl80211::InterfaceConfiguration> getInterfaces();

private:
	void parseInterfaceConfiguration(nl80211::InterfaceConfiguration &, const GenericNetlinkMessage &) const;

	GenericSocket & genl;
	const int family;
};

}
