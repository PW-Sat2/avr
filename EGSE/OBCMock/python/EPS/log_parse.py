import matplotlib.pyplot as plt
import argparse
import numpy

parser = argparse.ArgumentParser(description='Parse log file.')
parser.add_argument('log', metavar='N', type=str, nargs='+',
                    help='File to parse.')

args = parser.parse_args()
file_name = str(args.log).replace("[\'","").replace("\']","")

file = open(file_name,'r')
file_to_parse = file.read()
file.close()

file_lines = file_to_parse.split("\n")
hk_list = []
for i in range(1, len(file_lines)):
    try:
        hk_record = file_lines[i].split(";")[3].split("|")
    except:
        pass
    else:
        hk_list.append(hk_record)

mpptx_solar_voltage = []
mpptx_solar_current = []
mpptx_output_voltage = []
mpptx_temperature = []
#mpptx_state = []

mpptx_solar_power = []

mpptyp_solar_voltage = []
mpptyp_solar_current = []
mpptyp_output_voltage = []
mpptyp_temperature = []
#mpptyp_state = []

mpptyp_solar_power = []

mpptyn_solar_voltage = []
mpptyn_solar_current = []
mpptyn_output_voltage = []
mpptyn_temperature = []
#mpptyn_state = []

mpptyn_solar_power = []

distribution_3v3_curr = []
distribution_3v3_volt = []
distribution_5v_curr = []
distribution_5v_volt = []
distribution_VBAT_curr = []
distribution_VBAT_volt = []
#distribution_lcl_state = []
#distribution_lcl_flagb = []

batc_voltage_a = []
batc_chrg_curr = []
batc_dchrg_curr = []
batc_temperature = []
#batc_state = []

current_watt_hours = 0;
batc_watt_hours = []

bp_temperature_a = []
bp_temperature_b = []

ctrlb_3v3d_volt = []

ctrla_safet_ctr = []
ctrla_pwr_cycles = []
ctrla_uptime = []
ctrla_temperature = []
ctrla_supply_temperature = []

dcdc_3v3_temperature = []
dcdc_5v_temperature = []

for i in range(0, len(hk_list)):
    try:
        float(hk_list[i][12].split(",")[1])
        float(hk_list[i][11].split(",")[1])
        float(hk_list[i][13].split(",")[1])
        float(hk_list[i][14].split(",")[1])
    except:
        pass
    else:
        mpptx_solar_current.append(float(hk_list[i][1].split(",")[1]))
        mpptx_solar_voltage.append(float(hk_list[i][2].split(",")[1]))
        mpptx_output_voltage.append(float(hk_list[i][3].split(",")[1]))
        mpptx_temperature.append(float(hk_list[i][4].split(",")[1]))
        #mpptx_state

        mpptx_solar_power.append(mpptx_solar_current[i] * mpptx_solar_voltage[i])

        mpptyp_solar_current.append(float(hk_list[i][6].split(",")[1]))
        mpptyp_solar_voltage.append(float(hk_list[i][7].split(",")[1]))
        mpptyp_output_voltage.append(float(hk_list[i][8].split(",")[1]))
        mpptyp_temperature.append(float(hk_list[i][9].split(",")[1]))
        #mpptyp_state

        mpptyp_solar_power.append(mpptyp_solar_current[i] * mpptyp_solar_voltage[i])

        mpptyn_solar_current.append(float(hk_list[i][11].split(",")[1]))
        mpptyn_solar_voltage.append(float(hk_list[i][12].split(",")[1]))
        mpptyn_output_voltage.append(float(hk_list[i][13].split(",")[1]))
        mpptyn_temperature.append(float(hk_list[i][14].split(",")[1]))
        #mpptyn_state

        mpptyn_solar_power.append(mpptyn_solar_current[i] * mpptyn_solar_voltage[i])

        distribution_3v3_curr.append(float(hk_list[i][16].split(",")[1]))
        distribution_3v3_volt.append(float(hk_list[i][17].split(",")[1]))
        distribution_5v_curr.append(float(hk_list[i][18].split(",")[1]))
        distribution_5v_volt.append(float(hk_list[i][19].split(",")[1]))
        distribution_VBAT_curr.append(float(hk_list[i][20].split(",")[1]))
        distribution_VBAT_volt.append(float(hk_list[i][21].split(",")[1]))
        #distribution_lcl_state
        #distribution_lcl_flagb

        batc_voltage_a.append(float(hk_list[i][24].split(",")[1]))
        batc_chrg_curr.append(float(hk_list[i][25].split(",")[1]))
        batc_dchrg_curr.append(float(hk_list[i][26].split(",")[1]))
        batc_temperature.append(float(hk_list[i][27].split(",")[1]))
        #batc_state

        current_watt_hours = current_watt_hours + (batc_voltage_a[i] * batc_chrg_curr[i])/(60*60*3)
        current_watt_hours = current_watt_hours - (batc_voltage_a[i] * batc_dchrg_curr[i])/(60*60*3)
        batc_watt_hours.append(current_watt_hours)

        bp_temperature_a.append(float(hk_list[i][29].split(",")[1]))
        bp_temperature_b.append(float(hk_list[i][30].split(",")[1]))

        ctrlb_3v3d_volt.append(float(hk_list[i][31].split(",")[1]))

        ctrla_safet_ctr.append(float(hk_list[i][32].split(",")[1]))
        ctrla_pwr_cycles.append(float(hk_list[i][33].split(",")[1]))
        ctrla_uptime.append(float(hk_list[i][34].split(",")[1]))
        ctrla_temperature.append(float(hk_list[i][35].split(",")[1]))
        ctrla_supply_temperature.append(float(hk_list[i][36].split(",")[1]))

        dcdc_3v3_temperature.append(float(hk_list[i][37].split(",")[1]))
        dcdc_5v_temperature.append(float(hk_list[i][38].split(",")[1]))

x_vector = numpy.linspace(0, len(ctrla_pwr_cycles) / 3.0, num=len(ctrla_pwr_cycles))

fig = plt.figure(1) 
fig.canvas.set_window_title('MPPT X')
plt.subplot(411)
plt.plot(x_vector, mpptx_solar_voltage, 'r', label="Solar Voltage")
plt.ylabel('Voltage [V]')
plt.hold(True)
plt.plot(x_vector, mpptx_output_voltage, 'g', label="Output Voltage")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(412)
plt.plot(x_vector, mpptx_solar_current, 'b', label="Solar Current")
plt.ylabel('Current [A]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(413)
plt.plot(x_vector, mpptx_solar_power, 'y', label="Solar Power")
plt.ylabel('Power [W]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(414)
plt.plot(x_vector, mpptx_temperature, 'm', label="Temperature")
plt.ylabel(u'Temperature [\N{DEGREE SIGN}C]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.xlabel('Time [s]')

fig = plt.figure(2) 
fig.canvas.set_window_title('MPPT Y+')
plt.subplot(411)
plt.plot(x_vector, mpptyp_solar_voltage, 'r', label="Solar Voltage")
plt.ylabel('Voltage [V]')
plt.hold(True)
plt.plot(x_vector, mpptyp_output_voltage, 'g', label="Output Voltage")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(412)
plt.plot(x_vector, mpptyp_solar_current, 'b', label="Solar Current")
plt.ylabel('Current [A]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(413)
plt.plot(x_vector, mpptyp_solar_power, 'y', label="Solar Power")
plt.ylabel('Power [W]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(414)
plt.plot(x_vector, mpptyp_temperature, 'm', label="Temperature")
plt.ylabel(u'Temperature [\N{DEGREE SIGN}C]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.xlabel('Time [s]')

fig = plt.figure(3) 
fig.canvas.set_window_title('MPPT Y-')
plt.subplot(411)
plt.plot(x_vector, mpptyn_solar_voltage, 'r', label="Solar Voltage")
plt.ylabel('Voltage [V]')
plt.hold(True)
plt.plot(x_vector, mpptyn_output_voltage, 'g', label="Output Voltage")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(412)
plt.plot(x_vector, mpptyn_solar_current, 'b', label="Solar Current")
plt.ylabel('Current [A]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(413)
plt.plot(x_vector, mpptyn_solar_power, 'y', label="Solar Power")
plt.ylabel('Power [W]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(414)
plt.plot(x_vector, mpptyn_temperature, 'm', label="Temperature")
plt.ylabel(u'Temperature [\N{DEGREE SIGN}C]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.xlabel('Time [s]')

fig = plt.figure(4) 
fig.canvas.set_window_title('Distribution + CTRLB_3V3d_VOLT + DCDC')
plt.subplot(311)
plt.plot(x_vector, distribution_3v3_volt, 'b', label="3V3 Voltage")
plt.ylabel('Voltage [V]')
plt.hold(True)
plt.plot(x_vector, distribution_5v_volt, 'g', label="5V Voltage")
plt.plot(x_vector, distribution_VBAT_volt, 'r', label="VBAT Voltage")
plt.plot(x_vector, ctrlb_3v3d_volt, '--m', label="CTRLB 3V3d Voltage")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(312)
plt.plot(x_vector, distribution_3v3_curr, 'b', label="3V3 Current")
plt.ylabel('Current [A]')
plt.plot(x_vector, distribution_5v_curr, 'g', label="5V Current")
plt.plot(x_vector, distribution_VBAT_curr, 'r', label="VBAT Current")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(313)
plt.plot(x_vector, dcdc_3v3_temperature, 'y', label="DCDC 3V3 Temperature")
plt.ylabel(u'Temperature [\N{DEGREE SIGN}C]')
plt.plot(x_vector, dcdc_5v_temperature, 'c', label="DCDC 5V Temperature")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.xlabel('Time [s]')

fig = plt.figure(5) 
fig.canvas.set_window_title('Battery Controller + BP')
plt.subplot(411)
plt.plot(x_vector, batc_voltage_a, 'r', label="BATC A Voltage")
plt.ylabel('Voltage [V]')
plt.hold(True)
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(412)
plt.plot(x_vector, batc_chrg_curr, 'b', label="BATC chrg. Current")
plt.ylabel('Current [A]')
plt.plot(x_vector, batc_dchrg_curr, 'm', label="BATC dchrg. Current")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(413)
plt.plot(x_vector, batc_watt_hours, 'y', label="BATC energy diff")
plt.ylabel('Energy diff [Wh]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(414)
plt.plot(x_vector, batc_temperature, 'g', label="BATC Temperature")
plt.ylabel(u'Temperature [\N{DEGREE SIGN}C]')
plt.plot(x_vector, bp_temperature_a, '--r', label="BP Temperature A")
plt.plot(x_vector, bp_temperature_b, '--b', label="BP Temperature B")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.xlabel('Time [s]')

fig = plt.figure(6) 
fig.canvas.set_window_title('CTRLA')
plt.subplot(411)
plt.plot(x_vector, ctrla_safet_ctr, 'r', label="CTRLA Safety Counter")
plt.hold(True)
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(412)
plt.plot(x_vector, ctrla_pwr_cycles, 'g', label="CTRLA Power Cycles")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(413)
plt.plot(x_vector, ctrla_uptime, 'b', label="CTRLA Uptime")
plt.ylabel('Time [s]')
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.subplot(414)
plt.plot(x_vector, ctrla_temperature, 'm', label="CTRLA Temperature")
plt.ylabel(u'Temperature [\N{DEGREE SIGN}C]')
plt.plot(x_vector, ctrla_supply_temperature, 'y', label="CTRLA Supply Temperature")
plt.legend(loc=1, framealpha=0.7, fontsize=8)
plt.xlabel('Time [s]')

plt.show()
