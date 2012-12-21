<?php include("header.php"); ?>
<?php
if(empty($_GET['locid'])){	
	$result=mysql_query('SELECT * FROM LOCATION');
	echo "<table class=\"datatable\">";
	echo "<th>Locations where certain Pokemon can be found</th>";

	while ($row = mysql_fetch_assoc($result)) {
		echo "<tr>";
		echo "<td><a href=\"?locid=" . $row['LOCATION_ID'] . "\">" . $row['LOCATION_NAME'] . "</a></td>";
		echo "</tr>";
	}
	echo "</table>";
} 
else {
	echo "<div id=\"main-form\">";
	$result=mysql_query('SELECT * FROM LOCATION L WHERE L.LOCATION_ID = "' . mysql_real_escape_string($_GET['locid']) . '"') or die ("Invalid Location");
	$num_results = mysql_num_rows($result);
	if($num_results > 0 ){
	while($row = mysql_fetch_assoc($result)) {
		
		echo "<h1>Pokemon found in</h1>\n<h1>" . $row['LOCATION_NAME'] . "</h1><br />";
		$pkmnloc = mysql_query('SELECT * FROM PKMN_LOCATIONS PLOC WHERE PLOC.LOCATION_ID = ' . mysql_real_escape_string($_GET['locid']));
		while ($row2 = mysql_fetch_assoc($pkmnloc)) {
			$poke_name = mysql_query('SELECT PKMN_NAME FROM POKEMON P WHERE P.ID = ' . $row2['PKMN_ID']);
			while($row3 = mysql_fetch_assoc($poke_name)){
				echo "<a href=\"pokemon.php?pkmn=" . $row3['PKMN_NAME'] . "\">" . $row3['PKMN_NAME'] . "</a><br />";
			}
		}
	}
	}
	echo "</div>";
}

?>
<?php include("footer.php"); ?>