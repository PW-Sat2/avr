import time
import serial
from obc import OBC
from SerialPortTerminal import SerialPortTerminal


class UsbI2C:
    def __init__(self, dev):
        self.obc = OBC(SerialPortTerminal(dev))


if __name__ == "__main__":
    bus = UsbI2C('/dev/ttyACM0')
    time.sleep(1)
    print "Init"
    bus.obc.led(1)
    time.sleep(1)
    bus.obc.led(0)

    while True:
        print bus.obc.get_events()
        time.sleep(1)

    #
    #
    # def write(self, addr, data):
    #     _data = [str(i) for i in data]
    #     self.serial.write("w %d " % addr + " ".join(_data) + "\n")
    #
    #     self.serial.read_until(terminator='>')
    #
    #
    # def read(self, addr, bytes):
    #     s = "r %d %d " % (addr, bytes) + "\n"
    #     self.serial.write(s)
    #     line = self.serial.read_until(terminator='\n')
    #
    #     self.serial.read_until(terminator='>')
    #     return [int(i) for i in line.strip().split(" ")]
    #
    #
    #
    # def write_read(self, addr, write, read_bytes):
    #     _write = [str(i) for i in write]
    #     s = "wr %d " % addr + " ".join(_write) + " %d\n" % read_bytes
    #     self.serial.write(s)
    #     line = self.serial.read_until(terminator='\n')
    #
    #     self.serial.read_until(terminator='>')
    #     return [int(i) for i in line.strip().split(" ")]
