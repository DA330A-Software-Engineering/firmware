from enum import Enum

# from src.models.device_state import DeviceState, ToggleState, WindowState, DoorState, DisplayState, SpeakerState


class DeviceType(Enum):
    TOGGLE = 0
    SPEAKER = 1
    DISPLAY = 2
    OPENLOCK = 3
    FAN = 4
    SENSOR = 5

    @classmethod
    def from_str(cls, str_val: str) -> "DeviceType":
        match str_val:
            case "toggle":
                return cls.TOGGLE
            case "buzzer":
                return cls.SPEAKER
            case "screen":
                return cls.DISPLAY
            case "openLock":
                return cls.OPENLOCK
            case "fan":
                return cls.FAN
            case "sensor":
                return cls.SENSOR
            case _:
                raise ValueError("Invalid device type")

    # def default_state(self) -> "DeviceState":
    #     if self == DeviceType.TOGGLE:
    #         return ToggleState(False)
    #     elif self == DeviceType.DOOR:
    #         return DoorState(False, False)
    #     elif self == DeviceType.WINDOW:
    #         return WindowState(False)
    #     elif self == DeviceType.DISPLAY:
    #         return DisplayState(False, "")
    #     elif self == DeviceType.SPEAKER:
    #         return SpeakerState("")
    #     else:
    #         raise ValueError("Invalid device type")

    @property
    def attribute_names(self) -> list[str]:
        if self == DeviceType.TOGGLE:
            return ["on"]
        elif self == DeviceType.OPENLOCK:
            return ["open", "locked"]
        elif self == DeviceType.DISPLAY:
            return ["on", "text"]
        elif self == DeviceType.SPEAKER:
            return ["tune"]
        elif self == DeviceType.FAN:
            return ["on", "reverse"]
        else:
            raise ValueError("Invalid device type")

    @property
    def attribute_types(self) -> list[type]:
        if self == DeviceType.TOGGLE:
            return [bool]
        elif self == DeviceType.OPENLOCK:
            return [bool, bool]
        elif self == DeviceType.DISPLAY:
            return [bool, str]
        elif self == DeviceType.SPEAKER:
            return [str]
        elif self == DeviceType.FAN:
            return [bool, bool]
        else:
            raise ValueError("Invalid device type")
