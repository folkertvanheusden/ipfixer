 * calculate distribution of flow byte counts, over 25 bins:

   SELECT
	ROUND(AVG(octetdeltacount), 2) AS avg_bytes,
	ROUND((COUNT(*) * 100.0) / (SELECT COUNT(*) FROM records), 3) AS percentage
   FROM
	records
   WHERE
	octetdeltacount < 2147483647
   GROUP BY
 	ROUND(octetdeltacount * 25 / (SELECT MAX(octetdeltacount) FROM records where octetdeltacount < 2147483647))
   ORDER BY
	avg_bytes;


 * calculate amount of traffic (in bytes) per IP version (IPv4, IPv6):

   SELECT
	miscellaneous->'ipVersion',
	ROUND(SUM(octetdeltacount) * 100.0 / (SELECT SUM(octetdeltacount) FROM records), 2)
   FROM
	records
   GROUP BY
	miscellaneous->'ipVersion'
   ORDER BY
	miscellaneous->'ipVersion';


 * calculate amount of traffic (in bytes) per IP protocol (TCP, UDP, etc.):

   SELECT
	protocolidentifier,
	ROUND(SUM(octetdeltacount) * 100.0 / (SELECT SUM(octetdeltacount) FROM records), 2)
   FROM
	records
   GROUP BY
	protocolidentifier
   ORDER BY
	protocolidentifier;
