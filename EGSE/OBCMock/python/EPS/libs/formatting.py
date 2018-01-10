class Formatting(object):
    def HEADER(self):
        return '\033[95m'
    def OKBLUE(self):
        return '\033[94m'
    def OKGREEN(self):
        return '\033[92m'
    def WARNING(self):
        return '\033[93m'
    def FAIL(self):
        return '\033[91m'
    def ENDC(self):
        return '\033[0m'
    def BOLD(self):
        return '\033[1m'
    def UNDERLINE(self):
        return '\033[4m'
    
    def bold(self, string):
        return self.BOLD() + string + self.ENDC()
        
    def warning(self, string):
        return self.WARNING() + string + self.ENDC()
        
    def fail(self, string):
        return self.FAIL() + string + self.ENDC()
        
    def okgreen(self, string):
        return self.OKGREEN() + string + self.ENDC()
        
    def okblue(self, string):
        return self.OKBLUE() + string + self.ENDC()
        
    def color_it(self, string, code):
        if code==0:
            return self.okgreen(string)
        if code==1:
            return self.warning(string)
        return self.fail(string)
        
    def add_separator(self, string):
        separators = ""
        for i in range(0, 20-len(string)):
            separators = separators + " "
        return string + separators