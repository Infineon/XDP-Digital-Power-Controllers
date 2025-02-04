
import configparser
class ConfigFile(object):
    '''Tool configuration parser'''
    def __init__(self, filename):
        self.filename = filename
        try:
            self.config = configparser.ConfigParser()
            self.config.read(self.filename)
        except:
            raise Exception("Could not load and parse 'ini' configuration file '%s'" % self.filename)

    def string_value_get(self, section, option):
        '''Get a string value of a given config file section and option parameter'''
        try:
            return self.config.get(section, option)
        except:
            raise Exception("Config file '%s' does not contain section '%s' with option '%s'" % (self.filename,section,option))

    def getint(self, section, option):
        '''Get an integer value of a given config file section and option parameter. Hex values are converted to unsigned integer values.'''
        txt = self.string_value_get(section,option)
        try:
            return int(txt)
        except:
            try:
                return int(txt, 16)
            except:
                raise Exception("Config file '%s' section '%s' with option '%s' is not set to a valid integer" % (self.filename,section,option))

    def value_exist(self, section, option):
        '''Returns True if the given section and option parameter exist in the configuration file, otherwise it returns False'''
        try:
            txt = self.config.get(section, option)
            return True
        except:
            return False
     
    def has_section(self, section):
        return self.config.has_section(section)
        
    def set(self, section, option, value):
        return self.config.set(section, option, value)
        
    def write_file(self):
        '''Write file changes back to the file'''
        self.config.write(open(self.filename, 'w'))
    
      