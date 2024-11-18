<?php

function createFolder($uploadPath) {
	if (is_dir($uploadPath))
		return true;
	if (!mkdir($uploadPath)) {
		echo "<h1>Failed to create file</h1>";
		return false;
	}
	return true;
}

function createFile($uploadPath) {
	$output = $uploadPath . "/" . $_FILES["file-upload"]["name"];
	if (is_file($output)) {
		echo "<h1>Ce fichier existe deja</h1>";
		return;
	}

	$success = copy($_FILES["file-upload"]["tmp_name"], $output);
	if ($success)
		echo "<h1>T'as cree le fichier <?php echo $output ?> t'es chaud frere</h1>";
	else
		echo "<h1>Failed to create $output :(</h1>";
}

function main() {
	$env = getenv();
	$uploadPath = $env["SERVER_PATH"] . "/" . $env["UPLOAD_PATH"];

	if (!createFolder($uploadPath))
		return;
	createFile($uploadPath);
}

main();
