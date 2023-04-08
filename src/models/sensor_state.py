from dataclasses import dataclass
from typing import Any
import json
from src.models.json_serializable import JsonSerializable


@dataclass
class SensorState(JsonSerializable):
    id: str
    state: int

    @staticmethod
    def from_json(json_data: str) -> Any:
        data = json.loads(json_data)
        return SensorState(**data)

    def to_json(self) -> str:
        return json.dumps(self)
