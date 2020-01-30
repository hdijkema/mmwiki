<?php
require_once('config.php');

header("Content-Type: application/json");
$data = file_get_contents("php://input");
$v = json_decode($data);

$account = trim($v->account);
$passwd = trim($v->passwd);

$file = "$MMWIKI_PREFIX/passwd/passwd.json";
$json = json_decode(file_get_contents($file));

$accounts = array();
foreach($json as $obj) {
	$accounts[$obj->account] = $obj->passwd;
}

if (isset($accounts[$account])) {
	$pwd = $accounts[$account];
	if ($passwd == $pwd) {
		$_SESSION['logged-in'] = true;
		setcookie("logged-in", "yes");
		error_log("User $account logged-in");
	} else {
		error_log("Cannot login with user credentials for account $account");
		session_unset();
		setcookie("logged-in", "no");
		http_response_code(401);
	}
} else {
	error_log("No user $account");
	session_unset();
	setcookie("logged-in", "no");
	http_response_code(401);
}

?>