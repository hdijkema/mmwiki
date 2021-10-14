<?php
require_once('config.php');

if (loggedIn()) {
	echo "oke";
} else {
	http_response_code(401);
}

?>
