import socket
import json


def main():
    host = "127.0.0.1"
    activity_port = 3001
    reply_port = 3002
    sensor_port = 3003

    activity_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    reply_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sensor_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    with activity_socket, reply_socket, sensor_socket:
        activity_socket.bind((host, activity_port))
        reply_socket.bind((host, reply_port))
        # sensor_socket.bind((host, sensor_port))

        activity_socket.listen()
        reply_socket.listen()
        # sensor_socket.listen()

        conn1, addr1 = activity_socket.accept()
        conn2, addr2 = reply_socket.accept()
        # conn3, addr3 = sensor_socket.accept()

        with conn1, conn2:
            print(f"Action connected by {addr1}\nReply connected by {addr2}")
            while True:
                data = input("What string would you like to send to the action port?")
                conn1.sendall(data.encode())

                buffer = b""
                while True:
                    chunk = conn2.recv(4096)

                    if not chunk:
                        break

                    buffer += chunk
                    break

                data_dict = json.loads(buffer.decode())

                print(f"Received the hardware reply: {data_dict}")


if __name__ == "__main__":
    main()
