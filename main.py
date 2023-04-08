from src.hardware_connector.hardware_connector_placeholder import HardwareConnector
from src.models.action import Action
from src.models.hardware_reply import HardwareReply
from src.models.sensor_state import SensorState
from src.server.server_connector import ServerConnector

# from src.hardware_connector.hardware_connector_placeholder import HardwareConnector
import yaml  # type: ignore
from tornado.ioloop import IOLoop


def main() -> None:
    config = load_config("./config.yaml")

    server = ServerConnector(config)

    def send_sensor_data(pin: int, value: int):
        sensor_id = list(server.pin_map.keys())[list(server.pin_map.values()).index(pin)]
        server.send_sensor_update(SensorState(sensor_id, value))

    hardware = HardwareConnector(config["Serial"], on_send=send_sensor_data)

    def received_action_callback(action: Action) -> HardwareReply:
        successful_change = hardware.submit_state(
            pin=str(server.pin_map[action.device_id]),
            type=action.type,
            state=action.state,
        )
        print(action.__dict__)
        print(successful_change)

        return HardwareReply(action.id, successful_change)

    server.add_action_listener(received_action_callback)

    IOLoop.current().start()


def load_config(config_path: str) -> dict:
    config: dict = {}

    with open(config_path, "r") as config_file:
        config = yaml.load(config_file, Loader=yaml.FullLoader)
    return config


if __name__ == "__main__":
    main()
