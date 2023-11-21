import http.client

# Specify the URL you want to send the chunked POST request to
url = "127.0.0.1"
port = 8080
path = "/"

# Create a generator to yield data chunks
def generate_data_chunks():
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!0"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!1"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!2"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!3"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!4"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!5"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!6"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!7"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!8"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!9"
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!10"

# Connect to the server
conn = http.client.HTTPConnection(url, port)

# Begin the chunked request
conn.putrequest("POST", path)
conn.putheader("Connection", "keep-alive")
conn.putheader("Transfer-Encoding", "chunked")
conn.endheaders()

# Send the data chunks
for chunk in generate_data_chunks():
    chunk_size_hex = format(len(chunk), 'X').encode('utf-8')  # Convert chunk size to hexadecimal
    conn.send(chunk_size_hex + b'\r\n')
    conn.send(chunk + b'\r\n')

# End the request
conn.send(b'0\r\n\r\n')

# Get the response
response = conn.getresponse()

# Check the response status code
if response.status == 200:
    print("Request was successful")
    print("Response content:")
    print(response.read().decode('utf-8'))
else:
    print(f"Request failed with status code {response.status}")

# Close the connection
conn.close()


