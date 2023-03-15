from src.server.constants import (
    CFG_SOCKETS,
    CFG_ACTION_SOCKET,
    CFG_REPLY_SOCKET,
    CFG_REST_API,
    CFG_REST_API_BASE_IP,
)  # noqa
from src.server.device_socket import DeviceSocket
from src.models.action import Action
from src.models.hardware_reply import HardwareReply
from typing import Callable
import requests  # type: ignore


class ServerConnector:
    def __init__(self, config: dict) -> None:
        self.config = config
        self.action_socket = DeviceSocket(config[CFG_SOCKETS][CFG_ACTION_SOCKET], 2000)
        self.reply_socket = DeviceSocket(config[CFG_SOCKETS][CFG_REPLY_SOCKET], 2000)
        self.pin_map: dict[str, str] = {"x7IPsBEJXCTgYUdQTHcp": "5"}

        # try:
        #     self.pin_map = self.__fetch_master_pin_list(self.config[CFG_REST_API])
        # except requests.exceptions.RequestException as e:
        #     print("[ServerConnector] Caught exception while attempting to fetch master map of pins")
        #     raise e

    def add_action_listener(self, action_socket_listener: Callable[[Action], HardwareReply]):
        def wrapper(action_json: str):
            print(action_json)
            action = Action.from_json(action_json)
            response = action_socket_listener(action)
            self.send_response(response)

        self.action_socket.add_on_message_listener(lambda msg: wrapper(msg))

    def send_response(self, response: HardwareReply) -> None:
        self.reply_socket.broadcast(response)

    def __fetch_master_pin_list(self, rest_config: dict) -> dict[str, str]:
        host = rest_config[CFG_REST_API_BASE_IP]

        response = requests.get(host, timeout=5)
        response.raise_for_status()

        return response.json()
