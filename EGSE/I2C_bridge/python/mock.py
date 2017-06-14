from obc_mixin import OBCMixin, command, decode_bool, decode_return


class Event:
    low_to_high = 1
    high_to_low = 0

class Mock(OBCMixin):
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

    def dx(result):
        print "a", result

    @decode_return(dx)
    def write(self, addr, data):
        _data = [str(i) for i in data]
        self._command("w %d " % addr + " ".join(_data) + "\n")
