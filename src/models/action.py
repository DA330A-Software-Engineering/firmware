from .device_state import DeviceState, ToggleState, DisplayState, DoorState, SpeakerState, WindowState
from .device_types import DeviceTypes
import json


class Action(object):
    def __init__(self, id: str, state: DeviceState, type: DeviceTypes) -> None:
        self.id = id
        self.state = state
        self.type = type

    @staticmethod
    def from_json(json_data: str) -> "Action":
        data_dict = json.loads(json_data)

        device_type = DeviceTypes(int(data_dict["type"]))

        state_object = json.loads(data_dict["state"])

        match device_type:
            case DeviceTypes.TOGGLE:
                return Action(data_dict["id"], ToggleState(**state_object), device_type)
            case DeviceTypes.SPEAKER:
                return Action(data_dict["id"], SpeakerState(**state_object), device_type)
            case DeviceTypes.WINDOW:
                return Action(data_dict["id"], WindowState(**state_object), device_type)
            case DeviceTypes.DISPLAY:
                return Action(data_dict["id"], DisplayState(**state_object), device_type)
            case DeviceTypes.DOOR:
                return Action(data_dict["id"], DoorState(**state_object), device_type)
            case _:
                raise ValueError("The type must be specified to be one of the availble DeviceTypes")
