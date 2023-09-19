<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $target_dir = "uploads/"; // Create an "uploads" directory in the same folder as this script.
    $target_file = $target_dir . basename($_FILES["image"]["name"]);

    // Check if the file already exists
    if (file_exists($target_file)) {
        echo "Sorry, the file already exists.";
    } else {
        // Try to move the uploaded file to the specified directory
        if (move_uploaded_file($_FILES["image"]["tmp_name"], $target_file)) {
            echo "The file " . htmlspecialchars(basename($_FILES["image"]["name"])) . " has been uploaded.";
        } else {
            echo "Sorry, there was an error uploading your file.";
        }
    }
}
?>