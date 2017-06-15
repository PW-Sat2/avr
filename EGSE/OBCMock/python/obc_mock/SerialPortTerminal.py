import logging
import serial


class SerialPortTerminal:
    def __init__(self, comPort):
        self.log = logging.getLogger("OBCTerm")
        self._serial = serial.Serial(comPort, baudrate=38400, timeout=1, rtscts=False)

    def waitForPrompt(self, terminator='>'):
        self._serial.reset_input_buffer()
        self._serial.flushInput()

        self._serial.write("\n")
        self._serial.flush()
        c = self._serial.read(1)
        while c != terminator:
            c = self._serial.read(1)

    def readUntilPrompt(self, terminator='>'):
        data = ""
        c = self._serial.read(1)
        while c != terminator:
            data += c
            c = self._serial.read(1)

        return data

    def _safe_read(self, size):
        data = ''

        while size > 0:
            part = self._serial.read(size)

            data += part
            size -= len(part)

        return data

    def _command_prologue(self, cmd):
        self.waitForPrompt()

        self._serial.reset_input_buffer()
        self._serial.reset_output_buffer()
        self._serial.flushInput()
        self._serial.flushOutput()

        self._serial.write(cmd + "\n")
        self._serial.flush()

    def command(self, cmd):
        self._command_prologue(cmd)

        response = self.readUntilPrompt().rstrip('\n')
        self.log.debug("Command " + cmd + " responded with '" + response + "'")
        return response

    def reset(self):
        self._serial.reset_input_buffer()
        self._serial.reset_output_buffer()
        self._serial.flushInput()
        self._serial.flush()

    def close(self):
        self._serial.close()
