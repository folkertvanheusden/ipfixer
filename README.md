what it is
----------

ipfixer is a collector for IPFIX data. This data is
captured and then stored in e.g. a MondoDB database.


compiling
---------

The following packages are required:

 * libyaml-cpp-dev

 * libmongocxx-dev  at moment of writing (February
                    2022) this package is in Debian
                    in the 'experimental' branch)


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
