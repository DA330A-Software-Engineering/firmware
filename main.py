from src.models.action import Action
from src.models.hardware_reply import HardwareReply
from src.server.server_connector import ServerConnector
from src.hardware_connector.hardware_connector_placeholder import HardwareConnector


def main() -> None:
    with ServerConnector("./config.yaml") as server:
        hardware = HardwareConnector()

        def received_action_callback(action: Action) -> HardwareReply:
            successful_change = hardware.submit_state(
                pin=server.pin_map[action.device_id],
                type=action.type,
                state=action.state,
            )

            return HardwareReply(action.id, successful_change)

        server.run_action_socket([received_action_callback])


if __name__ == "__main__":
    main()
