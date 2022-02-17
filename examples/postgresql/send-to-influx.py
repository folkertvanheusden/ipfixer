#! /usr/bin/python3

# required: psycopg2 (from pip)

## configure this ##
db_connection_parameters = 'host=127.0.0.1 port=5434 dbname=ipfixer_eth1 user=ipfix password=ipfix'

n_counters = 10

influx_graphite_address = ('127.0.0.1', 2011)
####################

import psycopg2
import psycopg2.extras
import socket
import time

con = psycopg2.connect(db_connection_parameters)

cur = con.cursor(cursor_factory=psycopg2.extras.DictCursor)

cur.execute("select destinationtransportport, name, sum(octetdeltacount) as n_bytes, count(*) as n from records, tcp_udp_ports where ts >= now() - interval '5 minutes' and destinationtransportport=tcp_udp_ports.port group by destinationtransportport, name order by n desc limit %d" % n_counters)

now = int(time.time())

s = socket.socket()
s.connect(influx_graphite_address)

for row in cur:
    # print(now, row['destinationtransportport'], row['name'], row['n_bytes'])

    data = 'net.%s-%d %s %d\n' % (row['name'], row['destinationtransportport'], row['n_bytes'], now)
    s.sendall(data.encode('ascii'))


cur.execute("select miscellaneous->'ipVersion' as ipversion, sum(octetdeltacount) as n_bytes, count(*) as n from records where ts >= now() - interval '5 minutes' group by miscellaneous->'ipVersion' order by n desc limit 10")

for row in cur:
    data = 'net.ipv%s %s %d\n' % (row['ipversion'], row['n_bytes'], now)
    s.sendall(data.encode('ascii'))

s.close()
