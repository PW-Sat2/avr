import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '../..'))
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from utils import *
from config import config

from obc_mock.commands import Commands
from obc_mock.SerialPortTerminal import SerialPortTerminal

class HkA():
    offset = 0
    length = 72
    def read_hk(self):
        return self.read(HkA)

class HkB():
    offset = 0
    length = 18
    def read_hk(self):
        return self.read(HkB)
        
class PowerCycle():
    opcode = 0xE0

    def power_cycle(self):
        self.write(PowerCycle)

class LclEnable():
    opcode = 0xE1

    def lcl_enable(self, lcl_id):
        self.write_with_parameter(LclEnable, lcl_id)

class LclDisable():
    opcode = 0xE2

    def lcl_disable(self, lcl_id):
        self.write_with_parameter(LclDisable, lcl_id)

class BurnEnable():
    opcode = 0xE3

    def burn_enable(self, burn_id):
        self.write_with_parameter(BurnEnable, burn_id)
 
class DisableOverheatSubmode():
    opcode = 0xE4

    def disable_overheat_submode(self):
        self.write(DisableOverheatSubmode)

class ResetObcWatchdog():
    opcode = 0xE5
    
    def reset_obc_watchdog(self):
        self.write(ResetObcWatchdog)

class EPS(PowerCycle, LclEnable, LclDisable, BurnEnable, DisableOverheatSubmode, ResetObcWatchdog):
    def read(self, type):
        return self.iface.write_read(self.address, [type.offset], type.length)
        
    def write_with_parameter(self, type, parameter):
        if isinstance(parameter, list):
            for i in range(0, len(parameter)):
                self.iface.write(self.address, [type.opcode, parameter[i]])
        else:
            self.iface.write(self.address, [type.opcode, parameter])
        
    def write(self, type):
        self.iface.write(self.address, [type.opcode])

class EPS_A(EPS, HkA):
    def __init__(self):
        self.iface = Commands(SerialPortTerminal(config.get("EPS_A_I2C_BRIDGE_COM")))
        self.address = 0x35
        
class EPS_B(EPS, HkB):
    def __init__(self):
        self.iface = Commands(SerialPortTerminal(config.get("EPS_B_I2C_BRIDGE_COM")))
        self.address = 0x36

class EPS_A_LCL:
    TKmain = 1
    SunS = 2
    CAMnadir = 3
    CAMwing = 4
    SENS = 5
    ANTenna = 6
    iMTQ = 7
    ALL = [TKmain, SunS, CAMnadir, CAMwing, SENS, ANTenna, iMTQ]

class EPS_A_BURN:
    SAILmain = 1
    SADSmain = 2

class EPS_B_LCL:
    TKred = 1
    ANTenna = 2
    ALL = [TKred, ANTenna]

class EPS_B_BURN:
    SAILred = 1
    SADSred = 2
