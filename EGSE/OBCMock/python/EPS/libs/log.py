import time
from time import localtime, strftime

class Log(object):
    def __init__(self):
        self.file_name = self.generate_file_name()
        self.append_record(self.HEADER())

    def append_record(self, record):
        with open(self.file_name, 'a') as f:
            f.write(record)
        f.close()
        return record
        
    def get_timestamp(self):
        timestamp = []
        timestamp.append(strftime("%Y-%m-%d", localtime()))
        timestamp.append(strftime("%H:%M:%S", localtime()))
        return timestamp

    def generate_file_name(self):
        timestamp = []
        timestamp = self.get_timestamp()

        file_name = "EpsLog_"
        file_name += timestamp[0]
        file_name += "_"
        file_name += timestamp[1].replace(":", "-")
        file_name += self.EXPANSION()

        return file_name

    def EXPANSION(self):
        return ".txt"

    def HEADER(self):
        return ""

    def prefix_INFO(self):
        return "[INFO]"

    def prefix_DATA(self):
        return "[DATA]"

    def prefix_ERROR(self):
        return "[ERROR]"
    
    def NEW_LINE(self):
        return "\n"

    def SEPARATOR(self):
        return ";"

    def join(self, list):
        return ";".join(list)

    def info(self, info_list):
        info_record = self.prefix_INFO() + self.SEPARATOR()
        info_record += self.join(self.get_timestamp())
        info_record += self.SEPARATOR()
        info_record += self.join(info_list)

        self.append_record(info_record + self.NEW_LINE())

        return info_record

    def data(self, data_list):
        data_record = self.prefix_DATA() + self.SEPARATOR()
        data_record += self.join(self.get_timestamp())
        data_record += self.SEPARATOR()
        data_record += self.join(data_list)

        self.append_record(data_record + self.NEW_LINE())

        return data_record

    def error(self, error_message):
        error_record = self.prefix_ERROR() + self.SEPARATOR()
        error_record += self.join(self.get_timestamp())
        error_record += self.SEPARATOR()
        error_record += "\""+error_message+"\""
        
        self.append_record(error_record + self.NEW_LINE())
        
        return error_record


    def hk(self, hk_converted):
        kh_record = self.prefix_DATA() + self.SEPARATOR()
        kh_record += self.join(self.get_timestamp())
        kh_record += self.SEPARATOR()

        hk_as_string = ""
        for i in range(0, len(hk_converted)):
            single_value = hk_converted[i][0] + "_" + hk_converted[i][1]
            single_value += ","
            single_value += str(hk_converted[i][2])

            if not i==(len(hk_converted)-1):
                single_value += "|"

            kh_record += single_value

        self.append_record(kh_record + self.NEW_LINE())

        return hk_as_string
