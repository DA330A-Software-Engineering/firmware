from ast import Tuple
from src.models.device_types import DeviceType
from src.models.device_state import DeviceState
from typing import Callable, Optional
import serial  # type: ignore
import time


class HardwareConnector:
    def __init__(
        self,
        config: dict,
        on_receive: Callable[[str], None] | None = None,
        on_send: Callable[[int, int], None] | None = None,
    ) -> None:
        self.config = config
        self.on_receive = on_receive
        # self.on_send = on_send
        self.serial = serial.Serial(
            port=self.config["Port"],
            baudrate=self.config["baud_rate"],
            timeout=float(self.config["timeout"]),
        )

    def submit_state(self, pin: str, type: DeviceType, state: DeviceState) -> bool:
        # ! turn state into list of str (0-255 if bool or number)
        # ! Compile into "pin, attr1, attr2, attr3 ..." str with attr being -1 if not present
        # ! Send comma string to serial
        # ! Receive or get state to check it has updated correctly
        # Convert state to a list of strings (0-255 if bool or number)
        state_list = state.to_list()

        # Compile into "pin, attr1, attr2, attr3 ..." str with attr being -1 if not present
        state_str = ",".join([str(pin)] + state_list)

        # Send comma string to serial
        self.serial.write(state_str.encode())
        # self.on_send(state_str)

        # Receive or get state to check it has updated correctly
        response = self.serial.readline().decode().strip()
        while pin not in response:
            response = self.serial.readline().decode().strip()
        print(response)
        # self.on_receive(response)

        new_set_state = DeviceState.from_list(response.split(",")[1:], type)
        print(new_set_state.__dict__)
        print(state.__dict__)

        return new_set_state == state

    def get_state(self, pin: str, type: DeviceType) -> DeviceState:
        # ! Access current state on hardware
        # ! Compile it to DeviceState object
        # ! Return device state

        # Send get state command to hardware
        state_str = ",".join([str(pin), str(type.value), "-1", "-1", "-1", "-1"])
        self.serial.write(state_str.encode())

        # Wait for response
        time.sleep(0.1)
        response = self.serial.readline().decode().strip()

        # Parse response into DeviceState object
        state_list = response.split(",")
        if len(state_list) < 2:
            # Invalid response, return default state
            return type.default_state()
        state_obj = DeviceState.from_list(state_list[1:], type)

        return state_obj

        # return ToggleState(False)

    
    def get_sensor_data(self) -> Optional[Tuple[str, int]]:
        command = "get_sensor_data"
        self.serial.write(command.encode())
        
        time.sleep(0.1)
        response = self.serial.readline().decode().strip()
        
        if "," in response:
            pin_num, value_str = response.split(",")
            value = int(value_str)
            return (pin_num, value)
        else:
            return None

    # def get_sensor_data(self, sensor_id: str) -> Optional[Tuple[str, float]]:
    #     command = f"get_sensor_data,{sensor_id}"
    #     self.serial.write(command.encode())
        
    #     time.sleep(0.1)
    #     response = self.serial.readline().decode().strip()
        
    #     if "," in response:
    #         pin_num, value_str = response.split(",")
    #         value = float(value_str)
    #         return (pin_num, value)
    #     else:
    #         return None
        
    
    # def get_sensor_data_cb(self, sensor_pin_number: int, callback: Callable[[str, float], None]) -> None:
    
    #  command = f"get_sensor_data,{sensor_pin_number}"
    #  self.serial.write(command.encode())

    #  time.sleep(0.1)
    #  response = self.serial.readline().decode().strip()

    #  pin_num, value_str = response.split(",")
    #  value = float(value_str)
    #  callback(str(pin_num), value)


    
    # def get_sensor_data(self, sensor_id: str) -> Optional[float]:
    #     pin_num = None
    #     if sensor_id == "photocell":
    #         pin_num = "A1"
    #     elif sensor_id == "motion":
    #         pin_num = 2
    #     elif sensor_id == "steam":
    #         pin_num = "A3"
    #     elif sensor_id == "soil":
    #         pin_num = "A2"
    #     elif sensor_id == "gas":
    #         pin_num = "A0"
    #     else:
    #         return None  

    
    #     command = f"get_sensor_data,{pin_num}"
    #     self.serial.write(command.encode())

    
    #     time.sleep(0.1)
    #     response = self.serial.readline().decode().strip()

    
    #     try:
    #         _, value_str = response.split(":")
    #         value = float(value_str)
    #         return value
    #     except (ValueError, TypeError):
    #         return None 

