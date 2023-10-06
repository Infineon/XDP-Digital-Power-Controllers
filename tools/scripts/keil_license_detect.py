#!/usr/bin/python

USAGE = '''
Detects the Infineon site and updates the Keil license file
for the Keil ARM compiler to use the site specific license server.

This script does not support run under LINUX.

Note: This tool uses the WINDOWS 'ipconfig' command to retrieve
      the Infineon site DNS suffix.
      This suffix determines the selected Infineon site.

Usage:
   python keil_license_detect.py [options]

Options:
   -h, --help              Print this help message and exit.

   -o <license_file>       Output Keils 'TOOLS.INI' license file

'''
import sys
import re
import subprocess

domain = 'infineon.com'
na_server_site = ['sjc','tor','tdc']
eu_server_site = ['muc', 'vih']
tools_ini = ['FLEX_USE=1', 'FLEX_VARIANT=mdk_std', ""]

class Exception_Keil_License_Detect(Exception):
    '''Exception class for this script'''
    pass

def determine_license(file_name):
    """Detects the WINDOS network Infineon site and updates the given Keil license server file."""

    # If user using iAccess to log on,
    # the server site could be any of the following
    # depending on his/her location
    # Find current user server
    user_server = subprocess.check_output("ipconfig", shell=True)
    user_server_site =''
    m = re.search(r'DNS Suffix[\ \.\:]+(\w+)\.'+domain, user_server)

    if m:
        user_server_site = m.group(1)
    else:
        raise "ipconfig returns nothing valuable"

    if user_server_site in ['sjc','els','tdc']:
	    license_server = 'FLEX=3500@elslal01.els.infineon.com,3500@elslal02.els.infineon.com,3500@elslal03.els.infineon.com' 
        #license_server = 'FLEX=2045@torlal01.tor.infineon.com,2045@torlal02.tor.infineon.com,2045@torlal03.tor.infineon.com'
    elif user_server_site in ['muc', 'vih']:
        license_server ='FLEX=2045@ULICSERV1.MUC.INFINEON.COM'
    else:
        raise "No license server details for 'ipconfig' returned Infineon site: '%s'." % user_server_site

    # Add detected license server to the generated Keil license file
    tools_ini.insert(0, license_server)

    f = open(file_name, 'w')
    f.write('\n'.join(tools_ini))
    f.close()

###################################################################################################
# Main Routine
###################################################################################################
def main():
    import getopt
    import platform
    
    #Evaluate command line arguments
    try:
        # This script only supports Windows
        if 'Windows' not in platform.system():
            raise Exception_Keil_License_Detect("Only WINDOWS platform support")

        file_name = None
        opts, args = getopt.getopt(sys.argv[1:], "ho:", ["help"])
        for o, a in opts:
            if o in ("-h", "--help"):
                print(USAGE)
                return 0
            elif o in ("-o"):
                file_name=a

        if file_name is None:
            raise Exception_Keil_License_Detect('No Keil license output file given')

        # Retrieve license
        determine_license(file_name)
        return 0

    except Exception_Keil_License_Detect as msg:
        print("'%s' ERROR: %s" % (sys.argv[0], str(msg)))
    except getopt.GetoptError, msg:
        print("'%s' ERROR: %s" % (sys.argv[0], str(msg)))
    else:
        print(USAGE)
        return 0

    print(USAGE)
    return 2

# Make this file executable as a script.
if __name__ == '__main__':
    sys.exit(main())
###################################################################################################
