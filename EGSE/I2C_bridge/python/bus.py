import time
import serial

class UsbI2C:
    def __init__(self, dev):
        self.serial = serial.Serial(dev, 250000)
        print self.serial
        self.serial.flush()
        self.serial.flushInput()
        self.serial.flushOutput()

    def write(self, addr, data):
        _data = [str(i) for i in data]
        self.serial.write("w %d " % addr + " ".join(_data) + "\n")

        self.serial.read_until(terminator='>')


    def read(self, addr, bytes):
        s = "r %d %d " % (addr, bytes) + "\n"
        self.serial.write(s)
        line = self.serial.read_until(terminator='\n')

        self.serial.read_until(terminator='>')
        return [int(i) for i in line.strip().split(" ")]



    def write_read(self, addr, write, read_bytes):
        _write = [str(i) for i in write]
        s = "wr %d " % addr + " ".join(_write) + " %d\n" % read_bytes
        self.serial.write(s)
        line = self.serial.read_until(terminator='\n')

        self.serial.read_until(terminator='>')
        return [int(i) for i in line.strip().split(" ")]
