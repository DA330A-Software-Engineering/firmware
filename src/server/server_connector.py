from src.server.constants import (
    CFG_SOCKETS,
    CFG_SOCKET_IP,
    CFG_ACTION_SOCKET,
    CFG_REPLY_SOCKET,
    CFG_REST_API,
    CFG_REST_API_BASE_IP,
)  # noqa
from src.server.receiving_socket import ReceivingSocket
from src.server.sending_socket import SendingSocket
from src.models.action import Action
from src.models.hardware_reply import HardwareReply
from typing import Callable
import socket
import requests  # type: ignore
import sys


class ServerConnector:
    def __init__(self, config: dict) -> None:
        self.config = config
        self.pin_map: dict[str, str] = {"light_yellow": "5", "light_white": "13"}

    def __enter__(self) -> "ServerConnector":
        try:
            self.action_socket, self.reply_socket = self.__connect_sockets(self.config[CFG_SOCKETS])
        except socket.error as e:
            print("[ServerConnector] Caught exception while attempting to connect to a socket")
            raise e

        print("[ServerConnector] Successfully connected sockets")

        # try:
        #     self.pin_map = self.__fetch_master_pin_list(self.config[CFG_REST_API])
        # except requests.exceptions.RequestException as e:
        #     print("[ServerConnector] Caught exception while attempting to fetch master map of pins")
        #     raise e

        return self

    def __exit__(self, type, value, traceback):
        if type and value and traceback:
            print(f"[ServerConnector] Error of type {type.__name__} occurred in ServerConnector, closing connections.")

        self.action_socket.stop().close()
        self.reply_socket.close()

    def run_action_socket(self, callbacks: list[Callable[[Action], HardwareReply]]) -> None:
        assert callbacks is not None, "[ServerConnector] Cannot run action socket without at least one callback."

        for callback in callbacks:

            def wrapper(action: Action):
                response = callback(action)
                self.__handle_callback_output(response)

            self.action_socket.add_callback(wrapper)

        self.action_socket.start()
        self.action_socket.run()

    def __handle_callback_output(self, callback_response: HardwareReply) -> None:
        self.reply_socket.send_data(callback_response)

    def __connect_sockets(self, socket_config: dict) -> tuple[ReceivingSocket[Action], SendingSocket[HardwareReply]]:
        host = socket_config[CFG_SOCKET_IP]
        action_port = socket_config[CFG_ACTION_SOCKET]
        reply_port = socket_config[CFG_REPLY_SOCKET]

        action_socket = ReceivingSocket[Action](host, action_port).connect()
        reply_socket = SendingSocket[HardwareReply](host, reply_port).connect()

        return (action_socket, reply_socket)

    def __fetch_master_pin_list(self, rest_config: dict) -> dict[str, str]:
        host = rest_config[CFG_REST_API_BASE_IP]

        response = requests.get(host, timeout=5)
        response.raise_for_status()

        return response.json()
