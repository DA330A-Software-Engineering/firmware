from src.models.action import Action
from src.models.hardware_reply import HardwareReply
from src.server.server_connector import ServerConnector
from src.hardware_connector.hardware_connector_placeholder import HardwareConnector
import yaml  # type: ignore


def main() -> None:
    config = load_config("./config.yaml")

    with ServerConnector(config) as server:
        hardware = HardwareConnector(config)

        def received_action_callback(action: Action) -> HardwareReply:
            successful_change = hardware.submit_state(
                pin=server.pin_map[action.device_id],
                type=action.type,
                state=action.state,
            )

            return HardwareReply(action.id, successful_change)

        server.run_action_socket([received_action_callback])


def load_config(config_path: str) -> dict:
    config: dict = {}

    with open(config_path, "r") as config_file:
        config = yaml.load(config_file, Loader=yaml.FullLoader)
    return config


if __name__ == "__main__":
    main()
