import json
from typing import Type, TypeVar, Any
from src.hardware_connector.util import value_from_str
from src.models.device_types import DeviceType
from src.models.json_serializable import JsonSerializable

T = TypeVar("T", bound="DeviceState")
G = TypeVar("G")


class DeviceState(JsonSerializable):
    @classmethod
    def from_json(cls: Type[T], json_data: str) -> T:
        data = json.loads(json_data)
        return cls(**data)

    @classmethod
    def from_list(cls: Type[T], state_list: list[str], type: DeviceType) -> T:
        # Create a dictionary mapping attribute names to their values
        state_dict = {}

        for i, val in enumerate(state_list):
            print(val)
            if val != "-1":
                attr_name = type.attribute_names[i]
                print(attr_name)
                state_dict[attr_name] = value_from_str(val, type.attribute_types[i])
                print(value_from_str(val, type.attribute_types[i]))

        # Create a new instance of the DeviceState subclass using the dictionary
        if type == DeviceType.TOGGLE:
            return ToggleState(**state_dict)  # type: ignore
        elif type == DeviceType.DOOR:
            return DoorState(**state_dict)  # type: ignore
        elif type == DeviceType.WINDOW:
            return WindowState(**state_dict)  # type: ignore
        elif type == DeviceType.DISPLAY:
            return DisplayState(**state_dict)  # type: ignore
        elif type == DeviceType.SPEAKER:
            return SpeakerState(**state_dict)  # type: ignore
        else:
            raise ValueError("Invalid device type")

    def to_json(self) -> str:
        return json.dumps(self)

    def to_list(self) -> list[str]:
        state_list = []
        for attr, value in self.__dict__.items():
            print(attr, value)
            if value is None:
                state_list.append("-1")
            else:
                state_list.append(self.value_to_str(value))

        return state_list

    def value_to_str(self, value: Any) -> str:
        if isinstance(value, bool):
            return str(255 if value else 0)

        return str(value)

    def value_from_str(self, string_val: str, expected_type: Any) -> Any:
        if expected_type == bool:
            return True if int(string_val) == 255 else False

        return string_val

    def __eq__(self, __o: object) -> bool:
        if not isinstance(__o, DeviceState):
            return False

        own_state = self.__dict__.values()
        other_state = list(__o.__dict__.values())

        for i, val in enumerate(own_state):
            if val is None or other_state[i] is None:
                continue

            if val != other_state[i]:
                return False

        return True


class ToggleState(DeviceState):
    def __init__(self, on: bool | None):
        self.on = on


class DoorState(DeviceState):
    def __init__(self, open: bool | None = None, locked: bool | None = None):
        self.open = open
        self.locked = locked


class WindowState(DeviceState):
    def __init__(self, open: bool | None = None):
        self.open = open


class DisplayState(DeviceState):
    def __init__(self, on: bool | None = None, text: str | None = None):
        self.on = on
        self.text = text


class SpeakerState(DeviceState):
    def __init__(self, tune: str):
        self.tune = tune


class FanState(DeviceState):
    def __init__(self, on: bool | None = None, reverse: bool | None = None) -> None:
        super().__init__()
