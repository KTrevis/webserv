<?php
$output = "./" . $_FILES["file-upload"]["name"];
copy($_FILES["file-upload"]["tmp_name"], $output);
?>
<h1>T'as cree le fichier <?php echo $output ?> t'es chaud frere</h1>
