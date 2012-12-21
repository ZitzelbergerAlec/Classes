<?php include("header.php"); ?>

<div id="main-form">
<h2>Trainers</h2><br />
<h3>Oldest Trainer:</h3><br />
<?php
$result1 = mysql_query('SELECT * from TRAINER where AGE = (SELECT MAX(AGE) from TRAINER) LIMIT 1');
echo "<table class=\"datatable-trainers\">";
echo "<th>First Name</th>";
echo "<th>Last Name</th>";
echo "<th>Age</th>";
while ($row1 = mysql_fetch_assoc($result1)) {
			echo "<tr><td>" . $row1['TRAINER_FNAME'] . "</td><td>" . $row1['TRAINER_LNAME'] . "</td><td>" . $row1['AGE'] . "</td></tr>\n";
		}

echo "</table>";

echo "<br />";
?>

<h3>Youngest Trainer:</h3><br />
<?php
$result1 = mysql_query('SELECT * from TRAINER where AGE = (SELECT MIN(AGE) from TRAINER) LIMIT 1'); //gotta select 'em all
echo "<table class=\"datatable-trainers\">";
echo "<th>First Name</th>";
echo "<th>Last Name</th>";
echo "<th>Age</th>";
while ($row1 = mysql_fetch_assoc($result1)) {
			echo "<tr><td>" . $row1['TRAINER_FNAME'] . "</td><td>" . $row1['TRAINER_LNAME'] . "</td><td>" . $row1['AGE'] . "</td></tr>\n";
		}

echo "</table>";

echo "<br />";
?>
<h3>Stats:</h3><br />
<table class="datatable-trainers">
	<tr>
		<th>Stat</th>
		<th>Value</th>
	</tr>
	<tr>
		<td>Total Number of Trainers</td>
		<td>
			<?php 
				$result = mysql_query('SELECT COUNT(*) from TRAINER'); 
					while ($row = mysql_fetch_assoc($result)) {
					echo $row['COUNT(*)'];
					}
			?>
		</td>
	</tr>
	<tr>
		<td>Number of Male Trainers</td>
		<td>
			<?php 
				$result = mysql_query('SELECT COUNT(*) from TRAINER WHERE GENDER = "M"'); 
					while ($row = mysql_fetch_assoc($result)) {
					echo $row['COUNT(*)'];
					}
			?>
		</td>
	</tr>
	<tr>
		<td>Number of Female Trainers</td>
		<td>
			<?php 
				$result = mysql_query('SELECT COUNT(*) from TRAINER WHERE GENDER = "F"'); 
					while ($row = mysql_fetch_assoc($result)) {
					echo $row['COUNT(*)'];
					}
			?>
		</td>
	</tr>
	<tr>
		<td>Number of Trainers over age 18</td>
		<td>
			<?php 
				$result = mysql_query('SELECT COUNT(*) from TRAINER WHERE AGE >= 18'); 
					while ($row = mysql_fetch_assoc($result)) {
					echo $row['COUNT(*)'];
					}
			?>
		</td>
	</tr>
</table>

<h3>Trainer List:</h3><br />
<?php
$result = mysql_query('SELECT * from TRAINER'); //gotta select 'em all
echo "<table class=\"datatable-trainers\">";
echo "<th>First Name</th>";
echo "<th>Last Name</th>";
echo "<th>Age</th>";
echo "<th>Gender</th>";

while ($row = mysql_fetch_assoc($result)) {
			echo "<tr><td>" . $row['TRAINER_FNAME'] . "</td><td>" . $row['TRAINER_LNAME'] . "</td><td>" . $row['AGE'] . "</td><td>" . $row['GENDER'] . "</td></tr>\n";
		}

echo "</table>";
echo "</div>";
		include("footer.php");
?>