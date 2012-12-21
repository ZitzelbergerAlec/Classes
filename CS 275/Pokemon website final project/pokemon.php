<?php include("header.php"); ?>
<?php
if(empty($_GET['pkmn'])){
	$result=mysql_query('SELECT * FROM POKEMON');
	echo "<table class=\"datatable\">";
	echo "<th>#</th>";
	echo "<th>Image</th>";
	echo "<th>Pokemon Name</th>";
	echo "<th>Type</th>";
	echo "<th>Species</th>";
	echo "<th>HP</th>";

	while ($row = mysql_fetch_assoc($result)) {
		echo "<tr>";
		echo "<td>" . $row['ID'] . "</td>";
		echo "<td>" . "<img src=\"img/pkmn/" . strtolower($row['PKMN_NAME']) . ".jpg\">" . "</td>";
		echo "<td><a href=\"?pkmn=" . $row['PKMN_NAME'] . "\">" . $row['PKMN_NAME'] . "</a></td>";
		echo "<td class=\"" . $row['TYPE'] . "\">" . $row['TYPE'] . "</td>";
		echo "<td>" . $row['SPECIES'] . "</td>";
		echo "<td>" . $row['HP'] . "</td>";
		echo "</tr>";
	}
	echo "</table>";
} else { //show more details!
	echo "<div id=\"main-form\">\n";
	$result=mysql_query('SELECT * FROM POKEMON P WHERE P.PKMN_NAME = "' . mysql_real_escape_string($_GET['pkmn']) . '"') or die ("Invalid Pokedex entry");
	$num_results = mysql_num_rows($result); 
	if($num_results > 0 ){
	while ($row = mysql_fetch_assoc($result)) {
		
			echo "<h1>" . $row['PKMN_NAME'] . "</h1><br />";
			echo "<img src=\"img/pkmn/" . strtolower($row['PKMN_NAME']) . ".jpg\"/><br />\n";
			echo "<h2>Pokedex entry:</h2><br />";
			echo "<div id=\"pokedex-entry\">" . $row['POKEDEX_ENTRY'] . "</div>\n";
			if(!empty($row['EVOLVES_FROM'])){
				$evolvesrow=mysql_query('SELECT PKMN_NAME FROM POKEMON P WHERE P.ID = ' . $row['EVOLVES_FROM']);
				while($evrow = mysql_fetch_assoc($evolvesrow)){
					echo "<h3>Evolves from:</h3><br /><a href=\"?pkmn=" . $evrow['PKMN_NAME'] . "\"><img src=\"img/pkmn/" . strtolower($evrow['PKMN_NAME']) . ".jpg\"/></a><br />\n";
					echo "<a href=\"?pkmn=" . $evrow['PKMN_NAME'] . "\">" . $evrow['PKMN_NAME'] . "</a><br />\n";
				}
			}
			if(!empty($row['EVOLVES_TO'])){
				$evolvesrow=mysql_query('SELECT PKMN_NAME FROM POKEMON P WHERE P.ID = ' . $row['EVOLVES_TO']);
				while($evrow = mysql_fetch_assoc($evolvesrow)){
					echo "<h3>Evolves to:</h3><br /><a href=\"?pkmn=" . $evrow['PKMN_NAME'] . "\"><img src=\"img/pkmn/" . strtolower($evrow['PKMN_NAME']) . ".jpg\"/></a><br />\n";
					echo "<a href=\"?pkmn=" . $evrow['PKMN_NAME'] . "\">" . $evrow['PKMN_NAME'] . "</a><br />\n";
				}
			}
	
	} 
	} 
	else {
			echo "<h2>Invalid Pokedex Entry.</h2><br />";
		}
	echo "</div>";
}
?>
<?php include("footer.php"); ?>