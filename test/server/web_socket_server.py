from asyncio import Future
from typing import Any, Awaitable, Optional, Union
from tornado import websocket, web


class WebSocketServer(websocket.WebSocketHandler):
    """Simple WebSocket handler to serve clients."""

    # Note that `clients` is a class variable and `send_message` is a
    # classmethod.
    clients = set()  # type: ignore

    def __init__(self, application: web.Application, request: httputil.HTTPServerRequest, **kwargs: Any) -> None:
        super().__init__(application, request, **kwargs)

    def open(self):
        WebSocketServer.clients.add(self)

    def on_close(self):
        WebSocketServer.clients.remove(self)

    def write_message(self, message: Union[bytes, str, dict[str, Any]], binary: bool = False) -> Future[None]:
        return super().write_message(message, binary)

    def on_message(self, message: Union[str, bytes]) -> Optional[Awaitable[None]]:
        return super().on_message(message)

    @classmethod
    def send_message(cls, message: str):
        print(f"Sending message {message} to {len(cls.clients)} client(s).")
        for client in cls.clients:
            client.write_message(message)
