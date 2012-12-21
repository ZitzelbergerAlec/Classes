<html>
	<head>
		<title>Gotta catch 'em all!</title>
		<link rel="stylesheet" type="text/css" href="style.css" />
		<link rel="shortcut icon" href="favicon.ico">
	</head>
	<body>
		<div id="header">
			<div id="header-img"><a href="index.php"><img src="img/pokemon.png"><h1>Team Builder</h1></a></div>
			<div id="header-links">
				<a href="index.php">Home</a>
				<a href="trainers.php">Trainers</a>
				<a href="pokemon.php">Pokemon</a>
				<a href="teams.php">Teams</a>
				<a href="locations.php">Locations</a>
			</div>
		</div>
		<div id="main">
		<?php
		//connect to our database
		$dbhost = 'oniddb.cws.oregonstate.edu';
		$dbname = 'merrickd-db';
		$dbuser = 'merrickd-db';
		$dbpass = '00LviMNyBNugTGlU';

		$mysql_handle = mysql_connect($dbhost, $dbuser, $dbpass)
			or die("Error connecting to database server");

		mysql_select_db($dbname, $mysql_handle)
			or die("Error selecting database: $dbname");
		?>