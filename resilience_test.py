import requests
import concurrent.futures
import time

counter = 0
def send_request(url):
	try:
		response = requests.get(url)
        # Optionally, you can check the response status code and content
		if response.status_code != 200: 
			print("Request unsuccessful:", counter)
			counter += 1
		# else:
		# 	print("Request successful")
	except requests.RequestException as e:
		print(f"Request failed: {e}")

def test_web_server_concurrent(url, num_requests, num_concurrent):
    start_time = time.time()

    with concurrent.futures.ThreadPoolExecutor(max_workers=num_concurrent) as executor:
        # Use a list comprehension to create a list of futures
        futures = [executor.submit(send_request, url) for _ in range(num_requests)]

        # Wait for all futures to complete
        concurrent.futures.wait(futures)

    end_time = time.time()
    elapsed_time = end_time - start_time

    print(f"Total time taken: {elapsed_time} seconds")
    print(f"Requests per second: {num_requests / elapsed_time}")

if __name__ == "__main__":
    # Replace 'http://your_web_server_url' with the actual URL of your web server
    web_server_url = 'http://127.0.0.1:8080/empty.html'
    # Set the number of requests you want to simulate
    num_requests = 10000
    # Set the number of concurrent requests
    num_concurrent = 500

    test_web_server_concurrent(web_server_url, num_requests, num_concurrent)
