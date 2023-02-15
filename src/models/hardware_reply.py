import json


class HardwareReply(object):
    def __init__(self, id: str, state: int) -> None:
        self.id = id
        self.state = state

    def to_json(self) -> str:
        return json.dumps(self)
