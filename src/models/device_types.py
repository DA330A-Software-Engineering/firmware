from enum import Enum

# from src.models.device_state import DeviceState, ToggleState, WindowState, DoorState, DisplayState, SpeakerState


class DeviceType(Enum):
    TOGGLE = 0
    SPEAKER = 1
    WINDOW = 2
    DISPLAY = 3
    DOOR = 4
    BUZZER = 5
    FAN = 6

    @classmethod
    def from_str(cls, str_val: str) -> "DeviceType":
        match str_val:
            case "toggle":
                return cls.TOGGLE
            case "buzzer":
                return cls.SPEAKER
            case "window":
                return cls.WINDOW
            case "screen":
                return cls.DISPLAY
            case "door":
                return cls.DOOR
            case "fan":
                return cls.FAN
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
        elif self == DeviceType.DOOR:
            return ["open", "locked"]
        elif self == DeviceType.WINDOW:
            return ["open"]
        elif self == DeviceType.DISPLAY:
            return ["on", "text"]
        elif self == DeviceType.SPEAKER:
            return ["duration"]
        else:
            raise ValueError("Invalid device type")

    @property
    def attribute_types(self) -> list[type]:
        if self == DeviceType.TOGGLE:
            return [bool]
        elif self == DeviceType.DOOR:
            return [bool, bool]
        elif self == DeviceType.WINDOW:
            return [bool]
        elif self == DeviceType.DISPLAY:
            return [bool, str]
        elif self == DeviceType.SPEAKER:
            return [str]
        else:
            raise ValueError("Invalid device type")
