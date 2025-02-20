NlCpp
=====

C++ wrapper around the [libnl library](https://www.infradead.org/~tgr/libnl/);
main focus of this library is:

* Provide API suitable to use with modern C++.

* Add type information where reasonably possible.

* Encapsulate handling of object lifetime in a way compatible with RAII style.


Examples
--------

List all available network interfaces:

```cpp
nl::RouteCacheManager mngr;
auto rcache = mngr.linkCache();

for (const auto & link : rcache) {
    std::cout << link.ifindex() << ": " << link.name().value() << std::endl;
}
```

Remove all IPv4 addresses from interface with index 2:

```cpp
nl::RouteSocket rsocket;
rsocket.del(nl::RouteAddress()
    .ifindex(2)
    .family(AF_INET)
);
```

Add a new IPv4 address:

```cpp
rsocket.add(nl::RouteAddress()
    .ifindex(2)
    .local(nl::Address("192.168.0.2/24"))
);
```

Add a default route:

```cpp
rsocket.add(nl::Route()
    .dst(nl::Address("0.0.0.0/0"))
    .add(nl::NextHop()
        .ifindex(2)
        .gateway(nl::Address("192.168.0.1"))
    )
);
```
