from string import Formatter
import logging
from mock import Mock

class OBC(Mock):
    def __init__(self, terminal):
        self.log = logging.getLogger("OBC")

        self._formatter = Formatter()

        self._terminal = terminal
        self._terminal.reset()

    def _command(self, cmd, *args, **kwargs):
        cmdline = self._formatter.vformat(cmd, args, kwargs)
        return self._terminal.command(cmdline)

    def _command_no_wait(self, cmd, *args, **kwargs):
        cmdline = self._formatter.vformat(cmd, args, kwargs)

        return self._terminal.command_no_wait(cmdline)

    def reset(self):
        self._terminal.reset()

    def close(self):
        self._terminal.close()
