<?php

ob_start();
echo "Sorry, there was an error uploading your file.";
//header("Location: ../contact.html");
$output = ob_get_clean();

echo $output;
?>