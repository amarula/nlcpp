# Revision history for nlcpp

## 0.1.1 -- 2025-09-17

* Methods to send netlink messages and wait for response, with helper class to construct messages
* Added methods to use multicast groups
* Provide error code from Exception object
* Added initial interface for ctrl generic netlink family
* Added `RouteCache` wrapper class for route cache
* Added scope and label accessors to the `RouteAddress` class
* Added protocol, family and priority accessors to the `Route` class
* Added family accessors to `Address` class
* Added methods wrapping `GET_INTERFACE` command in nl80211 generic netlink family

## 0.1.0 -- 2025-08-29

* First version with wrappers for netlink sockets and initial support for
  addresses, links and routes, mainly using the route netlink protocol.
