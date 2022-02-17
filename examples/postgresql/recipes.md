 * calculate distribution of flow byte counts, over 25 bins:

   SELECT
	ROUND(AVG(octetdeltacount), 2),
	(COUNT(*) * 100) / (SELECT COUNT(*) FROM records) AS percentage
   FROM
	records
   GROUP BY
	ROUND(octetdeltacount * 25 / (SELECT MAX(octetdeltacount) FROM records));


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
