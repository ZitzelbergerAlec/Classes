<?php include("header.php"); ?>

<?php 
$result=mysql_query('SELECT * FROM TRAINER');
	echo "<table class=\"datatable\">";
	echo "<tr><th>First Name</th><th>Last Name</th><th>Age</th>";
	while ($row = mysql_fetch_assoc($result)) {
		echo "<tr>";
		echo "<td class=\"bold\">" . $row['TRAINER_FNAME'] . "</td>";
		echo "<td class=\"bold\">" . $row['TRAINER_LNAME'] . "</td>";
		echo "<td class=\"bold\">" . $row['AGE'] . "</td>";
		echo "</tr>";
		echo "<tr>";
		$result2=mysql_query('SELECT * FROM POKEMON P, TEAM T WHERE T.TRAINER_ID = ' . $row['TRAINER_ID'] . ' AND T.PK1 = P.ID UNION ALL SELECT * FROM POKEMON P, TEAM T WHERE T.TRAINER_ID = ' . $row['TRAINER_ID'] . ' AND T.PK2 = P.ID UNION ALL SELECT * FROM POKEMON P, TEAM T WHERE T.TRAINER_ID = ' . $row['TRAINER_ID'] . ' AND T.PK3 = P.ID UNION ALL SELECT * FROM POKEMON P, TEAM T WHERE T.TRAINER_ID = ' . $row['TRAINER_ID'] . ' AND T.PK4 = P.ID UNION ALL SELECT * FROM POKEMON P, TEAM T WHERE T.TRAINER_ID = ' . $row['TRAINER_ID'] . ' AND T.PK5 = P.ID UNION ALL SELECT * FROM POKEMON P, TEAM T WHERE T.TRAINER_ID = ' . $row['TRAINER_ID'] . ' AND T.PK6 = P.ID');
		while($row2 = mysql_fetch_assoc($result2)){
			echo "<td>" . $row2['PKMN_NAME'] . "</td>";
		}
		echo "</tr>";
		//echo "<tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr>";
	}
	echo "</table>";
?>
<?php include("footer.php"); ?>