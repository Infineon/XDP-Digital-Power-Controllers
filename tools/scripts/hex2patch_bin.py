#!/usr/bin/python

USAGE = '''
hex2patch_bin: Converts a patch ARM Compiler output HEX file into a
patch binary file that contains a patch header.

The

Usage:
   python hex2patch_bin.py [options]

Options:
   -h, --help       Print this help message and exit.

   -i <hexfile>     Input Intel Hex file (patch compiler output)

   -o <binfile>     Output patch bin file

   -c <cfg_file>    Configuration file.

                         The configuration file should contain parameter like:
                         - UTC timestamp of the firmware version where the patch
                           should be applied (base version).
                         - UTC timestamp of this version (new version).

                         Note: The timestamp information are only supported for partial
                         patches and not for full patches.

                         Example for the configuration file with the corresponding tags:
                             [Version]
                             BaseVersion = 0x55410B00
                             NewVersion = 0x554CC3D1

   --crc-header     Generate the CRC for the patch header

   --crc-data       Generate the CRC for the patch data

   --full-patch     Generates a full image patch, which must be linked to
                    the RAM address offset 0x20000000.
                         Note: This otion cannot be used together with
                         the '-e', '--sym' and '--sym-file' options.

'''
import sys
import os.path

def AddRelativeLibPath(path):
    """Add Python library include path"""
    current_dir = os.path.normpath(__file__+"//..")
    lib_dir = os.path.normpath(current_dir+"//"+path)
    sys.path.insert(0, lib_dir)

#AddRelativeLibPath("../python/intelhex-2.3")
AddRelativeLibPath("lib")

import configparser 

from array import array
from binascii import crc32

hexfile = None
binfile = None
crc_hdr = False
crc_data = False
full_patch = False
config_file = None
hexpatchfile = None
hexconffile = None

def Long2BinArray(array_elem, value):
    '''
    Extend the 'B' array element by 4 Bytes that are little Endian
    generated out of the given unsigned short value.
    The array is set in Little-Endian
    '''
    array_elem.append((value >> 0) & 0xFF)
    array_elem.append((value >> 8) & 0xFF)
    array_elem.append((value >> 16) & 0xFF)
    array_elem.append((value >> 24) & 0xFF)

def Short2BinArray(array_elem, value):
    '''
    Extend the 'B' array element by 2 Bytes that are little Endian
    generated out of the given unsigned short value.
    The array is set in Little-Endian
    '''
    array_elem.append((value >> 0) & 0xFF)
    array_elem.append((value >> 8) & 0xFF)


def zfillarr(data, size):
    """
    fills a byte array with zeroes
    :param data: array to be filled with zeros
    :param size: number of zeroes to be filled
    :return: none
    """

    for s in range(size):
        data.append(0)


def hex_file_write(a, offset, ofile):
    """
    Writes the configuration data in intel hex format

    :param a: byte array
    :param offset: start address of the patch
    :param ofile: file to store hexdata
    :return:
    """
    import intelhex

    ih = intelhex.IntelHex()

    # read the intel-hex configuration file
    try:
        size = ((offset - 0x10020000) & 0xFF00)
        #print "size %x" % size
        if hexconffile is not None:
            config_hex = intelhex.IntelHex(hexconffile)
            b = config_hex.tobinarray()
            for i in range (0,len(b)):
                ih[i] = b[i]
            for i in range(size, size + len(a)):
                ih[i] = a[i - size]
            ih.tofile(ofile, format='hex')
        else:
            x = len(a)
            for i in range(0, x):
                ih[i + size] = a[i]
            ih.tofile(ofile, format='hex')

    except (IOError, intelhex.IntelHexError) as e:
        sys.stderr.write("Error reading file: %s\n" % e)
        return 1



def hex2bin():
    '''
    Read the Intel Hex file and generates the binary patch file.
    '''
    import intelhex

    global full_patch

    #read the intel-hex file
    try:
        ih = intelhex.IntelHex(hexfile)
    except (IOError, intelhex.IntelHexError) as e:
        sys.stderr.write("Error reading file: %s\n" % e)
        return 1


    # Check on the configuration file if we need to generate a
    # version partial patch image
    partial_patch_version = False
    if config_file is not None:
        if config_file.has_section("Version"):
            partial_patch_version = True

    #get the start address of the intel-hex file
    (start,end) = ih._get_start_end()
    #convert the read intel-hex file into a binary array
    patch_data_bin = ih.tobinarray()

    # create the Patch header with the following format:
    #
    #      /** Patch command */
    #      uint16_t cmd;
    #      /** Option field. Supported bit values:
    #      * - @ref PATCH_CMD_OPT_CRC
    #      */
    #      uint16_t option;
    #      /** Overall patch size (includes the command and size parameter). */
    #      uint16_t size;
    #      /** Overall patch size (includes the command and size parameter). */
    #      uint16_t size_red;
    #      /** CRC-32 value of the command header blocks (e.g. Patch_Cmd_hdr_t + Patch_Cmd_Image_t).
    #      * This checksum field is ignored in case it is set to zero and the @ref PATCH_CMD_OPT_CRC bit is not set
    #      * in the option field.
    #      * The checksum calculation includes all command header and data Bytes, but it excludes the CRC field. */
    #      uint32_t crc_hdr;
    #      /** CRC-32 value of the overall command data (excluding command header blocks).
    #      * This checksum is ignored in case the @ref PATCH_CMD_OPT_CRC bit is not set in the option field
    #      * and the header checksum is zero.
    #      * The checksum calculation includes all command header and data Bytes, but it excludes the CRC field. */
    #      uint32_t crc_data;
    #
    #Additional fields for RTX based patching:
    #      /** Target RAM address where to store the overall patch (including the patch command header). */
    #      uint32_t location;
    #      /** Target RAM entry address where to execute the loaded patch image (callback function). */
    #      uint32_t execute;

    # /** OTP file system command option field. Used by the option
    # * field in the Otp_File_System_Cmd_Hdr_t structure.
    # * If this option fiesl is set to '1' the patch is considered to be compiled with an
    # * absolute address.
    # * Otherwise it assumes that the patch was compiled for the beginning of
    # * the corresponding patch partition.
    # */
    # #define OTP_FS_PATCH_ABSOLUTE              (0x1u)

    option = 0

    #size in Bytes of 'Patch_Cmd_hdr_t' + 'Patch_Cmd_Image_t/Patch_Cmd_Image_Version_t'
    header_size = 20
    if partial_patch_version is True:
        header_size += 8

    patch_hdr_bin = array("B")

    #crc-data is at the end of the patch
    crc_data_size = 4

    patch_size = len(patch_data_bin) + header_size + crc_data_size
    # fill zeroes to align with 32bit words
    size = 1024 - (patch_size % 1024)
    if size < 1024:
        zfillarr(patch_data_bin, size)

    # We have now a 1kbyte aligned size
    patch_size = len(patch_data_bin) + header_size + crc_data_size
    #generate patch header information
    # uint16_t cmd;
    if partial_patch_version is False:
        Short2BinArray(patch_hdr_bin, 0x000F) # Partial Patch (w/o version UTC header information)
    else:
        Short2BinArray(patch_hdr_bin, 0x0010) # Partial Patch (with version UTC header information)
    # uint16_t option;
    Short2BinArray(patch_hdr_bin, option)
    # uint16_t size;
    Short2BinArray(patch_hdr_bin, patch_size)
    # uint16_t size_red;
    Short2BinArray(patch_hdr_bin, patch_size)

    # calculate the header CRC-32 value (setting the CRC fields to zero)
    hdr_crc = crc32(patch_hdr_bin)

    # generate an array for any possible sub-command header
    sub_header = array("B")

    # generate an array for the complete binary data
    patch_bin_array = array("B")

    # uint32_t location;
    Long2BinArray(sub_header, start - header_size)
    # uint32_t execute;
    ih_dict = ih.todict()
    address = None
    if "start_addr" in ih_dict.keys():
        if "EIP" in ih_dict["start_addr"]:
            address = ih_dict["start_addr"]["EIP"]
            Long2BinArray(sub_header, address)
    if address is None:
        print ("ERROR: Given HEX File does not contain \"EIP\" entry address!")
        return 2

    # For partial patches with version information, the base of new UTC version time
    # time need to be added.
    if partial_patch_version is True:
        Long2BinArray(sub_header, int(config_file.get("Version", "BaseVersion"), 16))
        Long2BinArray(sub_header, int(config_file.get("Version", "NewVersion"), 16))
        #Long2BinArray(sub_header, 0x56EDA726)
        #Long2BinArray(sub_header, 0x57FBE2D6)

    # Update the header CRC value
    hdr_crc = crc32(sub_header, hdr_crc)

    #calculate the data checksum
    data_crc = crc32(patch_data_bin)

    # uint32_t crc_hdr;
    Long2BinArray(patch_hdr_bin, hdr_crc)

    # uint32_t crc_hdr;
    Long2BinArray(patch_data_bin, data_crc)

    patch_bin_array.extend(patch_hdr_bin)
    #write the patch sub header data to the file
    if len(sub_header):
        patch_bin_array.extend(sub_header)

    patch_bin_array.extend(patch_data_bin)

    if binfile is not None:
        # open the output file
        f = open(binfile, "wb")
        # write as a bin file
        patch_bin_array.tofile(f)
        # close the file
        f.close()

    if hexpatchfile is not None :
        hex_file_write(patch_bin_array, start, hexpatchfile)

    return 0


def main():
    import getopt

    global hexfile
    global binfile
    global crc_hdr
    global crc_data
    global full_patch
    global hexpatchfile
    global hexconffile

    try:
        opts, args = getopt.getopt(sys.argv[1:], "c:hi:j:o:p:",
            ["crc-data","crc-header","help", "full-patch"])
        for o, a in opts:
            if o in ('-h', '--help'):
                print(USAGE)
                return 0
            elif o == ('--full-patch'):
                full_patch = True
            elif o in ('-i'):
                hexfile=a
            elif o in ('-j'):
                hexconffile = a
            elif o in ('-o'):
                binfile=a
            elif o in ('-p'):
                hexpatchfile = a
            elif o in ('-c'):
                global config_file
                config_file = configparser.RawConfigParser()
                config_file.read(a)
            elif o in ('--crc-header'):
                crc_hdr=True
            elif o in ('--crc-data'):
                crc_data=True

        if hexfile is None:
            raise getopt.GetoptError("Hex input file not specified")
        if hexpatchfile is None and binfile is None:
            raise getopt.GetoptError("No output file specified")

        #if hexconffile is None:
        #   raise getopt.GetoptError("Hex configuration input file not specified")

        if len(args) > 1:
            raise getopt.GetoptError("Too many arguments")

    except getopt.GetoptError as msg:
        txt = "ERROR: "+str(msg)  # that's required to get not-so-dumb result from 2to3 tool
        print(txt)
        print(USAGE)
        return 2

    try:
        return hex2bin()
    except IOError as e:
        import errno
        if e.errno not in (0, errno.EPIPE):
            raise

if __name__ == '__main__':
    import sys
    sys.exit(main())
