import socket
def send_chunked_request():
    host = "127.73.73.7"
    port = 7681

    request = (
            "POST / HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "5\r\n"
            "This \r\n"
            "7\r\n"
            "is the \r\n"
            "8\r\n"
            "chunked \r\n"
            "8\r\n"
            "request \r\n"
            "8\r\n"
            "payload.\r\n"
            "0\r\n"
            "\r\n"
    )

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))
    client_socket.sendall(request.encode())

    # response = client_socket.recv(4096).decode()
    # print("Response:\n" + response)

    client_socket.close()

if __name__ == "__main__":
    send_chunked_request()