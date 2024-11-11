<?php
// Set the header to specify JSON response format
header("Content-Type: application/json");

$res = [
	"env" => getenv(),
	"get" => $_GET,
	"post" => $_POST,
	"files" => $_FILES
];

echo json_encode($res);
