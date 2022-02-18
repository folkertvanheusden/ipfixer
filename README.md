[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/folkertvanheusden/ipfixer.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/folkertvanheusden/ipfixer/context:cpp)
![Coverity Scan](https://shields.api-test.nl:/coverity/scan/24517)
[![Build Status](https://app.travis-ci.com/folkertvanheusden/ipfixer.svg?branch=master)](https://app.travis-ci.com/folkertvanheusden/ipfixer)
[![Code Score](https://api.codiga.io/project/31370/score/svg)](https://www.codiga.io)
[![Code Grade](https://api.codiga.io/project/31370/status/svg)](https://www.codiga.io)


what it is
----------

ipfixer is a collector for IPFIX, NetFlow v9 and
NetFlow v5 data.
This data is captured and then stored in a MongoDB,
MariaDB/MySQL, PostgreSQL or InfluxDB database.


compiling
---------

The following package is required:

 * libjansson-dev

 * libyaml-cpp-dev

 * pkg-config       for cmake

Optional packages:

 * libmongocxx-dev  at moment of writing (February
                    2022) this package is in Debian
                    in the 'experimental' branch)

 * libpqxx-dev      PostgreSQL support

 * libmariadb-dev   MariaDB (MySQL) support


Then:

 * mkdir build
 * cd build
 * cmake ..
 * make


usage
-----

Invoke 'ipfixer'. It requires a configuration-file
for which an example (ipfixer.yaml) is included.

InfluxDB requires a host/port of a 'graphite
endpoint'.

Note: if you get strange "out of range"-errors, make
sure you correclty configured IPFIX or NetFlow
depending on what the emitter is producing.

Note: you probably don't want "debug" log-level as
that uses a lot of CPU.


tested with
-----------

* EdgeMAX EdgeRouter Lite v1.10.0
* softflowd   - https://github.com/irino/softflowd
* ipt_NETFLOW - https://github.com/aabc/ipt-netflow


Written by Folkert van Heusden <mail@vanheusden.com>

Released under AGPL v2.0
