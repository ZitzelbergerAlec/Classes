<?php include("header.php"); ?>
<div id="main-form">
<?php
$pk = array(mysql_real_escape_string($_POST[PK1]), mysql_real_escape_string($_POST[PK2]), mysql_real_escape_string($_POST[PK3]), mysql_real_escape_string($_POST[PK4]), mysql_real_escape_string($_POST[PK5]), mysql_real_escape_string($_POST[PK6]));
$trainer_id = mysql_real_escape_string($_POST[TRAINER_ID]);
$pk1 = mysql_real_escape_string($_POST[PK1]);
$pk2 = mysql_real_escape_string($_POST[PK2]);
$pk3 = mysql_real_escape_string($_POST[PK3]);
$pk4 = mysql_real_escape_string($_POST[PK4]);
$pk5 = mysql_real_escape_string($_POST[PK5]);
$pk6 = mysql_real_escape_string($_POST[PK6]);

$sql="INSERT INTO TEAM (TRAINER_ID, PK1, PK2, PK3, PK4, PK5, PK6)
	VALUES ('$trainer_id', '$pk1', '$pk2', '$pk3', '$pk4', '$pk5', '$pk6')";

	if (!mysql_query($sql,$mysql_handle)){
		  die('Error: ' . mysql_error());
  	} 

echo "<h1>Your team:</h1><br />\n";

for($i=0; $i<6; $i++){
	$result=mysql_query('SELECT P.PKMN_NAME FROM POKEMON P WHERE P.ID = ' . $pk[$i]);

	while ($row = mysql_fetch_assoc($result)) {
		echo "<div id=\"team-member\"";
		if($i%2 == 0){ //put a break on even rows
			echo " class=\"clear-left\"";
		}
		echo "><a href=\"pokemon.php?pkmn=" . $row['PKMN_NAME'] . "\"><img src=\"img/pkmn/" . strtolower($row['PKMN_NAME']) . ".jpg\"></a></div>\n";
	}
}
?>
</div>
<?php include("footer.php"); ?>
