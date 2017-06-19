from commands_mixin import CommandsMixin, command, decode_bool, decode_return, decode_ints


class Event:
    low_to_high = 1
    high_to_low = 0


class Mock(CommandsMixin):
    @command("led {0}")
    def led(self):
        pass

    @decode_bool()
    @command("pin")
    def pin(self):
        pass

    def _parse_events(result):
        events = []
        map = {"0": Event.high_to_low, "1": Event.low_to_high}
        for i in result:
            events.append(map[i])

        return events

    @decode_return(_parse_events)
    @command("ev")
    def get_events(self):
        pass

    def write(self, addr, data):
        _data = [str(i) for i in data]
        self._command("w %d " % addr + " ".join(_data))

    @decode_ints()
    @command("r {0} {1}")
    def read(self, addr, n_bytes):
        pass

    @decode_ints()
    def write_read(self, addr, write, read_bytes):
        _write = [str(i) for i in write]
        return self._command("wr %d " % addr + " ".join(_write) + " %d" % read_bytes)
