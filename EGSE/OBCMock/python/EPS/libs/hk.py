import math
import resistance_sensors as sensor

class HkValidator(object):
    def check_is_equal(self, to_check, expected):
        if to_check == expected:
            return 0
        return 2
        
    def check_two_levels(self, to_check, fail_low, warning_low, warning_high, fail_high):
        if to_check>fail_high or to_check<fail_low:
            return 2
        if to_check>warning_high or to_check<warning_low:
            return 1
        return 0

class HkExtractor(object):
    def extract_16bit(self, hk_table, index0, index1):
        return hk_table[index0]+hk_table[index1]*256

    def extract_32bit(self, hk_table, index0, index1, index2, index3):
        return hk_table[index0]+hk_table[index1]*256+hk_table[index2]*65536+hk_table[index3]*16777216

    def extract_8bit(self, hk_table, index0):
        return hk_table[index0]
        
class EPS_A_HK(object):
    def __init__(self):
        self.convert = Conversions()
        self.validator = HkValidator()
        self.extractor = HkExtractor()

    def WHO_AM_I(self):
        return b'\x61'

    def hk_convert(self, hk_table):
        who_am_i = self.extractor.extract_8bit(hk_table, 0)
        # MPPTX
        mpptx_sol_curr = self.convert.mppt_current_sense(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x01,0x02))
        mpptx_sol_volt = self.convert.mppt_divider(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x03,0x04))
        mpptx_out_volt = self.convert.mppt_divider(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x05,0x06))
        mpptx_temp = self.convert.mppt_lmt87(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x07,0x08))
        mpptx_state = self.extractor.extract_8bit(hk_table,0x09)
        # MPPTY+
        mpptyp_sol_curr = self.convert.mppt_current_sense(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x0A,0x0B))
        mpptyp_sol_volt = self.convert.mppt_divider(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x0C,0x0D))
        mpptyp_out_volt = self.convert.mppt_divider(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x0E,0x0F))
        mpptyp_temp = self.convert.mppt_lmt87(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x10,0x11))
        mpptyp_state = self.extractor.extract_8bit(hk_table,0x12)
        # MPPTY-
        mpptym_sol_curr = self.convert.mppt_current_sense(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x13,0x14))
        mpptym_sol_volt = self.convert.mppt_divider(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x15,0x16))
        mpptym_out_volt = self.convert.mppt_divider(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x17,0x18))
        mpptym_temp = self.convert.mppt_lmt87(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x19,0x1A))
        mpptym_state = self.extractor.extract_8bit(hk_table,0x1B)
        # Distribution 
        distr_3v3_curr = self.convert.current_sense_distr_hk(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x1C,0x1D))
        distr_3v3_volt = self.convert.divider_distr_hk(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x1E,0x1F))
        distr_5v_curr = self.convert.current_sense_distr_hk(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x20,0x21))
        distr_5v_volt = self.convert.divider_distr_hk(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x22,0x23))
        distr_vbat_curr = self.convert.current_sense_distr_hk(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x24,0x25))
        distr_vbat_volt = self.convert.divider_distr_hk(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x26,0x27))
        distr_lcl_state_raw = self.extractor.extract_8bit(hk_table,0x28)
        distr_lcl_state = self.convert.lcl(self.extractor.extract_8bit(hk_table,0x28))
        distr_lcl_flagb_raw = self.extractor.extract_8bit(hk_table,0x29)
        distr_lcl_flagb = self.convert.lcl(self.extractor.extract_8bit(hk_table,0x29))
        # Battery controller
        batc_volt_a = self.convert.divider_batc(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x2A,0x2B))
        batc_chrg_curr = self.convert.current_sense_distr_hk(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x2C,0x2D))
        batc_dchrg_curr = self.convert.current_sense_distr_hk(self.convert.VREF_3V3(), self.extractor.extract_16bit(hk_table,0x2E,0x2F))
        batc_temp = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x30,0x31))
        batc_state = self.extractor.extract_8bit(hk_table,0x32)
        # Battery pack
        bp_temp_a = self.convert.tmp121(self.extractor.extract_16bit(hk_table,0x33,0x34))
        bp_temp_b = self.convert.tmp121(self.extractor.extract_16bit(hk_table,0x35,0x36))
        # CTRLB status
        ctrlb_3v3d_volt = self.convert.divider_3v3d(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x37,0x38))
        # CTRL A
        ctrla_safety_counter = self.extractor.extract_8bit(hk_table,0x39)
        ctrla_pwr_cycles = self.extractor.extract_16bit(hk_table,0x3A,0x3B)
        ctrla_uptime = self.extractor.extract_32bit(hk_table,0x3C,0x3D,0x3E,0x3F)
        ctrla_temp = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x40,0x41))
        ctrla_supp_temp = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x42,0x43))
        temp_3v3 = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x44,0x45))
        temp_5v = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,0x46,0x47))
        
        hk_converted = [["CTRLA", "WHO_AM_I", hex(who_am_i), self.validator.check_is_equal(who_am_i, 0x61)],
                        ["MPPTX", "SOL_CURR", str(mpptx_sol_curr), self.validator.check_two_levels(mpptx_sol_curr, 0, 0, 0.5, 0.6)],
                        ["MPPTX", "SOL_VOLT", str(mpptx_sol_volt), self.validator.check_two_levels(mpptx_sol_volt, 4, 4.5, 5, 5.5)],
                        ["MPPTX", "OUT_VOLT", str(mpptx_out_volt), self.validator.check_two_levels(mpptx_out_volt, 6.9, 7.2, 7.9, 8.0)],
                        ["MPPTX", "TEMP", str(mpptx_temp), self.validator.check_two_levels(mpptx_temp, -20, 0, 40, 60)],
                        ["MPPTX", "STATE", str(mpptx_state), self.validator.check_is_equal(mpptx_state, 0)],
                        ["MPPTY+", "SOL_CURR", str(mpptyp_sol_curr), self.validator.check_two_levels(mpptyp_sol_curr, 0, 0, 0.5, 0.6)],
                        ["MPPTY+", "SOL_VOLT", str(mpptyp_sol_volt), self.validator.check_two_levels(mpptyp_sol_volt, 5, 6, 13, 14)],
                        ["MPPTY+", "OUT_VOLT", str(mpptyp_out_volt), self.validator.check_two_levels(mpptyp_out_volt, 6.9, 7.2, 7.9, 8.0)],
                        ["MPPTY+", "TEMP", str(mpptyp_temp), self.validator.check_two_levels(mpptyp_temp, -20, 0, 40, 60)],
                        ["MPPTY+", "STATE", str(mpptyp_state), self.validator.check_is_equal(mpptyp_state, 0)],
                        ["MPPTY-", "SOL_CURR", str(mpptym_sol_curr), self.validator.check_two_levels(mpptym_sol_curr, 0, 0, 0.5, 0.6)],
                        ["MPPTY-", "SOL_VOLT", str(mpptym_sol_volt), self.validator.check_two_levels(mpptym_sol_volt, 5, 6, 13, 14)],
                        ["MPPTY-", "OUT_VOLT", str(mpptym_out_volt), self.validator.check_two_levels(mpptym_out_volt, 6.9, 7.2, 7.9, 8.0)],
                        ["MPPTY-", "TEMP", str(mpptym_temp), self.validator.check_two_levels(mpptym_temp, -20, 0, 40, 60)],
                        ["MPPTY-", "STATE", str(mpptym_state), self.validator.check_is_equal(mpptym_state, 0)],
                        ["DISTR", "3V3_CURR", str(distr_3v3_curr), self.validator.check_two_levels(distr_3v3_curr, 0, 0, 0.30, 0.4)],
                        ["DISTR", "3V3_VOLT", str(distr_3v3_volt), self.validator.check_two_levels(distr_3v3_volt, 3.20, 3.25, 3.40, 3.45)],
                        ["DISTR", "5V_CURR", str(distr_5v_curr), self.validator.check_two_levels(distr_5v_curr, 0, 0, 0.8, 0.9)],
                        ["DISTR", "5V_VOLT", str(distr_5v_volt), self.validator.check_two_levels(distr_5v_volt, 4.80, 4.90, 5.1, 5.2)],
                        ["DISTR", "VBAT_CURR", str(distr_vbat_curr), self.validator.check_two_levels(distr_vbat_curr, 0, 0, 1.1, 1.3)],
                        ["DISTR", "VBAT_VOLT", str(distr_vbat_volt), self.validator.check_two_levels(distr_vbat_volt, 6.9, 7.2, 7.9, 8.0)],
                        ["DISTR", "LCL_STATE", str(distr_lcl_state).replace(", ","").replace("[","").replace("]",""), 0],
                        ["DISTR", "LCL_FLAGB", str(distr_lcl_flagb).replace(", ","").replace("[","").replace("]",""), self.validator.check_is_equal(distr_lcl_flagb_raw, 127)],
                        ["BATC", "VOLT_A", str(batc_volt_a), self.validator.check_two_levels(batc_volt_a, 6.9, 7.2, 7.9, 8.0)],
                        ["BATC", "CHRG_CURR", str(batc_chrg_curr), self.validator.check_two_levels(distr_vbat_curr, 0, 0, 1.1, 1.3)],
                        ["BATC", "DCHRG_CURR", str(batc_dchrg_curr), self.validator.check_two_levels(distr_vbat_curr, 0, 0, 1.1, 1.3)],
                        ["BATC", "TEMP", str(batc_temp), self.validator.check_two_levels(batc_temp, -20, 0, 40, 60)],
                        ["BATC", "STATE", str(batc_state), 1],
                        ["BP", "TEMP_A", str(bp_temp_a), self.validator.check_two_levels(bp_temp_a, -5, 0, 40, 50)],
                        ["BP", "TEMP_B", str(bp_temp_b), self.validator.check_two_levels(bp_temp_b, -5, 0, 40, 50)],
                        ["CTRLB", "3V3d_VOLT", str(ctrlb_3v3d_volt), self.validator.check_two_levels(ctrlb_3v3d_volt, 3.15, 3.25, 3.35, 3.45)],
                        ["CTRLA", "SAFETY-CTR", str(ctrla_safety_counter), self.validator.check_is_equal(ctrla_safety_counter, 0)],
                        ["CTRLA", "PWR-CYCLES", str(ctrla_pwr_cycles), self.validator.check_two_levels(ctrla_pwr_cycles, 0, 0, 50, 100)],
                        ["CTRLA", "UPTIME", str(ctrla_uptime), self.validator.check_two_levels(ctrla_uptime, 300, 1800, 604800, 1209600)],
                        ["CTRLA", "TEMP", str(ctrla_temp), self.validator.check_two_levels(ctrla_temp, -20, 0, 40, 60)],
                        ["CTRLA", "SUPP_TEMP", str(ctrla_supp_temp), self.validator.check_two_levels(ctrla_supp_temp, -20, 0, 40, 60)],
                        ["DCDC", "3V3_TEMP", str(temp_3v3), self.validator.check_two_levels(temp_3v3, -20, 0, 40, 60)],
                        ["DCDC", "5V_TEMP", str(temp_5v), self.validator.check_two_levels(temp_5v, -20, 0, 40, 60)]]
        return hk_converted
        
class EPS_B_HK(object):
    def __init__(self):
        self.convert = Conversions()
        self.validator = HkValidator()
        self.extractor = HkExtractor()

    def WHO_AM_I(self):
        return b'\x9D'

    def hk_convert(self, hk_table):
        who_am_i = self.extractor.extract_8bit(hk_table, 0)
        # Battery pack
        bp_temp_c = self.convert.resistance_pt1000(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table, 1, 2))
        # Battery controller
        batc_volt_b = self.convert.divider_batc(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table,3,4))
        # CTRLB status
        ctrla_3v3d_volt = self.convert.divider_3v3d(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table, 5, 6))
        # CTRLB
        ctrlb_safety_counter = self.extractor.extract_8bit(hk_table,7)
        ctrlb_pwr_cycles = self.extractor.extract_16bit(hk_table,8,9)
        ctrlb_uptime = self.extractor.extract_32bit(hk_table, 10, 11, 12, 13)
        ctrlb_temp = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table, 14, 15))
        ctrlb_supp_temp = self.convert.lmt87(self.convert.VREF_3V0(), self.extractor.extract_16bit(hk_table, 16, 17))
        
        hk_converted = [["WHO_AM_I", hex(who_am_i), self.validator.check_is_equal(who_am_i, 0x9D)],
                        ["BP.TEMP_C", str(bp_temp_c), self.validator.check_two_levels(bp_temp_c, -5, 0, 40, 50)],
                        ["BATC.VOLT_B", str(batc_volt_b), self.validator.check_two_levels(batc_volt_b, 6.9, 7.2, 7.9, 8.0)],
                        ["CTRLA.3V3d_VOLT", str(ctrla_3v3d_volt), self.validator.check_two_levels(ctrla_3v3d_volt, 3.15, 3.25, 3.35, 3.45)],
                        ["CTRLB.SAFETY-CTR", str(ctrlb_safety_counter), self.validator.check_is_equal(ctrlb_safety_counter, 0)],
                        ["CTRLB.PWR-CYCLES", str(ctrlb_pwr_cycles), self.validator.check_two_levels(ctrlb_pwr_cycles, 0, 0, 50, 100)],
                        ["CTRLB.UPTIME", str(ctrlb_uptime), self.validator.check_two_levels(ctrlb_uptime, 300, 1800, 604800, 1209600)],
                        ["CTRLB.TEMP", str(ctrlb_temp), self.validator.check_two_levels(ctrlb_temp, -20, 0, 40, 60)],
                        ["CTRLB.SUPP_TEMP", str(ctrlb_supp_temp), self.validator.check_two_levels(ctrlb_supp_temp, -20, 0, 40, 60)]]
        return hk_converted

class Conversions(object):
    def VREF_3V0(self):
        return 3.0

    def VREF_3V3(self):
        return 3.27

    def PRECISION_1(self):
        return 1

    def PRECISION_2(self):
        return 2

    def PRECISION_3(self):
        return 3

    def lmt87(self, v_ref, adc_readout):
        v_adc = (adc_readout/1024.0)*v_ref
        if v_adc>(v_ref):
            v_adc = v_ref-0.01
        v_adc = v_adc*1000.0
        return round((((13.582-math.sqrt((-13.582*(-13.582))+4*0.00433*(2230.8-(v_adc)))))/(2*(-0.00433)))+30.0, self.PRECISION_1())
        
    def tmp121(self, readout):
        sensor_data = readout & 0b1111111111111
        return round((-(sensor_data & (2**12)) + (sensor_data & ~(2**12))) * 0.0625, self.PRECISION_1())

    def mppt_lmt87(self, v_ref, adc_readout):
        v_adc = (adc_readout/4096.0)*v_ref
        if v_adc>(v_ref):
            v_adc = v_ref-0.01  
        v_adc = v_adc*1000.0
        return round((((13.582-math.sqrt((-13.582*(-13.582))+4*0.00433*(2230.8-(v_adc)))))/(2*(-0.00433)))+30.0, self.PRECISION_1())
    
    def mppt_divider(self, v_ref, adc_readout):
        v_adc = (adc_readout/4096.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        return round(v_adc*((4.7+1.0)/(1.0)), self.PRECISION_2())
        
    def mppt_current_sense(self, v_ref, adc_readout):
        v_adc = (adc_readout/4096.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        return round(v_adc*(1.0/(0.068*50.0)), self.PRECISION_3())

    def divider_batc(self, v_ref, adc_readout):
        v_adc = (adc_readout/1024.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        return round(v_adc*((470.0+150.0)/(150.0)), self.PRECISION_2())

    def divider_3v3d(self, v_ref, adc_readout):
        v_adc = (adc_readout/1024.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        return round(v_adc*2.0, self.PRECISION_2())

    def divider_distr_hk(self, v_ref, adc_readout):
        v_adc = (adc_readout/1024.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        return round(v_adc*((4.7+2.2)/2.2), self.PRECISION_2())
    
    def current_sense_distr_hk(self, v_ref, adc_readout):
        v_adc = (adc_readout/1024.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        return round(v_adc*(1.0/(0.025*50.0)), self.PRECISION_3())
    
    def resistance_pt1000(self, v_ref, adc_readout):
        v_adc = (adc_readout/1024.0)*v_ref
        if v_adc>v_ref:
            v_adc = v_ref-0.01
        r_x = v_adc/((v_ref-v_adc)/3320.68)
        return round(sensor.pt1000_res_to_temp(r_x), self.PRECISION_1())

    def lcl(self, readout):
        imtq = ((readout >> 6) & 0b1)
        antenna = ((readout >> 5) & 0b1)
        sens = ((readout >> 4) & 0b1)
        cam_wing = ((readout >> 3) & 0b1)
        cam_nadir = ((readout >> 2) & 0b1)
        suns = ((readout >> 1) & 0b1)
        tk_main = (readout & 0b1)
        return [tk_main, suns, cam_nadir, cam_wing, sens, antenna, imtq]
