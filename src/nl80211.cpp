#include <nlcpp/nl80211.h>

#include <linux/nl80211.h>

namespace nl {

GenericSocket::Nl80211 GenericSocket::nl80211()
{
	return Nl80211(*this);
}

GenericSocket::Nl80211::Nl80211(GenericSocket & genl_)
	: genl(genl_)
	, family(genl_.resolve(NL80211_GENL_NAME))
{
}

}
