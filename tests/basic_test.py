import socket
import sys

def main():
    server_ip = "localhost"
    port = 16384
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((server_ip, port))
        print("Connected to the server")
    except ConnectionRefusedError:
        print("Connection refused by the server. Make sure the server is running and reachable.")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        # Close the socket connection
        client_socket.close()

main()
