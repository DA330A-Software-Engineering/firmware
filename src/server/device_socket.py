from typing import Callable, Any
from tornado.ioloop import IOLoop
from tornado import gen
from tornado.websocket import websocket_connect, WebSocketClientConnection


class DeviceSocket(object):
    def __init__(self, url: str, timeout) -> None:
        self.url = url
        self.on_message_listeners: list[Callable[[str], None]] = []
        self.timeout = timeout
        self.ioloop = IOLoop.current()
        self.ws: WebSocketClientConnection | None = None

    @gen.coroutine
    def connect(self):
        print("trying to connect")
        try:
            self.ws = yield websocket_connect(self.url)
        except Exception:
            print("connection error")
        else:
            print("connected")
            self.run()

    @gen.coroutine
    def run(self):
        while True:
            msg = yield self.ws.read_message()  # type: ignore
            if msg is None:
                print("connection closed")
                self.ws = None
                break

            for listener in self.on_message_listeners:
                listener(msg)

    @gen.coroutine
    def broadcast(self, object: Any) -> None:
        if self.ws is None:
            self.connect()
        else:
            self.ws.write_message(object.to_json())

    def add_on_message_listener(self, listener: Callable[[str], None]) -> None:
        self.on_message_listeners.append(listener)


# class DeviceSocket:
#     def __init__(self, uri: str) -> None:
#         self.on_message_listeners: list[Callable[[str], None]] = []
#         websocket.enableTrace(True)
#         self.socket = websocket.WebSocketApp(
#             uri,
#             # on_open=lambda ws: self.__on_open(ws),
#             on_message=lambda ws, msg: print(msg),
#             # on_error=lambda ws, err: self.__on_error(ws, err),
#             # on_close=lambda status, msg: self.__on_close(status, msg),
#         )

#         self.socket.run_forever(dispatcher=rel, reconnect=3)

#     def broadcast(self, object: Any) -> None:
#         self.socket.send(object.toJson())

#     def __on_error(self, ws, error) -> None:
#         print(error)

#     def __on_message(self, ws, message) -> None:
#         print("ass")
#         for listener in self.on_message_listeners:
#             listener(message)

#     def __on_open(self, ws) -> None:
#         print("Opened connection")

#     def __on_close(self, close_status_code, close_msg) -> None:
#         print("### closed ###")

#     def add_on_message_listener(self, listener: Callable[[str], None]) -> None:
#         self.on_message_listeners.append(listener)
