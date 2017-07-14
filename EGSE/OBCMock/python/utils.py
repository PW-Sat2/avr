import struct

def split(data, split):
    return [data[i:i + split] for i in xrange(0, len(data), split)]
def parse_into_uint16(data):
    return [struct.unpack('<H', bytearray(now))[0] for now in split(data, 2)]
def parse_into_uint32(data):
    return [struct.unpack('<I', bytearray(now))[0] for now in split(data, 4)]
