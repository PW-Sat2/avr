import time
from utils import *

from obc_mock.commands import Commands
from obc_mock.SerialPortTerminal import SerialPortTerminal

class PLDMixin:
    def measure(self, type):
        pass
    def read(self, type):
        pass
    def measure_and_read(self, type):
        pass


class WhoAmI(PLDMixin):
    offset = 0
    length = 1

    def read_who_am_i(self):
        return self.read(WhoAmI)[0]


class SunsRef(PLDMixin):
    opcode = 0x80
    offset = 1
    length = 10

    def read_sunsref(self):
        data_array = self.measure_and_read(SunsRef)
        return parse_into_uint16(data_array)


class Temperatures(PLDMixin):
    opcode = 0x81
    offset = 11
    length = 18

    def read_temperatures(self):
        data_array = self.measure_and_read(Temperatures)
        data = parse_into_uint16(data_array)
        return {"supply": data[0],
                "Xp": data[1],
                "Xn": data[2],
                "Yp": data[3],
                "Yn": data[4],
                "sads": data[5],
                "sail": data[6],
                "cam_nadir": data[7],
                "cam_wing": data[8]}


class Photodiodes(PLDMixin):
    opcode = 0x82
    offset = 29
    length = 8

    def read_photodiodes(self):
        data_array = self.measure_and_read(Photodiodes)
        data = parse_into_uint16(data_array)
        return {"Xp": data[0],
                "Xn": data[1],
                "Yp": data[2],
                "Yn": data[3]}


class Housekeeping(PLDMixin):
    opcode = 0x83
    offset = 37
    length = 4

    def read_housekeeping(self):
        data_array = self.measure_and_read(Housekeeping)
        data = parse_into_uint16(data_array)
        return {"int_3v3d": data[0],
                "obc_3v3d": data[1]}


class RadFET(PLDMixin):
    opcode = 0x84
    offset = 41
    length = 16

    def read_radfet(self):
        data_array = self.measure_and_read(RadFET)
        data = parse_into_uint32(data_array)
        return {"temp": data[0],
                "Vth": data[1:]}


class PLD(WhoAmI, SunsRef, Temperatures, Photodiodes, Housekeeping, RadFET):
    def __init__(self, dev):
        self.iface = Commands(dev)
        self.address = 0x30

    def measure_and_read(self, type):
        self.measure(type)
        return self.read(type)

    def measure(self, type):
        if self.iface.pin():
            raise RuntimeError("INT line high!")
        self.iface.get_events()
        self.iface.write(self.address, [type.opcode])
        self._wait_for_pulse()

    def read(self, type):
        return self.iface.write_read(self.address, [type.offset], type.length)

    def _wait_for_pulse(self):
        while self.iface.get_events().count(1) == 0:
            pass
        while self.iface.get_events().count(0) == 0:
            pass


if __name__ == "__main__":
    pld = PLD(SerialPortTerminal('/dev/ttyACM3'))
    time.sleep(3)

    while True:
        print pld.read_who_am_i()
        print pld.read_sunsref()
        print pld.read_temperatures()
        print pld.read_photodiodes()
        print pld.read_housekeeping()
        print pld.read_radfet()
