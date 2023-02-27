from abc import abstractmethod
from typing import Any


class JsonSerializable:
    @staticmethod
    @abstractmethod
    def from_json(json_data: str) -> Any:
        raise NotImplementedError

    @abstractmethod
    def to_json(self) -> str:
        raise NotImplementedError
