from __future__ import annotations
from .device_state import DeviceState, FanState, SensorState, ToggleState, DisplayState, OpenLockState, SpeakerState
from .device_types import DeviceType
from src.models.json_serializable import JsonSerializable
import json


class Action(JsonSerializable, object):
    def __init__(self, id: str, device_id: str, state: DeviceState, type: DeviceType) -> None:
        self.id = id
        self.device_id = device_id
        self.state = state
        self.type = type

    @staticmethod
    def from_json(json_data: str) -> Action:
        data_dict = json.loads(json_data)
        device_type = DeviceType.from_str(data_dict["type"])

        state_object = data_dict["state"]

        match device_type:
            case DeviceType.TOGGLE:
                return Action(data_dict["id"], data_dict["deviceId"], ToggleState(**state_object), device_type)
            case DeviceType.SPEAKER:
                return Action(data_dict["id"], data_dict["deviceId"], SpeakerState(**state_object), device_type)
            case DeviceType.DISPLAY:
                return Action(data_dict["id"], data_dict["deviceId"], DisplayState(**state_object), device_type)
            case DeviceType.OPENLOCK:
                return Action(data_dict["id"], data_dict["deviceId"], OpenLockState(**state_object), device_type)
            case DeviceType.FAN:
                return Action(data_dict["id"], data_dict["deviceId"], FanState(**state_object), device_type)
            case DeviceType.SENSOR:
                return Action(data_dict["id"], data_dict["deviceId"], SensorState(), device_type)
            case _:
                raise ValueError("The type must be specified to be one of the availble DeviceTypes")

    def to_json(self) -> str:
        return json.dumps(
            {
                "id": self.id,
                "deviceId": self.device_id,
                "type": self.type.name,
                "state": self.state.to_json(),
            }
        )

    def __str__(self) -> str:
        return str(self.__dict__)
