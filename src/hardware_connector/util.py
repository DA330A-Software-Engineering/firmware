from typing import Type, Any


def value_from_str(string_val: str, expected_type: Type) -> Any:
    if expected_type == bool:
        return int(string_val) == 255

    return string_val
