from ast import Tuple
from src.models.device_types import DeviceType
from src.models.device_state import DeviceState
from typing import Callable, Optional
import serial  # type: ignore
import time
import threading


class HardwareConnector:
    def __init__(
        self,
        config: dict,
        on_receive: Callable[[str], None] | None = None,
        on_send: Callable[[int, int], None] | None = None,
    ) -> None:
        self.config = config
        self.on_receive = on_receive
        self.on_send = on_send
        self.serial = serial.Serial(
            port=self.config["Port"],
            baudrate=self.config["baud_rate"],
            timeout=float(self.config["timeout"]),
        )
        self.action_in_progress = False
        self.sensor_thread = threading.Thread(target=self.listen_for_sensor_updates)
        self.sensor_thread.daemon = True
        self.is_sensor_thread_running = False

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

        self.action_in_progress = True

        # Receive or get state to check it has updated correctly
        response = self.serial.readline().decode().strip()
        while pin not in response:
            response = self.serial.readline().decode().strip()
        print(response)
        # self.on_receive(response)

        self.action_in_progress = False

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

    
    def listen_for_sensor_updates(self):
        while True:
            if self.is_sensor_thread_running and not self.action_in_progress:
                response = self.serial.readline().decode().strip()
                if "," in response:
                    pin_num, value = response.split(",")
                    if self.on_send:  # on_send callback with the pin_num and value
                        self.on_send(pin_num, value)

    
    def start_sensor_thread(self):
        self.is_sensor_thread_running = True
        self.sensor_thread.start()

    
    def stop_sensor_thread(self):
        self.is_sensor_thread_running = False
