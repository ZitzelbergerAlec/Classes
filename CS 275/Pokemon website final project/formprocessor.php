<?php 
/*
include("header.php"); 
echo "Here's the database table we're using: <br />";
echo "<img src=\"trainer-db.png\"></img>";
echo "<br />You entered: ";
print_r($_POST);
include("footer.php"); 
*/

$error = 0;

if(empty($_POST[TRAINER_FNAME])){
	$error = 1;
} else if(empty($_POST[AGE]) || ($_POST[AGE] < 1) || ($_POST[AGE] >= 999)){
	$error = 3;
} 
//profanity filters
else if(
		   strstr(strtolower($_POST[TRAINER_FNAME]), "penis") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "penis") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "cunt") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "cunt") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "shit") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "shit") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "ass") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "ass") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "balls") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "balls") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "cum") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "cum")
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "fuck") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "fuck") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "vagina") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "vagina") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "pussy") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "pussy") 
		|| strstr(strtolower($_POST[TRAINER_FNAME]), "cock") 
		|| strstr(strtolower($_POST[TRAINER_LNAME]), "cock") 
		){
	$error = 69;
}

if($error != 0){
header('Location: index.php?error=' . $error);
} else {
include("header.php"); ?>
<div id="main-form">

<?php
$trainer_fname = mysql_real_escape_string($_POST[TRAINER_FNAME]);
$trainer_lname = mysql_real_escape_string($_POST[TRAINER_LNAME]);
$age = mysql_real_escape_string($_POST[AGE]);
$gender = mysql_real_escape_string($_POST[GENDER]);

	$sql="INSERT INTO TRAINER (TRAINER_FNAME, TRAINER_LNAME, AGE, GENDER)
	VALUES ('$trainer_fname','$trainer_lname','$age', '$gender')";

	if (!mysql_query($sql,$mysql_handle)){
		  die('Error: ' . mysql_error());
  	}

	echo "<img src=\"prof_oak.jpg\"><br />";
	echo "<h1>Wait, $_POST[TRAINER_FNAME], <br />it's dangerous out there!<br /> Assemble your team!</h1><br />";
	echo "Quick, choose 6 Pokemon for your team!\n";
?>
<form id="TEAM" action="teamform.php" method="POST">
<?php
	for($i=1; $i<=6; $i++){
		$result = mysql_query('SELECT * from POKEMON'); //gotta select 'em all
		echo "Pokemon " . $i . ":";
		echo "<select name=\"PK$i\">\n";
			while ($row = mysql_fetch_assoc($result)) {
			echo "<option value =\"" . $row['ID'] . "\">" . $row['PKMN_NAME'] . "</option>";
		}
		echo "</select><br />\n";
	}
?>
<?php echo "<input type=\"hidden\" name=\"TRAINER_ID\" value=\"" . mysql_insert_id() . "\">"; ?>
<input type="submit" value="Submit" />
</form>	
</div>
<?php
include("footer.php");
}
?>