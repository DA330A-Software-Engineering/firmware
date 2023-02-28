from src.models.device_types import DeviceType
from src.models.device_state import DeviceState, ToggleState


class HardwareConnector:
    def __init__(self, config: dict) -> None:
        self.config = config

    def submit_state(self, pin: str, type: DeviceType, state: DeviceState) -> bool:
        # ! turn state into list of str (0-255 if bool or number)
        # ! Compile into "pin, attr1, attr2, attr3 ..." str with attr being -1 if not present
        # ! Send comma string to serial
        # ! Receive or get state to check it has updated correctly

        return True

    def get_state(self, pin: str, type: DeviceType) -> DeviceState:
        # ! Access current state on hardware
        # ! Compile it to DeviceState object
        # ! Return device state

        return ToggleState(False)
