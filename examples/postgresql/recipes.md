 * calculate distribution of flow byte counts, over 25 bins:

   SELECT
	ROUND(AVG(octetdeltacount), 2),
	(COUNT(*) * 100) / (SELECT COUNT(*) FROM records) AS percentage
   FROM
	records
   GROUP BY
	ROUND(octetdeltacount * 25 / (SELECT MAX(octetdeltacount) FROM records));
