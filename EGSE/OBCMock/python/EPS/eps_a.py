import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'libs'))
import time
from hk import *
from eps import *
from config import config
from log import *
import formatting
import curses
import control
import math

format = formatting.Formatting()
screen = curses.initscr()
curses.start_color()
keyboard = control.KeyGetterLinux()

    
log = Log()

print "Log file name: " + log.file_name
print log.info(["Started"])

if __name__ == "__main__":
    eps_a = EPS_A()
    eps_a_hk = EPS_A_HK()
    time.sleep(2)

    curses.init_pair(1, curses.COLOR_GREEN, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_YELLOW, curses.COLOR_BLACK)
    curses.init_pair(3, curses.COLOR_RED, curses.COLOR_BLACK)
    curses.init_pair(4, curses.COLOR_BLUE, curses.COLOR_BLACK)
    curses.init_pair(5, curses.COLOR_MAGENTA, curses.COLOR_BLACK)

    curses.curs_set(0)
    curses.noecho()
    curses.nocbreak()

    target_hk_refresh_freq = config.get("HK_REFRESH_FREQ")
    current_hk_refresh_freq = target_hk_refresh_freq
    measured_hk_refresh_freq = target_hk_refresh_freq

    last_command = "last command..."
    obc_watchdog_reset_counter = 0

    while True:
        time_before = time.time()

        hk_converted = eps_a_hk.hk_convert(eps_a.read_hk())

        log.hk(hk_converted)

        screen.clear()

        screen.addstr(1, 3, " " + hk_converted[0][1] + " ", curses.color_pair(hk_converted[0][3]+1) | curses.A_REVERSE)
        screen.addstr(1, 13, hk_converted[0][2] + " ", curses.color_pair(hk_converted[0][3]+1) | curses.A_REVERSE)

        screen.addstr(3, 3, "     " + hk_converted[1][0] + "     ", curses.A_REVERSE)
        for i in range(1, 6):
            screen.addstr(i+3, 3, hk_converted[i][1])
            screen.addstr(i+3, 13, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(10, 3, "     " + hk_converted[6][0] + "    ", curses.A_REVERSE)
        for i in range(6, 11):
            screen.addstr(i+5, 3, hk_converted[i][1])
            screen.addstr(i+5, 13, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(17, 3, "     " + hk_converted[11][0] + "    ", curses.A_REVERSE)
        for i in range(11, 16):
            screen.addstr(i+7, 3, hk_converted[i][1])
            screen.addstr(i+7, 13, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(3, 22, "      " + hk_converted[16][0] + "      ", curses.A_REVERSE)
        for i in range(16, 24):
            screen.addstr(i-12, 22, hk_converted[i][1])
            screen.addstr(i-12, 33, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(13, 22, "      " + hk_converted[24][0] + "       ", curses.A_REVERSE)
        for i in range(24, 29):
            screen.addstr(i-10, 22, hk_converted[i][1])
            screen.addstr(i-10, 33, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(20, 22, "       " + hk_converted[29][0] + "        ", curses.A_REVERSE)
        for i in range(29, 31):
            screen.addstr(i-8, 22, hk_converted[i][1])
            screen.addstr(i-8, 33, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(3, 43, "      " + hk_converted[31][0] + "      ", curses.A_REVERSE)
        screen.addstr(4, 43, hk_converted[31][1])
        screen.addstr(4, 54, hk_converted[31][2], curses.color_pair(hk_converted[31][3]+1))

        screen.addstr(6, 43, "      " + hk_converted[32][0] + "      ", curses.A_REVERSE)
        for i in range(32, 37):
            screen.addstr(i-25, 43, hk_converted[i][1])
            screen.addstr(i-25, 54, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        screen.addstr(13, 43, "      " + hk_converted[37][0] + "       ", curses.A_REVERSE)
        for i in range(37, 39):
            screen.addstr(i-23, 43, hk_converted[i][1])
            screen.addstr(i-23, 54, hk_converted[i][2], curses.color_pair(hk_converted[i][3]+1))

        key_pressed = keyboard.getch(False)
        
        if len(key_pressed)>1:
            key_pressed = ""
        
        if len(key_pressed)==1:
            if key_pressed=="q":
                break

            elif key_pressed=="!":
                eps_a.lcl_enable(EPS_A_LCL.TKmain)
                last_command = "LCL ON: TKmain"
            elif key_pressed=="@":
                eps_a.lcl_enable(EPS_A_LCL.SunS)
                last_command = "LCL ON: SunS"
            elif key_pressed=="#":
                eps_a.lcl_enable(EPS_A_LCL.CAMnadir)
                last_command = "LCL ON: CAMnadir"
            elif key_pressed=="$":
                eps_a.lcl_enable(EPS_A_LCL.CAMwing)
                last_command = "LCL ON: CAMwing"
            elif key_pressed=="%":
                eps_a.lcl_enable(EPS_A_LCL.SENS)
                last_command = "LCL ON: SENS"
            elif key_pressed=="^":
                eps_a.lcl_enable(EPS_A_LCL.ANTenna)
                last_command = "LCL ON: ANTenna"
            elif key_pressed=="&":
                eps_a.lcl_enable(EPS_A_LCL.iMTQ)
                last_command = "LCL ON: iMTQ"
            elif key_pressed=="*":
                eps_a.lcl_enable(EPS_A_LCL.ALL)
                last_command = "LCL ON: ALL"    

            elif key_pressed=="1":
                eps_a.lcl_disable(EPS_A_LCL.TKmain)
                last_command = "LCL OFF: TKmain"
            elif key_pressed=="2":
                eps_a.lcl_disable(EPS_A_LCL.SunS)
                last_command = "LCL OFF: SunS"
            elif key_pressed=="3":
                eps_a.lcl_disable(EPS_A_LCL.CAMnadir)
                last_command = "LCL OFF: CAMnadir"
            elif key_pressed=="4":
                eps_a.lcl_disable(EPS_A_LCL.CAMwing)
                last_command = "LCL OFF: CAMwing"
            elif key_pressed=="5":
                eps_a.lcl_disable(EPS_A_LCL.SENS)
                last_command = "LCL OFF: SENS"
            elif key_pressed=="6":
                eps_a.lcl_disable(EPS_A_LCL.ANTenna)
                last_command = "LCL OFF: ANTenna"
            elif key_pressed=="7":
                eps_a.lcl_disable(EPS_A_LCL.iMTQ)
                last_command = "LCL OFF: iMTQ"
            elif key_pressed=="8":
                eps_a.lcl_disable(EPS_A_LCL.ALL)
                last_command = "LCL OFF: ALL"
        
            elif key_pressed=="p":
                eps_a.power_cycle()
                last_command = "Power Cycle"

            elif key_pressed=="o":
                eps_a.disable_overheat_submode()
                last_command = "Disable Overheat Prot."
            
            elif key_pressed=="n":
                eps_a.burn_enable(EPS_A_BURN.SAILmain)
                last_command = "BURN ON: SAILmain"
            elif key_pressed=="m":
                eps_a.burn_enable(EPS_A_BURN.SADSmain)
                last_command = "BURN ON: SADSmain"

        screen.addstr(18, 43, "  HK freq.  " + str( round(measured_hk_refresh_freq, 1) ) + "  ", curses.color_pair(4) | curses.A_REVERSE)
        screen.addstr(20, 43, "  LAST COMMAND   ", curses.color_pair(5) | curses.A_REVERSE)
        screen.addstr(21, 43, last_command, curses.color_pair(5))

        if obc_watchdog_reset_counter>0:
            obc_watchdog_reset_counter = obc_watchdog_reset_counter - 1
        else:
            obc_watchdog_reset_counter = config.get("OBC_WATCHDOG_RESET_FREQ")
            eps_a.reset_obc_watchdog()
            screen.addstr(17, 43, "  OBC WDT reset  ", curses.color_pair(4) | curses.A_REVERSE)

        screen.refresh()

        time.sleep(1.0/current_hk_refresh_freq)

        measured_hk_refresh_freq = 1.0/(time.time()-time_before)
        if measured_hk_refresh_freq > target_hk_refresh_freq:
            current_hk_refresh_freq = current_hk_refresh_freq * 0.97
        elif measured_hk_refresh_freq < target_hk_refresh_freq:
            current_hk_refresh_freq = current_hk_refresh_freq * 1.03

    curses.endwin()
