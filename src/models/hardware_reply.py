import json
from src.models.json_serializable import JsonSerializable


class HardwareReply(JsonSerializable):
    def __init__(self, id: str, ok: bool) -> None:
        self.id = id
        self.ok = ok

    @staticmethod
    def from_json(json_data: str) -> "HardwareReply":
        data_dict = json.loads(json_data)
        return HardwareReply(data_dict["id"], data_dict["ok"])

    def to_json(self) -> str:
        return json.dumps({"id": self.id, "ok": self.ok})
