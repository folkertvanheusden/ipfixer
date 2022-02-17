#! /usr/bin/python3

# required: psycopg2 (from pip)

## configure this ##
db_connection_parameters = 'host=127.0.0.1 port=5434 dbname=ipfixer_eth1 user=ipfix password=ipfix'
####################


import csv
import io
import psycopg2
import urllib.request

data = urllib.request.urlopen('https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.csv').read().decode('utf-8')

con = psycopg2.connect(db_connection_parameters)

cursor = con.cursor()

cursor.execute('CREATE TABLE IF NOT EXISTS tcp_udp_ports(port INT NOT NULL, name VARCHAR(64) NOT NULL, PRIMARY KEY(port))')

f = io.StringIO(data)

reader = csv.reader(f, delimiter=',')

map_ = dict()

for row in reader:
    if len(row) >= 2 and len(row[0]) > 0:
        name = row[0]
        port = row[1]

        try:
            port = int(port)

            map_[port] = name

        except ValueError:
            # most likely the csv header
            pass

cursor.execute('truncate tcp_udp_ports')

for key in map_:
    port = key
    name = map_[key]

    cursor.execute('INSERT INTO tcp_udp_ports(port, name) VALUES(%s, %s)', (port, name))

con.commit()

con.close()
