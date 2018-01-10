import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'libs'))
import time
from hk import *
from eps import *
from config import config
import formatting
import math

if __name__ == "__main__":
    eps_b = EPS_B()
    eps_b_hk = EPS_B_HK()
    time.sleep(2)

    while True:
        print eps_b_hk.hk_convert(eps_b.read_hk())
        print "\n\n"
        time.sleep(0.5)
