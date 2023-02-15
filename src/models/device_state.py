import json
from typing import Type, TypeVar

T = TypeVar("T", bound="DeviceState")


class DeviceState(object):
    @classmethod
    def from_json(cls: Type[T], json_data: str) -> T:
        data = json.loads(json_data)
        return cls(**data)

    def to_json(self) -> str:
        return json.dumps(self)


class ToggleState(DeviceState):
    def __init__(self, on: bool | None):
        self.on = on


class DoorState(DeviceState):
    def __init__(self, open: bool | None, locked: bool | None):
        self.open = open
        self.locked = locked


class WindowState(DeviceState):
    def __init__(self, open: bool | None):
        self.open = open


class DisplayState(DeviceState):
    def __init__(self, on: bool | None, text: str | None):
        self.on = on
        self.text = text


class SpeakerState(DeviceState):
    def __init__(self, duration: str | None):
        self.duration = duration
