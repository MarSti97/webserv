import cgi
import os
import sys

# Set the directory where you want to store the uploaded images
upload_dir = 'uploads/'

# Create the upload directory if it doesn't exist
if not os.path.exists(upload_dir):
	os.makedirs(upload_dir)
    
req_body_data = sys.stdin.buffer.read()

output_filename = os.environ.get("BODY_FILENAME")
file_path = upload_dir + output_filename 

with open(file_path, 'wb') as output_file:
    output_file.write(req_body_data)

print("<html><body>")
print("<h2>File successfully uploaded and saved:</h2>")
print(f"<p>File name: {output_filename}</p>")
print(f"<p>File size: {os.path.getsize(file_path)} bytes</p>")
print(f"<p>File location: {file_path}</p>")
print("</body></html>")