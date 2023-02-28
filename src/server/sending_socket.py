import socket
from typing import TypeVar, Generic
from src.models.json_serializable import JsonSerializable

T = TypeVar("T", bound=JsonSerializable)


class SendingSocket(Generic[T]):
    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def connect(self) -> "SendingSocket[T]":
        try:
            self.socket.connect((self.host, self.port))
        except socket.error as e:
            print(f"[SendingSocket] Caught exception while connecting to socket: {e}")
            self.socket.close()
            raise e

        return self

    def close(self) -> "SendingSocket[T]":
        self.socket.close()
        return self

    def send_data(self, data: T) -> None:
        self.socket.sendall(data.to_json().encode())
