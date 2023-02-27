import socket
import json
import threading
from typing import Callable, TypeVar, Generic, get_args
from src.models.json_serializable import JsonSerializable

T = TypeVar("T", bound=JsonSerializable)


class ReceivingSocket(Generic[T]):
    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.settimeout(0.05)
        self.thread = None
        self.callbacks: list[Callable[[T], None]] = []
        self.connected = False

    def connect(self) -> "ReceivingSocket[T]":
        try:
            self.socket.connect((self.host, self.port))
            self.connected = True
        except socket.error as e:
            print(f"[ReceivingSocket] Caught exception while connecting to socket: {e}")
            self.socket.close()
            raise e

        return self

    def close(self) -> "ReceivingSocket[T]":
        self.socket.close()
        return self

    def start(self):
        self.thread = threading.Thread(target=self.run)
        self.thread.start()

    def stop(self) -> "ReceivingSocket[T]":
        if not self.thread:
            return self

        self.thread.join()
        return self

    def add_callback(self, callback: Callable[[T], None]) -> None:
        self.callbacks.append(callback)

    def remove_callback(self, callback: Callable[[T], None]) -> None:
        self.callbacks.remove(callback)

    def run(self) -> None:
        while True:
            action = self.receive_action()

            for callback in self.callbacks:
                callback(action)

    def receive_action(self) -> T:
        data = b""
        while True:
            try:
                chunk = self.socket.recv(4096)

                if not chunk:
                    break

                data += chunk
                break
            except TimeoutError:
                pass

        data_dict = json.loads(data.decode())
        return get_args(self.__orig_class__)[0].from_json(json.dumps(data_dict))  # type: ignore
