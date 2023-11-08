#!/usr/bin/python

USAGE = '''
full_config: copies the last configuration and invalidates it until the config section is full.

Usage:
   python full_config.py [options]

Options:
   -h, --help       Print this help message and exit.
'''
from array import array
from intelhex import IntelHex

####################################################################################################
# History
#   Version 1:
#       First Implementation
#####################################################################################################


def zfillarr(data, size):
    """
    fills a byte array with zeroes
    :param data: array to be filled with zeros
    :param size: number of zeroes to be filled
    :return: none
    """

    data.extend([0] * size)


def raw_lsb_msb_invert(a):
    """
    This function fill ups the binary array with empty dummy bytes
    till the size is a multiple of the OTP row size (row size = 16 Bytes).

    Afterwards the bytes within the row are swapped that the raw starts
    with the MSB Byte and ends with the LSB Byte.

    The binary array is given as function parameter, and the converted
    array is returned.

    :param a: Byte array to be converted
    :return:
    """

    a2 = array('B')

    # generate a copy of the given array 'a'
    # avoids that the given array object is manipulated
    a2.fromstring(a.tostring())

    b = array('B')

    endOfBinary = False
    insertOffset = 0

    while endOfBinary == False:
        for i in range(16):
            try:
                elem = a2.pop(0)
            except:
                #no bytes left in the input array
                endOfBinary = True
                b.insert(insertOffset, 0) #fill up the 16 byte row with empty bytes
            else:
                b.insert(insertOffset, elem)

        insertOffset += 16 #go to the next raw

    return b


def invert_odd_raw_bytes(a):
    """
    Binary inverts the data Bytes of odd OTP raws (row size = 16 Bytes).

    The binary array is given as function parameter, and the converted
    array is returned.

    :param a: Byte array to be converted
    :return:
    """

    a2 = array('B')

    #generate a copy of the given array 'a'
    # avoids that the given array object is manipulated
    a2.fromstring(a.tostring())

    b = array('B')

    i = 0
    endOfBinary = False
    while endOfBinary == False:
        try:
            elem = a2.pop(0)
        except:
            #no bytes left in the input array
            endOfBinary = True
        else:
            if i & 0x10:
                elem = elem ^ 0xFF
            b.append(elem)
        i += 1

    return b

def array_to_uint16(a):
    '''Return an 16-Bit value out of the given byte array'''
    return a[0] | (a[1] << 8)

def array_to_uint32(a):
    '''Return an 32-Bit value out of the given byte array'''
    return a[0] | (a[1] << 8) | (a[2] << 16) | (a[3] << 24)

def otp_bin_file_write(a, filename):
    """
    Writes the configuration data as a binary file

    :param filename: filename or path to the file
    :param a: byte array
    :return:
    """


    # open the output file
    f = open(filename, "wb")

    # write config data as a binary file
    a.tofile(f)

    # close the file
    f.close()

def otp_hex_file_write(a, filename):
    """
    Writes the configuration data in intel hex format

    :param filename: filename or path to the file
    :param a: byte array
    :return:
    """

    ih = IntelHex()
    x = len(a)
    for i in range(0, x):
       ih[i]=a[i]

    ih.tofile(filename, format='hex')

def log_store(s, LogFile):
    """
    This function writes a log of the configuration data to a file for debugging

    :param s: String with the log content
    :param LogFile: Filename for the log
    :return:
    """
    # open ceedling register output file
    f = open(LogFile, "w")
    # write Ceedling register file
    f.write(s)
    # close the file
    f.close()


def configuration_fill_with_last_file(merged_array, new_array):
    '''
    :param merged_array: array storing the merged data 
    :param new_array: new array to be merged in

    Fills a configuration with the last file until it is fullnew configuration into a configuration arraa.
    '''

#    /** Invalid command, will be ignored and the parser jumps to the next command message */
#    OTP_FS_CMD_TERMINATION = 0x0000u,
#    /** TRIM Structure. This table defines the structure of the trim registers  */
#    OTP_FS_CMD_TRIM_STRUCTURE = 0x0001u,
#    /** TRIM Data. Content of the trim registers  */
#    OTP_FS_CMD_TRIM_DATA = 0x0002u,
#    /** Config Structure. This table defines the structure of the config registers  */
#    OTP_FS_CMD_CONFIG_STRUCTURE = 0x0003u,
#    /** Configuration Data. Content of the configuration registers  */
#    OTP_FS_CMD_CONFIG_DATA = 0x0004u,
#    /** Default PMBUS Structural Data. Describes the PMBUS commands  */
#    OTP_FS_CMD_PMBUS_STRUCTURE = 0x0005u,
#    /** Default PMBUS Data. Content of the PMBUS Configuration  */
#    OTP_FS_CMD_PMBUS_DEFAULT_LP0 = 0x0006u,
#    /** Default PMBUS Data. Content of the PMBUS Configuration  */
#    OTP_FS_CMD_PMBUS_USER_LP0 = 0x0007u,
#    /** Default PMBUS Data. Content of the PMBUS Configuration  */
#    OTP_FS_CMD_PMBUS_DEFAULT_LP1 = 0x0008u,
#    /** Default PMBUS Data. Content of the PMBUS Configuration  */
#    OTP_FS_CMD_PMBUS_USER_LP1 = 0x0009u,
#    /** DCDC Configuration Driver ID Pointer Table Command.
#     * It describes the partial configuration pointer table to be used for DCDC configuration over BIF register. */
#    OTP_FS_DCDC_DRIVERID = 0x000Au,
#    /** Indicates a set of #CONFIG_PROTECTOR_MAX_USERS encrypted passwords to unlock the
#     * device's configuration for a specific customer (see @ref pgOpus_Config_Protector). */
#    OTP_FS_CONFIG_PASSWORDS = 0x000Bu,
#    /** Indicates a set of FW configuration parameters which enables/disables and/or configures certain FW features. */
#    OTP_FS_USER_PARAMETERS = 0x000Cu,
#    /** DCDC Configuration Partial Command.
#     *  It provides the partial configuration to be used for
#     *  DCDC configuration over BIF register. */
#    OTP_FS_DCDC_PARTIAL = 0x000Eu,
#    /** Patch Image. Loads this patch after the RTX OS is successfully started.
#     * Then it copies the attached data with the given length to the provided 'ram_address'.
#     * Afterwards it calls the patch entry callback routine (if given == unequal zero). The callback
#     * routine could be linked against the ROM code and could use the full RTX OS functionality. */
#    OTP_FS_CMD_PATCH_IMAGE = 0x000Fu,
#    /** Patch Image with firmware version check. Loads this patch after the RTX OS is successfully started.
#     * Then it copies the attached data with the given length to the provided 'ram_address'.
#     * It checks if the current running firmware version UTC timestamp fits to firmware version where the
#     * patch is generated for.
#     * Afterwards it calls the patch entry callback routine (if given == unequal zero). The callback
#     * routine could be linked against the ROM code and could use the full RTX OS functionality.
#     * The patch contains a new firmware version information. The current firmware UTC timestamp information
#     * is updated based on the patch information. */
#    OTP_FS_PATCH_IMAGE_VERSION = 0x0010u,
#    /** Stops parsing the patches in case this command word is found. */
#    OTP_FS_CMD_INVALID = 0xFFFFu

    # Log string
    s = ''
    otpOffset = 0
    lastotpOffset = 0
    lastlength = 0
    
    a = array('B')
    
    while otpOffset < len(new_array):
        command = array_to_uint16(new_array[otpOffset:])
        length = array_to_uint16(new_array[otpOffset + 4:])
        length2 = array_to_uint16(new_array[otpOffset + 6:])

        if command == 0x0000: # OTP_FS_CMD_TERMINATION
            s += "Termination found \n"
            merged_array.extend(new_array[0:otpOffset])
            merged_array[lastotpOffset]=0xFF
            merged_array[lastotpOffset+1]=0xFF
            
            a = new_array[lastotpOffset:lastotpOffset+lastlength]
            a_inv = new_array[lastotpOffset:lastotpOffset+lastlength]
            a_inv[0] = 0xFF
            a_inv[1] = 0xFF
            length = lastlength
            # Invalidate original file
            while otpOffset < (16384 - 2*length):
                merged_array.extend(a_inv)
                otpOffset += length

            # the last one should be valid
            merged_array.extend(a)

            return s

        s += "Offset 0x%04X (len=%d): \n" % (otpOffset, length)

        if length != length2:
            s +=  "Message 0x%04X with inconsistent length %d != %d \n" % (command, length, length2)

        if (otpOffset + length) > len(new_array):
            s+= "Message 0x%04X length %d exceeds binary file \n" % (command, length)
            return s

        if length < 16:
            s+= "\tCommand length too small for an OTP FS command \n"
            return s

        lastotpOffset = otpOffset
        lastlength = length
        otpOffset += length

    return s

def hex2fullconfig(configFile, fpath, binfpath):
    """
    Reads the configuration file and fills it with the last configuration.
    Leaving the last file valid and invalidate all others. 
    :param configFile: Contain one config file which shall be filled
    :param fpath: Path 
    :param binfpath: path for the log file 

    """    
    import intelhex
    import os
    import sys
    merged_array= array('B')
    
    FullconfigHexFile = os.path.join(fpath, "config_otp_fullconf.hex")
    FullconfigBinFile = os.path.join(fpath, "config_otp_fullconf.bin")
    LogFile = os.path.join(binfpath, "log_xvalent.txt")

    LogString = ''
        
    print 'Processing %s ...'  % configFile
    path_s = os.path.join(fpath, configFile)
    
    try:
        c = IntelHex(path_s)
    except (IOError, intelhex.IntelHexError), e:
        sys.stderr.write("Error reading file: %s\n" % e)
        return 1
    c_array = c.tobinarray()
    LogString += 'Processing %s ... \n'  % configFile
    LogString += configuration_fill_with_last_file(merged_array, c_array) 
        
    otp_hex_file_write(merged_array, FullconfigHexFile)
    otp_bin_file_write(merged_array, FullconfigBinFile)
    log_store(LogString, LogFile)

    #try:
    #    p = intelhex.IntelHex(patchfile)
    #    return 0
    #except (IOError, intelhex.IntelHexError), e:
    #    sys.stderr.write("Error reading file: %s\n" % e)
    #    return 1

    # add configuration data and fill to the end of the partition 

def main():
    import sys
    import getopt
    import os

    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:",["help"])
        for o in opts:
            if o in ('-h', '--help'):
                print(USAGE)
                return 0

        if len(args) > 1:
            raise getopt.GetoptError("Too many arguments")

    except getopt.GetoptError, msg:
        txt = "ERROR: "+str(msg)  # that's required to get not-so-dumb result from 2to3 tool
        print(txt)
        print(USAGE)
        return 2

    # These are the configfiles which will be merged into the xvalent otp configuration file
    configFile = 'config_otp_ACF_P5_3P3V_30A.hex'

    fpath = 'otp_files'
    binfpath = 'build'
    
    try:
        os.mkdir(fpath)
    except:
        if os.path.isdir(fpath):
            pass
    try:
        os.mkdir(binfpath)
    except:
        if os.path.isdir(binfpath):
            pass
        
    try:
        return hex2fullconfig(configFile, fpath, binfpath)
    except IOError as e:
        sys.stderr.write("Error reading file: %s\n" % e)
        return 1

if __name__ == '__main__':
    import sys

    sys.exit(main())
