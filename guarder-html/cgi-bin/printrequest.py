
import os
import cgi
import sys
import re

# Print the Content-Type header
print("Content-Type: text/html\n")

# Print the HTML page with the contents of the FieldStorage
print("<html>")
print("<head><title>POST Request</title></head>")
print("<body>")
print("<h1>POST Request</h1>")

# # Parse the request body
# form = cgi.FieldStorage()

# # Print the request headers
# print("<p>Here are the request headers:</p>")
# for key, value in sorted(form.headers.items()):
#     print(f"<p><strong>{key}:</strong> {value}</p>")

# # Print the request body (if it exists)
# if form:
#     print("<p>Here is the request body:</p>")
#     for field_name in form.keys():
#         field = form[field_name]
#         if field.filename:
#             print(f"<p><strong>File Field Name:</strong> {field_name}</p>")
#             print(f"<p><strong>File Name:</strong> {field.filename}</p>")
#             print("<pre>")
#             print(field.file.read())  # Print the content of the uploaded file
#             print("</pre>")
#         else:
#             print(f"<p><strong>Field Name:</strong> {field_name}</p>")
#             print(f"<p><strong>Field Value:</strong> {field.value}</p>")
# else:
#     print("<p>No request body data found.</p>")


# Read the raw input from sys.stdin
input_data = sys.stdin.read()

key_to_extract = "user_input"

# Use a regular expression to find and extract the value
pattern = re.compile(fr'{key_to_extract}=(.*)')
match = pattern.search(input_data)

if match:
    value = match.group(1)
    print(f"<p>The message you sent to the server was: {value}</p>")
else:
    print(f"<p>{key_to_extract} not found in the input string.</p>")

print("</body>")
print("</html>")