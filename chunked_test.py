import requests

# Specify the URL you want to send the chunked POST request to
url = "http://127.73.73.7:7681"  # Replace with the actual URL

# Create a generator to yield data chunks
def generate_data_chunks():
    yield b"Hello"
    yield b", world\n"
    yield b"This is the end!"

# Send a chunked POST request
response = requests.post(url, data=generate_data_chunks(), headers={"Transfer-Encoding": "chunked"})

# Check the response status code
if response.status_code == 200:
    print("Request was successful")
    print("Response content:")
    print(response.text)
else:
    print(f"Request failed with status code {response.status_code}")

