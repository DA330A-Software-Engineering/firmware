from threading import Lock
from src.models.device_types import DeviceType
from src.models.device_state import DeviceState
from typing import Callable
import serial  # type: ignore
import time
from tornado.ioloop import IOLoop


class HardwareConnector:
    def __init__(self, config: dict, on_send: Callable[[int, str], None]) -> None:
        self.config = config
        self.on_send = on_send
        self.serial = serial.Serial(
            port=self.config["Port"],
            baudrate=self.config["baud_rate"],
            timeout=float(self.config["timeout"]),
        )
        self.action_in_progress = False
        self.lock = Lock()
        # self.sensor_thread = threading.Thread(target=self.listen_for_sensor_updates)
        # self.sensor_thread.daemon = True
        # self.is_sensor_thread_running = False

    def submit_state(self, pin: str, deviceType: DeviceType, state: DeviceState) -> bool:
        # ! turn state into list of str (0-255 if bool or number)
        # ! Compile into "pin, attr1, attr2, attr3 ..." str with attr being -1 if not present
        # ! Send comma string to serial
        # ! Receive or get state to check it has updated correctly

        if deviceType == DeviceType.SENSOR:
            return True

        # Convert state to a list of strings (0-255 if bool or number)
        state_list = state.to_list()
        # Compile into "pin, attr1, attr2, attr3 ..." str with attr being -1 if not present
        state_str = ",".join([str(pin)] + state_list)
        print(state_str)
        
        with self.lock:
            # Send comma string to serial
            self.serial.write(state_str.encode())
            # self.on_send(state_str)

            # Receive or get state to check it has updated correctly
            response = self.serial.readline().decode().strip()
            while pin != response[:2] and pin != response[:1]:
                time.sleep(0.1)
                response = self.serial.readline().decode().strip()

        new_set_state = DeviceState.from_list(response.split(",")[1:], deviceType)

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

    def listen_for_sensor_updates(self, loop: IOLoop):
        while True:
            if self.lock.locked():
                continue

            response = self.serial.readline().decode().strip()
            if "," in response:
                pin_num, value = response.split(",")
                loop.add_callback(self.on_send, int(pin_num), value)
                # self.on_send(int(pin_num), value)

    # def start_sensor_thread(self):
    #     self.is_sensor_thread_running = True
    #     self.sensor_thread.start()

    # def stop_sensor_thread(self):
    #     self.is_sensor_thread_running = False
