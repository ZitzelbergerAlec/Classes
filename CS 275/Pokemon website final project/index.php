<?php
include("header.php");
$error = $_GET['error'];
?>
<div id="main-form">
	<?php
	if(!empty($error)){
		echo "<div id=\"error\">";
		if($error == 1){
			echo "Please enter your first name.";
		} else if($error == 3){
			echo "Please enter an age between 1 and 999.";
		} else if($error == 69){
			echo "Please be mature and enter an appropriate name.";
		}
		echo "</div>";
	}
	?>
	<h2 class="main-title">Gotta enter all your info!</h2><br />
	<form id="trainer_name" action="formprocessor.php" method="POST">
		First name: <input type="text" name="TRAINER_FNAME" /><br />
		Last name: <input type="text" name="TRAINER_LNAME" /><br />
		Age: <input type="text" name="AGE" size=2/><br />
		Gender: <select name="GENDER">
		  <option>M</option>
		  <option>F</option>
		</select><br />
		<input type="submit" value="Submit" />
	</form>
</div>


<?php include("footer.php"); ?>