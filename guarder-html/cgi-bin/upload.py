import cgi
import os
import sys

# Set the directory where you want to store the uploaded images
upload_dir = 'uploads/'

# Create the upload directory if it doesn't exist
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)
    
input_data = sys.stdin.buffer.read()
output_filename = "1.png"

with open(output_filename, 'wb') as output_file:
    output_file.write(input_data)
# # Create an instance of FieldStorage to handle the uploaded file
# form = cgi.FieldStorage()

# # Check if the 'file' field exists in the form
# if 'file' in form:
#     file_item = form['file']
    
#     # Check if the file was successfully uploaded
#     if file_item.filename:
#         # Build the absolute path to save the file
#         file_path = os.path.join(upload_dir, os.path.basename(file_item.filename))
        
#         # Open a new file in binary write mode and save the uploaded file
#         with open(file_path, 'wb') as file:
#             file.write(file_item.file.read())
        
#         print("Content-Type: text/html\n")
#         print("<html><body>")
#         print("<h2>File successfully uploaded and saved:</h2>")
#         print(f"<p>File name: {file_item.filename}</p>")
#         print(f"<p>File size: {os.path.getsize(file_path)} bytes</p>")
#         print(f"<p>File location: {file_path}</p>")
#         print("</body></html>")
#     else:
#         print("Content-type: text/html\n")
#         print("<html><body>")
#         print("<h2>Error: No file was uploaded.</h2>")
#         print("</body></html>")
# else:
#     print("Content-type: text/html\n")
#     print("<html><body>")
#     print("<h2>Error: 'file' field not found in the form.</h2>")
#     print("</body></html>")