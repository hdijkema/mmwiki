<?php

###############################################
# Global prefix
###############################################

global $MMWIKI_PREFIX;
$MMWIKI_PREFIX = "c:/xampp/htdocs/mmwiki";

###############################################
# Start session
###############################################

session_start();

###############################################
# Support functions
###############################################

function loggedIn() {
	if (isset($_SESSION['logged-in'])) { return true; }
	else { return false; }
}

?>