from dataclasses import dataclass
import json
from src.models.json_serializable import JsonSerializable


@dataclass
class SensorState(JsonSerializable):
    id: str
    state: dict[str, str]

    @staticmethod
    def from_json(json_data: str) -> "SensorState":
        data = json.loads(json_data)
        return SensorState(**data)

    def to_json(self) -> str:
        return json.dumps(self.__dict__)
