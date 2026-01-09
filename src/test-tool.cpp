#include <nlcpp.h>

#include <poll.h>
#include <string.h>
#include <sys/socket.h>

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::function;
using std::move;
using std::mutex;
using std::ostringstream;
using std::scoped_lock;
using std::string;
using std::unique_ptr;
using std::vector;

using namespace nl;

vector<pollfd> pollFds {
	{ 0, POLLIN, 0 },
};

vector<unique_ptr<CacheManager>> managers {};
vector<unique_ptr<Cache>> caches {};

namespace {

mutex outputMutex;
void printLine(const string & line)
{
	scoped_lock lock(outputMutex);
	cout << line << endl;
}

struct Command
{
	string name;
	function<void(const vector<string> &)> action;
};

void parseLinkArgs(RouteLink & link, const vector<string> & args)
{
	for (size_t i = 0; i < args.size(); i++) {
		if (args[i] == "name" && i + 1 < args.size())
			link.name(args[++i]);

		else if (args[i] == "ifindex" && i + 1 < args.size())
			link.ifindex(stoi(args[++i]));

		else if (args[i] == "type" && i + 1 < args.size())
			link.type(args[++i]);

		else {
			cerr << "unexpected link argument: " << args[i] << endl;
			exit(1);
		}
	}
}

void linkList(const vector<string> &)
{
	RouteCacheManager mngr;
	auto rcache = mngr.linkCache();

	for (const auto & link : rcache) {
		ostringstream ss;
		ss << "link-list-item " << link.ifindex();
		if (const auto name = link.name())
			ss << " " << name.value();
		printLine(ss.str());
	}
	printLine("link-list-done");
}

void linkAdd(const vector<string> & args)
{
	RouteLink link;
	parseLinkArgs(link, args);

	nl::RouteSocket rsocket;
	auto res = rsocket.add(link);

	ostringstream ss;
	ss << "link-add-done " << (res ? "true" : "false");
	printLine(ss.str());
}

void parseAddressArgs(const RouteLinkCache & linkCache, RouteAddress & addr, const vector<string> & args)
{
	for (size_t i = 0; i < args.size(); i++) {
		if (args[i] == "family" && i + 1 < args.size()) {
			const auto & family = args[i + 1];
			if (family == "inet")
				addr.family(AF_INET);
			else if (family == "inet6")
				addr.family(AF_INET6);
			i++;
		}

		else if (args[i] == "dev" && i + 1 < args.size()) {
			const auto & dev = args[i + 1];
			addr.ifindex(linkCache.getByName(dev).ifindex());
			i++;
		}

		else {
			addr.local(Address(args[i]));
		}
	}
}

void addressList(const vector<string> & args)
{
	RouteCacheManager mngr;
	auto rcache = mngr.addressCache();

	RouteAddress filter;
	parseAddressArgs(mngr.linkCache(), filter, args);

	for (const auto & addr : rcache.filter(move(filter))) {
		ostringstream ss;
		ss << "address-list-item " << string(addr.local());
		printLine(ss.str());
	}
	printLine("address-list-done");
}

void addressAdd(const vector<string> & args)
{
	RouteCacheManager mngr;
	auto rcache = mngr.addressCache();

	RouteAddress addr;
	parseAddressArgs(mngr.linkCache(), addr, args);

	nl::RouteSocket rsocket;
	auto res = rsocket.add(addr);

	ostringstream ss;
	ss << "address-add-done " << (res ? "true" : "false");
	printLine(ss.str());
}

void addressDel(const vector<string> & args)
{
	RouteCacheManager mngr;
	auto rcache = mngr.addressCache();

	RouteAddress addr;
	parseAddressArgs(mngr.linkCache(), addr, args);

	nl::RouteSocket rsocket;
	auto res = rsocket.del(addr);

	ostringstream ss;
	ss << "address-del-done " << (res ? "true" : "false");
	printLine(ss.str());
}

void addressWatch(const vector<string> & args)
{
	auto mngr = std::make_unique<RouteCacheManager>();
	auto cache = std::make_unique<RouteAddressCache>(mngr->addressCache());

	cache->watch([](const RouteAddress & addr, Action action) {
		ostringstream ss;
		ss << "address-watch-event ";

		switch (action)
		{
		case Action::New: ss << "new"; break;
		case Action::Delete: ss << "del"; break;
		default: ss << "unknown"; break;
		}

		ss << " " << string(addr.local());
		printLine(ss.str());
	});

	pollFds.emplace_back(mngr->getFd(), POLLIN, 0);
	managers.emplace_back(move(mngr));
	caches.emplace_back(move(cache));
	printLine("address-watch-done");
}

vector<Command> commands = {
	{ "link-list", linkList },
	{ "link-add", linkAdd },
	{ "address-list", addressList },
	{ "address-add", addressAdd },
	{ "address-del", addressDel },
	{ "address-watch", addressWatch },
};

}

int main(int argc, char * argv[])
{
	char * line = nullptr;
	size_t size = 0;

	if (argc > 1) {
		vector<string> args;
		for (int i = 2; i < argc; i++)
			args.emplace_back(argv[i]);

		for (const auto & cmd : commands) {
			if (cmd.name == argv[1]) {
				cmd.action(args);
				return 0;
			}
		}

		cerr << "Unknown command: '" << argv[1] << "'" << endl;
		return 1;
	}

	while (true) {
		int ready = poll(pollFds.data(), pollFds.size(), -1);
		if (ready < 0) {
			fprintf(stderr, "poll failed: %s", strerror(errno));
			return 1;
		}

		for (size_t i = 1; i < pollFds.size(); i++) {
			if (pollFds[i].revents)
				managers[i - 1]->dataReady();
		}

		if (pollFds[0].revents) {
			if (getline(&line, &size, stdin) <= 0)
				break;

			optional<string> command;
			vector<string> args;

			const char * last = line;
			for (const char * cur = line;; cur++) {
				if (isspace(*cur) || *cur == '\0') {
					if (last < cur) {
						if (!command)
							command.emplace(last, cur);
						else
							args.emplace_back(last, cur);
					}
					last = cur + 1;

					if (*cur == '\0')
						break;
				}
			}

			if (!command)
				continue;

			bool found = false;
			for (const auto & cmd : commands) {
				if (cmd.name == *command) {
					found = true;
					cmd.action(args);
					break;
				}
			}

			if (!found)
				cerr << "Unknown command: '" << *command << "'" << endl;
		}
	}

	free(line);
	return 0;
}
