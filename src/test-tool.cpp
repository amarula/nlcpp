#include <nlcpp.h>

#include <sys/socket.h>

#include <functional>
#include <iostream>
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
using std::vector;

using namespace nl;

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
	rsocket.add(addr);

	printLine("address-add-done");
}

void addressDel(const vector<string> & args)
{
	RouteCacheManager mngr;
	auto rcache = mngr.addressCache();

	RouteAddress addr;
	parseAddressArgs(mngr.linkCache(), addr, args);

	nl::RouteSocket rsocket;
	rsocket.del(addr);

	printLine("address-del-done");
}

vector<Command> commands = {
	{ "link-list", linkList },
	{ "address-list", addressList },
	{ "address-add", addressAdd },
	{ "address-del", addressDel },
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

	while (getline(&line, &size, stdin) > 0) {
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

	free(line);
	return 0;
}
