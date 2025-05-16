#include <nlcpp/generic.h>

#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>

namespace nl {

GenericSocket::GenericSocket():
	Socket(NETLINK_GENERIC)
{}

int GenericSocket::resolve(const string & name)
{
	int res = genl_ctrl_resolve(sock, name.c_str());
	if (res < 0)
		Exception::throwCode("genl_ctrl_resolve failed", res);
	return res;
}

int GenericSocket::resolveGroup(const string & family, const string & group)
{
	int res = genl_ctrl_resolve_grp(sock, family.c_str(), group.c_str());
	if (res < 0)
		Exception::throwCode("genl_ctrl_resolve_grp failed", res);
	return res;
}

}
