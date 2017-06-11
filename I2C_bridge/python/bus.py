import time
import serial

class UsbI2C:
    def __init__(self, dev):
        self.serial = serial.Serial(dev, 38400)
        self.serial.flush()
        self.serial.flushInput()
        self.serial.flushOutput()

        self.serial.write("\n")
        self.serial.write("\n")
        time.sleep(0.1)
        s = self.serial.read_all()
        if s.strip() != ">\n>":
            print "Wrong! |", s, "|"


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

if __name__ == "__main__":
    dev = UsbI2C('/dev/ttyACM3')
    # dev.write(26, [0x81])
    # print "A:", dev.write_read(26, [0], 6)
    # print "B:", dev.write_read(26, [1], 5)

    # dev.write(26, [0x80])
    print "A:", dev.write_read(26, [0], 6)
    time.sleep(0.01)
    dev.write(26, [0x81])
    # time.sleep(0.1)
    # dev.write(26, [0x81])
    # dev.write(26, [0x82])

    print "C:", dev.write_read(26, [0], 6)
    # print "D:", dev.write_read(26, [1], 5)