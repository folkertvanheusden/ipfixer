what it is
----------

ipfixer is a collector for IPFIX and NetFlow v9 data.
This data is captured and then stored in e.g. a
MondoDB database.


compiling
---------

The following package is required:

 * libyaml-cpp-dev

Optional packages:

 * libmongocxx-dev  at moment of writing (February
                    2022) this package is in Debian
                    in the 'experimental' branch)

 * libpqxx-dev      Postgres support
 * libjansson-dev   Required for Postgres


Then:

 * mkdir build
 * cd build
 * cmake ..
 * make


usage
-----

Invoke 'ipfixer'. It requires a configuration-file
for which an example (ipfixer.yaml) is included.


Written by Folkert van Heusden <mail@vanheusden.com>

Released under AGPL v2.0
