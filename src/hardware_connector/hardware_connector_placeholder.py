from src.models.device_types import DeviceType
from src.models.device_state import DeviceState


class HardwareConnector:
    def __init__(self) -> None:
        pass

    def __enter__(self) -> "HardwareConnector":
        return self

    def __exit__(self, type, value, traceback):
        pass

    def submit_state(self, pin: str, type: DeviceType, state: DeviceState) -> bool:
        return False
