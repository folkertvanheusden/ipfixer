[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/folkertvanheusden/ipfixer.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/folkertvanheusden/ipfixer/context:cpp)


what it is
----------

ipfixer is a collector for IPFIX and NetFlow v9 data.
This data is captured and then stored in a MongoDB or
PostgreSQL database.


compiling
---------

The following package is required:

 * libyaml-cpp-dev

Optional packages:

 * libmongocxx-dev  at moment of writing (February
                    2022) this package is in Debian
                    in the 'experimental' branch)

 * libpqxx-dev      PostgreSQL support
 * libjansson-dev   Required for PostgreSQL

 * libmariadb-dev   MariaDB (MySQL) support
 * libjansson-dev   ...is then required as well


Then:

 * mkdir build
 * cd build
 * cmake ..
 * make


usage
-----

Invoke 'ipfixer'. It requires a configuration-file
for which an example (ipfixer.yaml) is included.

Note: if you get strange "out of range"-errors, make
sure you correclty configured IPFIX or NetFlow
depending on what the emitter is producing.


Written by Folkert van Heusden <mail@vanheusden.com>

Released under AGPL v2.0
