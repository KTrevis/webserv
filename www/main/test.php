<?php
// Set the header to specify JSON response format
header("Content-Type: application/json");

// Check if any form data was submitted
if (!empty($_POST) || !empty($_FILES)) {
    $response = [
        "status" => "success",
        "message" => "Data received",
        "formData" => $_POST,
        "files" => []
    ];

    // Process any uploaded files
    foreach ($_FILES as $key => $file) {
        if ($file['error'] === UPLOAD_ERR_OK) {
            // Save file details
            $response['files'][$key] = [
                "name" => $file['name'],
                "type" => $file['type'],
                "size" => $file['size'],
                "tmp_name" => $file['tmp_name']
            ];
            
            // (Optional) Move the uploaded file to a permanent location
            // move_uploaded_file($file['tmp_name'], "/path/to/uploads/" . $file['name']);
        } else {
            // Handle file upload errors
            $response['files'][$key] = [
                "error" => "Error uploading file"
            ];
        }
    }
} else {
    $response = [
        "status" => "error",
        "message" => "No form data received"
    ];
}

// Send the JSON response
echo json_encode($response);
