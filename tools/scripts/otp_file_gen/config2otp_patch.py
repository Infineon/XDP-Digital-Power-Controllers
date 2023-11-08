from math import ceil
import pandas as pd
from binascii import crc32
from array import array
from intelhex import IntelHex
import datetime
import os

####################################################################################################
# History
#   Version 4:
#       Implemented PMBUS separated for lp0 and lp1
#   Version 5:
#       Setting for lp0 is used for lp1, only the reset values are taken from lp1
#   Version 6:  10/26/17
#       trim added to default configuration. It now contains trim and pmbus default SHASTA-312
#   Version 7:  12/18/18
#       generation of a full configuration added for SHASTA-637
#####################################################################################################

USAGE = '''
config2otp: Converts register map and pmbus excel sheets to hex bin and text OTP files

Usage:
   python config2otp.py

Options: none

Arguments: none
'''


# This class packs the data in a list of byte.
class pack_reg:
    """
    This class is used for packing the register structure.
    Currently only nopack is used which assumes that the data are maximum 1 byte
    """
    data = []
    start = 1
    dataindex = 0
    byteindex = 0

    def __init__(self):
        self.dataindex = 0
        self.bitindex = 0
        self.data = []

    def pack(self, x, lx):

        number_of_bits_new = self.bitindex + lx
        # if number_of_bits_new == 90:
        #    print('Hi')
        # Previous number of bytes.
        # if current number of bytes is greater then previous number we have to start dancing
        number_of_bits_old = self.bitindex
        # First empty position in current byte
        current_empty_bit_pos = number_of_bits_old % 8
        # Number of empty bits in current byte
        number_of_empty_bits = 8 - current_empty_bit_pos

        self.bitindex = number_of_bits_new

        # This is the first time we come or we start with a fresh new byte
        # In this case current_empty_bit_position is '0'
        # We just append the whole data (lx bits)
        if current_empty_bit_pos == 0:
            lrem = lx
            adda = x
            while lrem > 0:
                self.data.append(adda & 0xFF)
                adda >>= 8
                lrem -= 8
            y = self.data[-1]
        elif lx <= number_of_empty_bits:
            # The whole data fit into the old byte
            msk = (1 << (lx + 1)) - 1
            adda = (x & msk) << current_empty_bit_pos
            y = self.data[-1] + adda
            self.data[-1] = y
        else:
            # Distinguish, first clculate how many bits fit into the current byte
            # and then append remaining data in new bytes
            # lx is bigger then the number of remaining bits
            msk = (1 << number_of_empty_bits) - 1
            adda = (x & msk) << current_empty_bit_pos
            y = self.data[-1] + adda
            self.data[-1] = y
            # remaining bits which do not fit
            adda = x >> number_of_empty_bits
            lremaining = lx - number_of_empty_bits
            while lremaining > 0:
                self.data.append(adda & 0xFF)
                adda >>= 8
                lremaining -= 8

        if y > 255:
            print "Warning !! Byte value greater then 255: %d" % y

    def nopack(self, x):
        if isinstance(x, int):
            self.data.append(x)
        else:
            for i in x:
                self.data.append(i)
                
    # Alignt to 32 bit for fw parameters
    def align(self):
        rem = 32 - self.bitindex % 32
        if rem < 32:
            self.bitindex += rem 
            zfillarr(self.data, (rem >> 3))


def to_int(x):
    """
    Converts the instance address of 32 bits into integer using
     It takes the value from bits 8 to 15 according to this mask: xxxx_XXxx
    :param x: hexadecimal string without preceeding "0x"
    :return: None
    """
    return (int("0x" + str(x), 16) & 0xFFFF) >> 8


def twos_compl(x, bits):
    return (1 << (bits + 1)) - x


def Long2BinArray(array_elem, value):
    """
    Extend the 'B' array element by 4 Bytes that are little Endian
    generated out of the given unsigned short value.
    The array is set in Little-Endian
    """
    array_elem.append((value >> 0) & 0xFF)
    array_elem.append((value >> 8) & 0xFF)
    array_elem.append((value >> 16) & 0xFF)
    array_elem.append((value >> 24) & 0xFF)


def Var2BinArray(array_elem, value, size):
    """
    Extend the 'B' array element by 4 Bytes that are little Endian
    generated out of the given unsigned short value.
    The array is set in Little-Endian

    :param array_elem: byte array
    :param value: value to be converted
    :param size: in bits
    :return:
    """

    for s in range(0, size, 8):
        array_elem.append((value >> s) & 0xFF)


def Short2BinArray(array_elem, value):
    """
    Extend the 'B' array element by 2 Bytes that are little Endian
    generated out of the given unsigned short value.
    The array is set in Little-Endian
    """
    array_elem.append((value >> 0) & 0xFF)
    array_elem.append((value >> 8) & 0xFF)


def CalculateRegisterData(reg_adr, reg_res, reg_start, reg_width):
    """
    :param reg_adr: Full register address
    :param reg_res: Reset values (per subfield and instance)
    :param reg_start: Bit start position of subfield
    :param reg_width: Bit width of subfields
    :return: Reg32data:

    Calculate the value of the register from the subfields
    """

    Reg32data = []
    # Iterate over the instances
    for inst in range(len(reg_res[0])):

        reg_val = 0
        # Iterate over the subfields
        for rd in range(len(reg_start)):
            # RegsStructList += reg_width[rd]
            # PackRegsTbl.nopack(reg_width[rd])
            # Collect the subfield values of the register and collect them in one variable
            # for all instances
            width = reg_width[rd][0]
            start = reg_start[rd][0]
            reset = int(reg_res[rd][inst])
            if reset < 0:
                x = twos_compl(reset, width)
            else:
                x = reset

            # Here we check if the reset value fits into the range given by the width
            if x >= (1 << width):
                print "Warning value exceeds bit width, value set to zero: \n " \
                      "Adr: 0x%X, Start: %d, Width %d, Value is: %d, Value max: %d" % \
                      (reg_adr[0], start, width, reset, (1 << width) - 1)
                x = 0
            reg_val += (x << start)
        num_of_bits_per_register = start + width
        Reg32data += [[reg_adr[inst], reg_val, num_of_bits_per_register]]

    return Reg32data


def get_header(reglist, app, numi):
    """
    :param reglist: Register List
    :param app: Application to retrieve
    :param numi: Number of instances
    :return: None

    Retrieve the command data for registers or PMBus commands
    
    """
    header_cols_list = list(reglist)

    # Search for an apps column
    header_apps = [s for s in header_cols_list if ((("Sim " + app) == s) or (("Sim " + app + ".") in s))]

    if len(header_apps) != numi:
        # Use reset in case of an error
        print "Error in %s: Too many apps columns is:%d should be: %d" % (app, len(header_apps), numi)
        header_apps_used = ['Reset']
    else:
        header_apps_used = header_apps

    return header_apps_used

def  FWConfigGet(fw_sheet, Dataxls, FWDataList):
    """
    :param fw_sheet: String containing the name of the excel sheet 
    :param Dataxls: Database containing the corresponding xls file
    :param FWDataList: is a list containing fw parameter
    :return: None

     Read the FW Excel file and retrieve the fw data from it, could be either trim or config.
     The FWData list is a two dimensional list each line containing
     <data> ......... data value
     <bit_length> ... should be multiple of 8 bits
    """
    # now get the data from xls
    fwdata = Dataxls.parse(fw_sheet)
    fwbitwidth = fwdata['Bit_Width (multiple of 8)'].values.tolist()
    fw_values = fwdata['Value'].values.tolist()
    for i in range(len(fwbitwidth)):
        FWDataList += [[fw_values[i], fwbitwidth[i]]]

    

def GetRegData(app, Dataxls, RegisterType, RegsStructList, RegsStructArray, RegsDataList, RegsDataArray):
    """
    :param app: String containing the name of the application to process
    :param Dataxls: Database containing the corresponding xls file
    :param RegisterType: is "Trim" or "Config"
    :param RegsStructList: is s list containing the register structure with debug information for easier reading
    :param RegsStructArray: is an array containing the structure which can be store into a byte file
    :param RegsDataList: is s list containing register data with debug information for easier reading
    :param RegsDataArray: the Reset Value of the for the register it is used currently as a default value
    :return: None

     Read the Register Excel file and retrieve the register structure and data form it.
     Version 2 with reduced inormation
     The register structure is decribed as follows
     item
     i0 The number of instances of this register sheet

        repeated for the number of instances (numi)
        1   The base adresses related to the instances
        2   The number of registers in this sheet (numr)

            repeated for the number of registers
            r0  The register offset address. 8bits, range 0 to 255
            r1  Number of Bits (Mask. 5bits, range 0 to 31

    """

    # List of all sheets in the file. The sheetname defines the name of register block
    # A register block is a list of registers for a specific block the block can be instanciated
    # several times in a design
    # The register pages are defined in system and are instances of register blocks
    
    # Take the first excel dataset because it contains the registers
    # The second contains FW configuration 
    sheets = Dataxls[0].sheet_names
    sheets.pop(0)

    PackRegsTbl = pack_reg()
    PackDataTbl = pack_reg()

    # retreive the register pages
    top = Dataxls[0].parse('system')
    topsel = top[['BIF Module Name', 'Baseline Address']]
    # print (topsel)
    # sheets = ["analog"]
    num_bits_total = 0
    num_registers_total = 0
    structural_bits_total = 0
    Reg32ListFull = []

    # Process registers sheet by sheet
    for sheet in sheets:
        num_bits_per_sheet = 0
        instances = topsel[topsel["BIF Module Name"] == sheet]
        base_add = instances["Baseline Address"]
        # get the number of base addresses and store in the table
        numi = instances.shape[0]
        x = base_add.apply(to_int).values.tolist()
        base_adr_list = x

        # now store the registers
        reglist = Dataxls[0].parse(sheet)
        regselcols = ["Register Name", 'Hex Address', 'Start Position', 'Width', 'Trim', 'OTP Store']
        header_apps_used = get_header(reglist, app, numi)

        regselcols += header_apps_used

        regsel = reglist[regselcols]

        if RegisterType == "TRIM":
            regselonly = regsel[(regsel["Trim"] == True) & (regsel["OTP Store"] == True)]
        else:
            regselonly = regsel[(regsel["Trim"] == False) & (regsel["OTP Store"] == True)]

        regselfilt = regselonly[["Register Name"]].drop_duplicates("Register Name")

        # Retreive number of registers in the sheet
        numr = regselfilt.shape[0]
        # print ("numr", numr)
        # print ("regselfilt", regselfilt)

        # Loop through all registers if there are any
        if numr > 0:
            RegsStructList.append(sheet)
            PackRegsTbl.pack(numi, 3)
            RegsStructList.append(numi)
            # Number of instances 2 bits
            structural_bits_total += 2

            # proces the instances
            # store the base addresses itself (it's a list) per instance
            for xi in x:
                PackRegsTbl.pack(xi, 8)

            RegsStructList += x
            # Base address 8 bits
            structural_bits_total += 8 * numi

            # Add number of registers
            PackRegsTbl.pack(numr, 8)
            RegsStructList.append(numr)
            # Number of registers 0 - 255 stored in 8 bits
            structural_bits_total += 8
            num_registers_total += numr * numi

            Reg32List = [[], [], []]
            RegsDataListperInst = [[], [], []]
            # Loop over the registers
            for reg in regselfilt.values:
                regdata = regsel[regsel["Register Name"] == reg[0]]
                RegsStructList.append(reg[0])

                # retreive number of subfields in register
                numrd = regdata.shape[0]
                # PackRegsTbl.nopack(numrd)
                RegsStructList.append(numrd)

                # retreive register address and convert to integer
                reg_adr = int(str(regdata["Hex Address"].values[0]), 16)
                PackRegsTbl.pack(reg_adr, 8)
                RegsStructList.append(reg_adr)
                # Register offset address 8 bits
                structural_bits_total += 8

                # retreive subfield start position and width
                reg_start = regdata[["Start Position"]].values.tolist()
                reg_width = regdata[["Width"]].values.tolist()
                reg_res = regdata[header_apps_used].values.tolist()

                # Loop over the register subfields
                reg_adr_total = [0x70000000 + 0x100 * i + reg_adr for i in base_adr_list]
                Reg32data = CalculateRegisterData(reg_adr_total, reg_res, reg_start, reg_width)
                for i in range(numi):
                    Reg32List[i] += [Reg32data[i]]
                    RegsDataListperInst[i] += [[reg[0], Reg32data[i]]]

                num_of_bits_per_register = Reg32data[0][2]
                RegsStructList.append(num_of_bits_per_register)

                # We report 1 bit less to fit into a 5 bit value
                # The range of instances is 1 .. 32.
                PackRegsTbl.pack(num_of_bits_per_register - 1, 5)
                num_bits_per_sheet += num_of_bits_per_register
                # Number of bits per register (Mask) 5 bits
                structural_bits_total += 5

                # List to visualize the registers: address, value, number of bits
                # RegsDataList.append(Reg32data)

            # Duplicate according to the number of instances
            # We first gather all register data in one list for easier processing
            for i in range(numi):
                Reg32ListFull += Reg32List[i]
                RegsDataList += RegsDataListperInst[i]

            RegsStructList += [sheet + '_bits', num_bits_per_sheet]
            num_bits_total += num_bits_per_sheet

    # Pack the data bit by bit into a byte list
    for s in Reg32ListFull:
        PackDataTbl.pack(s[1], s[2])

    # Terminate the Structure list with a '0' indicating that there
    # are no more instances to follow
    PackRegsTbl.pack(0, 3)

    RegsStructList += ['total_data_bits', num_bits_total, int(ceil(num_bits_total / 8))]
    RegsStructList += ['total_structural_bits', structural_bits_total, int(ceil(structural_bits_total / 8))]
    RegsStructList += ['total_number_registers', num_registers_total]
    RegsStructList += ['total_addr_mask', num_registers_total * 32 * 2, int(ceil(num_registers_total * 4 * 2))]

    # Now append the fw data
    # Dataset1 contains fw data
    fw_sheets = Dataxls[1].sheet_names
    FWDataList=[]

    if RegisterType == "TRIM":
        # Get the FW trim data
        FWConfigGet(fw_sheets[0], Dataxls[1], FWDataList)
    else:
        # Get the FW configuration data
        FWConfigGet(fw_sheets[1], Dataxls[1], FWDataList)

    PackDataTbl.align()

        # Pack the FW data byte by byte into a byte list
    for s in FWDataList:
        PackDataTbl.pack(s[0], s[1])

    
    # Convert the lists into an array
    RegsStructArray.fromlist(PackRegsTbl.data)
    RegsDataArray.fromlist(PackDataTbl.data)
    
    # 


def GetPMBusData(app, Dataxls, DataType, RegsStructList, RegsStructArray, RegsDataList, PMBUDataArrayList):
    """
    :param app: String containing the name of the application to process
    :param Dataxls: Database containing the corresponding xls file
    :param DataType: contains "PMBUS0" for Loop0, "PMBUS1" for Loop1, because we are dealing with PMBus data only and MFR_en
    :param RegsStructList: is s list containing the command structure with debug information for easier reading
    :param RegsStructArray: is an array containing the structure which can be stored into a byte file
    :param RegsDataList: is s list containing command data with debug information for easier reading
    :param PMBUDataArrayList: List of the Reset Values for the commands. It is used currently as a default value for all loops,
            user and data. It is a list of four Byte arrays.
    :return: None

     Read the PMBus Excel file and retrieve the structure and data form it.
     The PMBus structure is decribed as follows
     item
     i0 The PMBus number of instances numi (1)
     i1 The PMBus instance number (1 or 2)

        repeated for the number of commands
        r0  The command 8 Bits, range 0 to 255
        r1  is command enabled 1 Bit, 0, 1
        r2  number of bits 9 Bits, 1 - 256

    """

    # List of all sheets in the file. The sheetname defines the name of register block
    # A register block is a list of registers for a specific block the block can be instanciated
    # several times in a design
    # The register pages are defined in system and are instances of register blocks
    sheets = Dataxls.sheet_names

    # Storage for the structural information
    PackRegsTbl = pack_reg()
    # Storage for the actual data
    PackUserDataTbl_lp0 = pack_reg()
    PackUserDataTbl_lp1 = pack_reg()
    PackDefaultDataTbl_lp0 = pack_reg()
    PackDefaultDataTbl_lp1 = pack_reg()
    
    RegsStructure_tmp = []
    RegsData_tmp = []
    RegsStructList_tmp = []
    RegsDataList_tmp = []
                
    sheet = 'Shasta'
    num_bits_per_sheet = 0
    # We have 1 instances of pmbus for now
    numi = 2
    reglist = Dataxls.parse(sheet)

    # We only check f
    header_apps_used = get_header(reglist, app, numi)
    RegisterColumns = {'opcode': 'Opcode', 'command': 'Command', 'mfr': 'MFR', 'supported': 'Loop 0\r\nSupport',
                       'nbits': 'Loop 0 OTP\r\nnbits',
                       'reset_lp0': 'Loop 0 reset', 'reset_lp1': 'Loop 0 reset',
                       'apps_lp0': header_apps_used[0], 'apps_lp1': header_apps_used[1],
                       'cmd_num_bytes': '#B',
                       }

    # Iterate over all values in the dictionary
    regsel = reglist[[v for v in RegisterColumns.values()]]

    if DataType['mfr_en'] == 0:
        # Filter only the commands which are supported, have nbits more than 0, have mfr not set
        regselonly = regsel[(regsel[RegisterColumns['supported']] == 'y') &
                            (regsel[RegisterColumns['mfr']] != 'y') &
                            (regsel[RegisterColumns['nbits']] != 0)]
    else:
        # Filter only the commands which are supported and have nbits more than 0 including mfr
        regselonly = regsel[(regsel[RegisterColumns['supported']] == 'y') &
                            (regsel[RegisterColumns['nbits']] != 0)]

    # Only return the command field
    regselfilt1 = regselonly[["Command"]]

    # remove 'Reserved' from the command field
    regselfilt = regselfilt1[(regselfilt1["Command"] != 'Reserved')]

    # Retreive number of commands in the sheet
    numr = regselfilt.shape[0]
    #print ("numr", numr)
    #print ("regselfilt", regselfilt)

    structural_bits_total = 0
    num_registers_total = 0
    num_bits_total = 0
    reg_cmd_num_bytes_total = 0
    x = 1
    # Loop through all commands if there are any
    if numr > 0:
        RegsStructList.append(sheet)
        PackRegsTbl.pack(numi, 3)
        RegsStructList.append(numi)
        # Number of instances 2 bits
        structural_bits_total += 3

        # proces the instances
        # store the base addresses itself (it's a list) per instance
        PackRegsTbl.pack(x, 3)

        RegsStructList.append(x)
        # Base address 8 bits
        structural_bits_total += 3 * numi

        # Add number of commands
        PackRegsTbl.pack(numr, 8)
        RegsStructList.append(numr)
        # Number of commands 0 - 255 stored in 8 bits
        structural_bits_total += 8
        num_registers_total += numr * numi

        RegsStructure = []
        RegsData = []

        # Loop over the commands
        for reg in regselfilt.values:
            regdata = regsel[regsel["Command"] == reg[0]]

            # retreive the hex command code and convert to integer
            reg_adr = int(str(regdata["Opcode"].values[0]), 16)
            PackRegsTbl.pack(reg_adr, 8)
            # Commands address 8 bits
            structural_bits_total += 8

            # retreive subfield start position and width
            excel_error = 0
            try:
                reg_res_lp0 = int(str(regdata[[RegisterColumns['reset_lp0']]].values[0][0]), 16)
            except ValueError:
                print "Warning!!! %s loop0, No valid reset data found, set to zero CMD: 0x%x" % (DataType['type'], reg_adr)
                reg_res_lp0 = 0

            try:
                reg_res_lp1 = int(str(regdata[[RegisterColumns['reset_lp1']]].values[0][0]), 16)
            except ValueError:
                print "Warning!!! %s loop1, No valid reset data found, set to zero CMD: 0x%x" % (DataType['type'], reg_adr)
                reg_res_lp1 = 0

            try:
                reg_apps_lp0 = int(str(regdata[[RegisterColumns['apps_lp0']]].values[0][0]), 16)
            except ValueError:
                print "Warning!!! %s loop0, No valid reset data found, set to zero CMD: 0x%x" % (DataType['type'], reg_adr)
                reg_apps_lp0 = 0

            try:
                reg_apps_lp1 = int(str(regdata[[RegisterColumns['apps_lp1']]].values[0][0]), 16)
            except ValueError:
                print "Warning!!! %s loop1, No valid reset data found, set to zero CMD: 0x%x" % (DataType['type'], reg_adr)
                reg_apps_lp1 = 0

            try:
                reg_bits = int(regdata[[RegisterColumns['nbits']]].values[0][0])
            except ValueError:
                print "Error!!! %s, No valid bits entry found CMD: 0x%x" % (DataType['type'], reg_adr)
                excel_error = 1

            try:
                reg_en = int(regdata[[RegisterColumns['supported']]].values[0][0] == 'y')
            except ValueError:
                print "Error!!! %s, No valid supporte field found CMD: 0x%x" % (DataType['type'], reg_adr)
                excel_error = 1

            try:
                reg_cmd_num_bytes = int(regdata[[RegisterColumns['cmd_num_bytes']]].values[0][0])
            except ValueError:
                print "Error!!! %s, No valid num bytes entry found CMD: 0x%x" % (DataType['type'], reg_adr)
                excel_error = 1

            if reg_res_lp0 > (1 << reg_bits):
                print "Error!!! %s loop 0 Bitlength mismatch CMD: 0x%x Numbits: %d, Data: 0x%x" % (
                DataType['type'], reg_adr, reg_bits, reg_res_lp0)
                excel_error = 1

            if reg_res_lp1 > (1 << reg_bits):
                print "Error!!! %s loop 1 Bitlength mismatch CMD: 0x%x Numbits: %d, Data: 0x%x" % (
                DataType['type'], reg_adr, reg_bits, reg_res_lp1)
                excel_error = 1

            if reg_apps_lp0 > (1 << reg_bits):
                print "Error!!! %s loop 0 Bitlength mismatch CMD: 0x%x Numbits: %d, Data: 0x%x" % (
                DataType['type'], reg_adr, reg_bits, reg_res_lp0)
                excel_error = 1

            if reg_apps_lp1 > (1 << reg_bits):
                print "Error!!! %s loop 1 Bitlength mismatch CMD: 0x%x Numbits: %d, Data: 0x%x" % (
                DataType['type'], reg_adr, reg_bits, reg_res_lp1)
                excel_error = 1

            # Only collect data if there is no error
            if excel_error == 0:
                reg_cmd_num_bytes_total += reg_cmd_num_bytes
                RegsStructure_tmp += [[reg[0], reg_adr, reg_en, reg_bits]]
                RegsData_tmp += [[reg[0], reg_adr, reg_bits, reg_res_lp0, reg_res_lp1, reg_apps_lp0, reg_apps_lp1]]
                RegsStructList_tmp += [reg[0], reg_adr, reg_en, reg_bits]
                RegsDataList_tmp += [[reg[0], [reg_adr, reg_bits, reg_res_lp0, reg_res_lp1, reg_apps_lp0, reg_apps_lp1]]]
                
    #the first 32bit value in data array is total number of bytes + number of commands
    #generate size and number of commands check
    pmbus_size_num_check = reg_cmd_num_bytes_total + numr
    RegsStructure = [[u'SIZE_NUM_CHECK', 0, 1, 32]]
    RegsStructure += RegsStructure_tmp
    RegsData = [[u'SIZE_NUM_CHECK', 0, 32, pmbus_size_num_check, pmbus_size_num_check, pmbus_size_num_check, pmbus_size_num_check]]
    RegsData_tmp += [[reg[0], reg_adr, reg_bits, reg_res_lp0, reg_res_lp1, reg_apps_lp0, reg_apps_lp1]]
    RegsData += RegsData_tmp
    RegsStructList = [u'SIZE_NUM_CHECK', 0, 1, 32]
    RegsStructList += RegsStructList_tmp
    RegsDataList = [[u'SIZE_NUM_CHECK', [0, 32, pmbus_size_num_check, pmbus_size_num_check, pmbus_size_num_check, pmbus_size_num_check]]]
    RegsDataList += RegsDataList_tmp

    # Pack the data bit by bit into a byte list
    # Per loop, default and user are currently the same
    for s in RegsData:
        # reg_res_lp0
        PackDefaultDataTbl_lp0.pack(s[3], s[2])
        # reg_apps_lp0
        PackUserDataTbl_lp0.pack(s[5], s[2])
        # reg_res_lp1
        PackDefaultDataTbl_lp1.pack(s[4], s[2])
        # reg_apps_lp1
        PackUserDataTbl_lp1.pack(s[6], s[2])
        num_bits_total += s[2]

    # Pack Structural bits
    for s in RegsStructure:
        PackRegsTbl.pack(s[1], 8)
        structural_bits_total += 8
        PackRegsTbl.pack(s[3], 9)
        structural_bits_total += 9
        PackRegsTbl.pack(s[2], 1)
        structural_bits_total += 1

    # Terminate the Structure list with a '0' indicating that there
    # are no more instances to follow
    PackRegsTbl.pack(0, 3)

    RegsStructList += ['total_data_bits', num_bits_total, int(ceil(num_bits_total / 8))]
    RegsStructList += ['total_structural_bits', structural_bits_total, int(ceil(structural_bits_total / 8))]
    RegsStructList += ['total_number_registers', num_registers_total]
    RegsStructList += ['total_addr_mask', num_registers_total * 32 * 2, int(ceil(num_registers_total * 4 * 2))]

    # Convert the lists into an array
    PMBUSDefaultDataArray_lp0 = array("B")
    PMBUSUserDataArray_lp0 = array("B")
    PMBUSDefaultDataArray_lp1 = array("B")
    PMBUSUserDataArray_lp1 = array("B")

    RegsStructArray.fromlist(PackRegsTbl.data)

    PMBUSDefaultDataArray_lp0.fromlist(PackDefaultDataTbl_lp0.data)
    PMBUSUserDataArray_lp0.fromlist(PackUserDataTbl_lp0.data)

    PMBUSDefaultDataArray_lp1.fromlist(PackDefaultDataTbl_lp1.data)
    PMBUSUserDataArray_lp1.fromlist(PackUserDataTbl_lp1.data)

    PMBUDataArrayList.extend(
        [[PMBUSDefaultDataArray_lp0], [PMBUSUserDataArray_lp0], [PMBUSDefaultDataArray_lp1], [PMBUSUserDataArray_lp1]])

def zfillarr(data, size):
    """
    fills a byte array with zeroes
    :param data: array to be filled with zeros
    :param size: number of zeroes to be filled
    :return: none
    """

    for s in range(size):
        data.append(0)


def create_hdr(data, info):
    """
    This function will append a 16 byte header to the data field.
    The header consists of following fields
    CMD         16 bit      defines the data type
    Option      16 bit      defines a subtype
    length      16 bit      length of the data including header
    lengthr     16 bit      redundant length field
    crchdr      32 bit      CRC for the header field
    crcdata     32 bit      CRC for the data

    :param data: Binarray holding the data which sould get a header
    :param info: header info
    :return header_bin: Binarray of the header
    """

    # fill zeroes to align with 32bit words
    size = 4 - (len(data) % 4)
    if size < 4:
        zfillarr(data, size)

    option = 0
    if crc_hdr is True:
        option |= 0x1
    if crc_data is True:
        option |= 0x2

    # size in Bytes of 'Patch_Cmd_hdr_t' + 'Patch_Cmd_Image_t/Patch_Cmd_Image_Version_t'
    # remove the data_crc from the header and append it at the end of data
    header_size = 12
    # The total size is including header data adn 4 byte data_crc
    total_length = header_size + len(data) + 4

    hdr_bin = array("B")

    Short2BinArray(hdr_bin, info["cmd"])  # CMD information
    Short2BinArray(hdr_bin, option)  # Option information
    Short2BinArray(hdr_bin, total_length)  # Length information
    Short2BinArray(hdr_bin, total_length)  # Redundant length information

    # calculate the header CRC-32 value (setting the CRC fields to zero)
    hdr_crc = crc32(hdr_bin)

    # calculate the data checksum
    data_crc = crc32(data)

    # update header checksum field
    if crc_hdr is True:
        # uint32_t crc_hdr;
        Long2BinArray(hdr_bin, hdr_crc)
    else:
        # uint32_t crc_hdr;
        Long2BinArray(hdr_bin, 0)

    # update data checksum field at the end of data
    if crc_data is True:
        # uint32_t crc_hdr;
        Long2BinArray(data, data_crc)
    else:
        # uint32_t crc_hdr;
        Long2BinArray(data, 0)

    return hdr_bin


def create_regs_ceedling_data(datalist, ceedlinglist):
    """
    This function creates a array containing 32 bit address and 32bit data for registers

    :param datalist List holding register information
    :param ceedlinglist List hodlding the ceedling data, register address and content
    :return none
    """

    # Stores only address and data
    for i in datalist:
        ceedlinglist += [[i[1][0], i[1][1]]]


def create_pmbus_ceedling_data(DataArray, CeedlingList):
    """
    This function creates a list containing th PMBUS data (plain 8bit)

    :param DataArray Array of bytes  holding pmbus data
    :param ceedlinglist List holding pmbus data
    """

    # Default LP0, User LP0, Default LP1, User LP1
    CeedlingList += [DataArray[0][0].tolist(), DataArray[1][0].tolist(), DataArray[2][0].tolist(), DataArray[3][0].tolist()]


def log_store(s, LogFile):
    """
    This function writes a log of the configuration data to a file for debugging

    :param s: String with the log content
    :param LogFile: Filename for the log
    :return:
    """
    # open log output file
    f = open(LogFile, "w")
    # write log file
    f.write(s)
    # close the file
    f.close()


def GetData(app, Dataxls, DataType, RegsStructArray, RegsDataArrayList, RegisterCeedlingList):
    """
    This function reads structure and data from the Excel Table

    :param app: String containing the name of the application to process
    :param Dataxls: Database containing the corresponding xls file
    :param DataType: List containing type specific parameters
    :param RegsStructArray: Array containing the structure of registers or commands
    :param RegsDataArrayList: List of Arrays containing the default data of registers or commands
    :param RegisterCeedlingList: Register list for use in ceedling
    :param LogString: String containing debug information
    :return:
    """

    RegsStructListLocal = []
    RegsStuctArrLocal = array("B")
    RegsDataListLocal = []
    RegsDataArrLocal = array("B")
    PMBUSDataArrListLocal = []
    try:
        if DataType["type"] == 'PMBUS':
            GetPMBusData(app, Dataxls, DataType, RegsStructListLocal, RegsStuctArrLocal, RegsDataListLocal,
                         PMBUSDataArrListLocal)
        else:
            GetRegData(app, Dataxls, DataType["type"], RegsStructListLocal, RegsStuctArrLocal,
                                                       RegsDataListLocal, RegsDataArrLocal)
    except IOError as e:
        return e

    s = ''
    s += "\n\n%s Register Structure Table:\n" % DataType["type"]
    for item in RegsStructListLocal:
        if isinstance(item, unicode) | isinstance(item, str):
            s += "\n%s " % item
        else:
            s += "%s " % item

    s += "\n\n%s Register Data:\n" % DataType["type"]

    if DataType["type"] != 'PMBUS':
        for item in RegsDataListLocal:
            s += "\n%30s " % item[0]
            # Address
            s += "%10x " % item[1][0]
            # Data
            s += "%10x " % item[1][1]
            # Length
            s += "%10x " % item[1][2]
    else:
        for item in RegsDataListLocal:
            s += "\n%40s " % item[0]
            # CMD
            s += "%4x " % item[1][0]
            # Bits
            s += "%4x " % item[1][1]
            # Data Reset LP0
            s += "%30x " % item[1][2]
            # Data Reset LP1
            s += "%30x " % item[1][3]
            # Data User LP0
            s += "%30x " % item[1][4]
            # Data User LP1
            s += "%30x " % item[1][5]

    s += "\n\n%s Registers Structure Array Length: %d \n" % (DataType["type"], len(RegsStuctArrLocal))
    s += "%s Registers Structure Array: %s \n" % (DataType["type"], RegsStuctArrLocal)
    hdr = create_hdr(RegsStuctArrLocal, {'cmd': DataType['cmds']})

    RegsStructArray.extend(hdr + RegsStuctArrLocal)
    # Data are stacked so no filler needed
    # filler = 1024 - (len(RegsStructArray) % 1024)
    # zfillarr(RegsStructArray, filler)

    xl = 0
    if DataType["type"] == 'PMBUS':
        for l in PMBUSDataArrListLocal:
            s += "%s %s Registers Data Array Length: %d\n" % (DataType["type"], DataType["cmdd"][xl], len(l[0]))
            s += "%s %s Registers Data Array: %s\n" % (DataType["type"], DataType["cmdd"][xl], l[0])
            hdr = create_hdr(l[0], {'cmd': DataType["cmdd"][xl]})
            RegsDataArrayList.extend([hdr + l[0]])
            xl += 1
        create_pmbus_ceedling_data(PMBUSDataArrListLocal, RegisterCeedlingList)
    else:
        s += "%s Registers Data Array Length: %d\n" % (DataType["type"], len(RegsDataArrLocal))
        s += "%s Registers Data Array: %s\n" % (DataType["type"], RegsDataArrLocal)
        hdr = create_hdr(RegsDataArrLocal, {'cmd': DataType['cmdd']})
        RegsDataArrayList.extend([hdr + RegsDataArrLocal])
        create_regs_ceedling_data(RegsDataListLocal, RegisterCeedlingList)

    # print "%s" % s
    return s


def ceedling_register_store(TrimRegsCeedlingList, ConfRegsCeedlingList, CeedlingRegfile):
    """
    This function writes a c file which contains the registers available in OTP

    :param TrimRegsCeedlingList: Trim registers content
    :param ConfRegsCeedlingList: Conf registers content
    :param CeedlingRegfile: Filename to store the data
    :return:
    """
    RegsListString = \
        """
/*
 * register_dump_ACF_P5_3P3V_30A.c
 *
 *  Created on: May 22, 2017
 *      Author: zojerh
 */

#include "register_dump_ACF_P5_3P3V_30A.h"
#include "ceedling_hal_api_mem_mapper.h"   /* Mocking model for SFR accesses */
#include <stdint.h>
#include <unity.h>

#define OTP_REGISTER_COLS (2)

void check_registers(void)
{
    uint32_t x;

    uint32_t t[][OTP_REGISTER_COLS] = {
        """
    RegsListString += '\t\t\n'
    for i in TrimRegsCeedlingList:
        RegsListString += '\t\t{ ' + hex(i[0]) + ', '
        RegsListString += hex(i[1]) + ' },\n'
    for i in ConfRegsCeedlingList:
        RegsListString += '\t\t{ ' + hex(i[0]) + ', '
        RegsListString += hex(i[1]) + ' },\n'

    RegsListString += '\t\t};\n'
    RegsListString += \
        """
    for (int32_t i = 0; i < sizeof(t)/(2*sizeof(uint32_t)); i++)
    {
        x = ceedlingHAL_API_SFR_TraceGetAssert( (uint32_t) t[i][0]);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(t[i][1], x, "The data read from the register is not equal to the expected value");

    }
}
        """

    # open ceedling register output file
    f = open(CeedlingRegfile, "w")
    # write Ceedling register file
    f.write(RegsListString)
    # close the file
    f.close()


def ceedling_pmbus_store(CeedlingList, Ceedlingfile):
    """
    This function writes a c file which contains the pmbus data available in OTP

    :param CeedlingList: PMBUS data content
    :param Ceedlingfile: Filename to store the data
    :return: None
    """
    RegsListString = \
        """
/*
 * ceedling_pmbus_ACF_P5_3P3V_30A.c
 *
 *Created on: May 22, 2017
 *Author: zojerh
 */

#include "ceedling_pmbus_ACF_P5_3P3V_30A.h"
#include <stdint.h>
#include <unity.h>

#define PMBUS_STORES (4)

void check_pmbus_cmds(uint8_t* default_lp0, uint8_t* user_lp0, uint8_t* default_lp1, uint8_t* user_lp1)
{
    uint8_t tpmbus[][PMBUS_STORES] = {
    """

    RegsListString += '\n'
    for i in range(len(CeedlingList[0])):
        RegsListString += '\t\t\t\t{ %5s,' % hex(CeedlingList[0][i])
        RegsListString += '%5s,' % hex(CeedlingList[1][i])
        RegsListString += '%5s,' % hex(CeedlingList[2][i])
        RegsListString += '%5s }, \n' % hex(CeedlingList[3][i])

    RegsListString += '\t\t\t};\n'
    RegsListString += \
        """
     for (int32_t i = 0;    i < sizeof(tpmbus) / (PMBUS_STORES * sizeof(uint8_t)); i++)
    {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tpmbus[i][0],  *default_lp0++, "The data read from default lp0 is not equal to the expected value");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tpmbus[i][1], *user_lp0++, "The data read from user_lp0 is not equal to the expected value");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tpmbus[i][2],  *default_lp1++, "The data read from default lp1 is not equal to the expected value");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tpmbus[i][3], *user_lp1++, "The data read from user_lp1 is not equal to the expected value");
    }
}
        """
    # open ceedling register output file
    f = open(Ceedlingfile, "w")
    # write Ceedling register file
    f.write(RegsListString)
    # close the file
    f.close()

def configuration_structure_store(TrimRegsStructArray, ConfRegsStructArray, ConfigurationStrutureFile):
    """
    This function writes a c file which contains structure data for trim and config registers. It can be used to load into rom

    :param TrimRegsStructArray: Trim structure data
    :param ConfRegsStructArray: Conf structure data
    :param ConfigurationStrutureName: Filename to store the data
    :return:
    """
    ConfStructureString = \
        """/*
 * configuration_structure.c
 *
 *  Created on: Dec 13, 2017 with python
 *      Author: zojerh
 */

#include "configuration_structure.h"
#include <stdint.h>             /* uint*_t */

/** trim_struct_data contains the structural information for trim data */
const uint8_t trim_struct_data[] = { 
        """
    ConfStructureString += '\t\t\n'
    for i in range(0, len(TrimRegsStructArray)):
        ConfStructureString += '\t\t' + str(TrimRegsStructArray[i]) + ',\n'

    ConfStructureString += \
        """
        };
        
/** conf_struct_data contains the structural information for conf data */
const uint8_t conf_struct_data[] = { 
        """

    ConfStructureString += '\t\t\n'
    for i in range(0, len(ConfRegsStructArray)):
        ConfStructureString += '\t\t' + str(ConfRegsStructArray[i]) + ',\n'
        
    ConfStructureString += \
        """
        };\n

        
        """

    # open configuration structure output file
    f = open(ConfigurationStrutureFile, "w")
    # write file
    f.write(ConfStructureString)
    f.write('\n\n')
    # close the file
    f.close()
   
def pmbus_configuration_data_store_header(PmbusConfigurationDatafile2):

    """
    This function writes a c file which contains data for pmbus commands. It can be used to load into rom

    :param PMBUSDataArrayList: PMBUS data
    :param PmbusConfigurationDatafile: Filename to store the data
    :return:
    """
    PmbusDataString = \
        """/*
/*
 * pmbus_patch_configuration_data.h
 *
 ** Created on:  %s
 */
 /* ============================================================================
** Copyright (C) %s Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/

#ifndef SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_
#define SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_

#define Patch_Pmbus_Table

#ifdef Patch_Pmbus_Table
extern void PMBUS_Restore_All(PMBUS_PAGE_t page, uint8_t * p);
void patch_PMBUS_HANDLE_RESTORE_DEFAULT_ALL(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
#endif

#endif /* SRC_USER_APP_PMBUS_PATCH_CONFIGURATION_DATA_H_ */
        """
    
    today = datetime.date.today()
    year = today.strftime('%Y')
    today_date = today.strftime('%Y-%m-%d')

    PmbusDataString = PmbusDataString % (today_date, year)
    # open configuration structure output file
    f = open(PmbusConfigurationDatafile2, "w")
    # write file
    f.write(PmbusDataString)
    f.write('\n\n')
    # close the file
    f.close()


def pmbus_configuration_data_store(PMBUSDataArrayList, PmbusConfigurationDatafile):

    """
    This function writes a c file which contains data for pmbus commands. It can be used to load into rom

    :param PMBUSDataArrayList: PMBUS data
    :param PmbusConfigurationDatafile: Filename to store the data
    :return:
    """
    PmbusDataString = \
        """/*
 * pmbus_patch_configuration_data.c
 *
  ** Created on:  %s
 */
 /* ============================================================================
** Copyright (C) %s Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/

#include "driver_common.h"       // scratchpad memory
/* Standard library */
#include <stdint.h>             /* uint*_t */
#include <string.h>             /* memset */
/* APIs */
#include "pmbus_cmd_handlers.h"
#include "pmbus_api.h"
#include "faults_api.h"
#include "faults_drv.h"
#include "regulation_drv.h"
#include "pmbus_autogen.h"
#include "system_shasta.h"  // sys_reset function
#include "otp_drv.h"             /* OTP initialize */
#include "otp_fs.h"             /* Save/get user parameter to/from OTP */
#include "pmbus_patch_configuration_data.h"

#ifdef Patch_Pmbus_Table
/** trim_struct_data contains the structural information for trim data */
const uint8_t patch_pmbus_default_data_array_loop0[] = { 
        """
    PmbusDataString += '\t\t\n'
    for i in range(0, len(PMBUSDataArrayList[0])):
        PmbusDataString += '\t\t' + str(PMBUSDataArrayList[0][i]) + ',\n'

    PmbusDataString += \
        """
        };
        
/** conf_struct_data contains the structural information for conf data */
const uint8_t patch_pmbus_default_data_array_loop1[] = { 
        """

    PmbusDataString += '\t\t\n'
    for i in range(0, len(PMBUSDataArrayList[2])):
        PmbusDataString += '\t\t' + str(PMBUSDataArrayList[2][i]) + ',\n'
        
    PmbusDataString += \
        """
        };
        
/**
 * Callback or Handle function called by pmbus thread after a read or write of given command
 * @param page Selects the pmbus page to operate on.
 * @param direction Can either be PMBUS_WRITE or PMBUS_READ types
 */

void patch_PMBUS_HANDLE_RESTORE_DEFAULT_ALL(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction)
{
	if (direction == PMBUS_WRITE)
	{
		uint8_t * p;
		uint32_t size;
		uint32_t size_check;
		Otp_File_System_Cmd_Hdr_t* config_ptr = NULL;
		// first grab the trim and config restore results:
		CONFIGURATOR_ERROR_t error = configurator_error_get();
		if (error != CONFIGURATOR_NO_FAULT)
		{
			PMBUS_Log_Error(PMBUS, PROGRAM_COUNTER, PMBUS_STATUS_CML_OTHER_MEM_FAULT);  // we use bit0 other mem fault for trim and config issues
			configurator_error_clear();  // clear the config error
			configurator_pmbus_default_set((uint8_t)page);  // try to see if we have a default pmbus image, load this regardless of trim/config status
		}
		p = configurator_pmbus_default_ptr_get((uint8_t)page);

		size_check = ((uint32_t)p[3]<<24U) |  ((uint32_t)p[2]<<16U) | ((uint32_t)p[1]<<8U) | (uint32_t)p[0];
		// grab expected size plus command count from command descriptors
		size = PMBUS_Calculate_Size_Check();
		if (size != size_check)
		{
			// point all the loops to loop0 since the default data is the same for all loops
			 config_ptr = (Otp_File_System_Cmd_Hdr_t*)patch_pmbus_default_data_array_loop0;
			 p = (uint8_t *) config_ptr + sizeof(Otp_File_System_Cmd_Hdr_t);
		}
		PMBUS_Restore_All(page, p);
	}
}
#endif\n
        """

    today = datetime.date.today()
    year = today.strftime('%Y')
    today_date = today.strftime('%Y-%m-%d')

    PmbusDataString = PmbusDataString % (today_date, year)
    # open configuration structure output file
    f = open(PmbusConfigurationDatafile, "w")
    # write file
    f.write(PmbusDataString)
    f.write('\n\n')
    # close the file
    f.close()
    
def OtpMap_store(OTPMapString, otpMapFile):
    """
    This function writes a txt file whioch contains information about the various files in an OTP

    :param file_sizes: list of File sizes
    :param otpMapFile: Filename to store the data
    :return: None
    """

    MapString = \
    """################################################################################################
# OtpMap.txt    
# Automatically created by config2otp.py
# This file contains upper and lower limits for fields in a configuration OTP which can be randomized
# Everything outside the range must kept as is
# The numbers are absolute byte positions inside the otp file starting a 0
################################################################################################
"""

    MapString += OTPMapString

    # open otpmapoutput file
    f = open(otpMapFile, "w")
    # write otpMapFile
    f.write(MapString)
    # close the file
    f.close()


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


def otp_txt_file_write(a, filename):
    """
    Write the given binary array eleme 'a' to the OTP data content file in txt format 'filename'
    No commend and full OTP length
    :param a: byte array
    :param filename: filename or path to the file
    :return:
    """

    f = open(filename, 'w')
    i = 0
    for elem in a:
        hexByte = "%02x" % elem
        if i >= 16:
            i = 0
            # EOL
            # insert CR
            f.write("\n")
        f.write(hexByte)
        i += 1

    f.write("\n")
    f.close()


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

    #ih.frombytes(a)

    # open the output file
    #f = open(otpHexFile, "wb")

    # write config data as a binary file
    ih.tofile(filename, format='hex')

    # close the file
    #f.close()


def data_comment_generate(a):
    """
    This function generates an string array which contains dat file
    specific comments.
    Each comment string represents the binary data of the function argument array.

    :param a: byte array
    :return:
    """

    str = []
    intro = "Data: "
    new_string = intro
    index = 0
    for elem in a:
        byte_string = " 0x%02X" % elem
        new_string += byte_string

        index += 1
        if index >= 16:
            str.append(new_string)
            new_string = intro
            index = 0

    if len(new_string) > 0:
        str.append(new_string)

    return str


def bin2otp(a, otpbinFile, otpHexFile, otpTxtFile):
    """
    Reads the patch bin files and generates the OTP data file.

    :param BinArray: ByteArray containing all config data
    :return:
    """

    global data_comment
    global odd_rows_inverted

    odd_rows_inverted = True
    data_comment = True

    otpOffset = 0

    # insert dummy bytes till offset
    for i in range(otpOffset):
        a.append(0)

    # generate a copy of the given array 'a'
    # This array is used ot printout the comments in each output dat file. The comment
    # should contain the origin data bytes (not inverted and no MSB/LSB swap
    a2 = array('B')
    a2.fromstring(a.tostring())

    comment_list = []
    if data_comment:
        # generate a comment list
        comment_list = data_comment_generate(a2)

    # invert raw based byte order MSB Byte first, LSB Byte last
    a_full_size = array('B')
    a_full_size.fromstring(a.tostring())

    size = 65535 - len(a_full_size)
    zfillarr(a_full_size, size)

    a = raw_lsb_msb_invert(a)
    a_full_size = raw_lsb_msb_invert(a_full_size)

    if odd_rows_inverted is True:
        # invert the odd OTP rows (1 row = 128 Bit = 16 Byte)
        a_oddrow_inv = invert_odd_raw_bytes(a)
        a_full_size_oddrow_inv = invert_odd_raw_bytes(a_full_size)

    # txt file for the FPGA
    otp_txt_file_write(a_full_size_oddrow_inv, otpTxtFile)
    a = raw_lsb_msb_invert(a)
    otp_bin_file_write(a, otpbinFile)
    otp_hex_file_write(a, otpHexFile)

    return 0


def GetApps(PMBUSxls):
    """
    This function determines generates a lists of all available apps using the PMBUS file

    :param PMBUSxls: Database containing the PMBUS xls
    :return: list of applications
    """
    import re
    sheet = 'Shasta'
    reglist = PMBUSxls.parse(sheet)
    header_cols_list = list(reglist)

    app = 'Sim'

    # Search for an apps column
    AppsList = [re.search('Sim (.*)\.0',s).group(1) for s in header_cols_list if (app in s) and ('.0' in s)]
    return AppsList

def otp_map_limits(filename, start, size ):
    """
    This function processes the register map table for each file

    :param fieldname: String containing the name of the otp file
    :param start: absolute start address for this file  
    :param size: file size in bytes
    :return: partmap: containing the map data for this file
    """
    partmap  = filename + '_cmd_lower 0x%x \n'  %  (start + 0)
    partmap += filename + '_cmd_upper 0x%x \n'  %  (start + 3)
    partmap += filename + '_siz_lower 0x%x \n'  %  (start + 4)
    partmap += filename + '_siz_upper 0x%x \n'  %  (start + 7)
    partmap += filename + '_hdr_lower 0x%x \n' %  (start +  8)
    partmap += filename + '_hdr_upper 0x%x \n' %  (start + 11)
    partmap += filename + '_dat_lower 0x%x \n'  %  (start + 12)
    partmap += filename + '_dat_upper 0x%x \n' %  (start + size - 1)
    
    return partmap
    
    
    
def ProcessApps(app, OtpMap, MFR_en, PMBUSxls):
    """
    This function processes Register and PMBUS settings for a specific application

    :param app: String containing the name of the application to process
    :param OtpMap: Boolean to indicate that a OTP Map shall be created  
    :param MFR_en: Boolean to indicate that PMBUS MFR specific commands shall be included  
    :param process_default: Flag to indicate if default application is used which loads only reset values
    :param PMBUSxls: Database containing the PMBUS xls
    :return:
    """

    mfrname = ''
    if MFR_en == 1:
        mfrname = '_mfr'
        
    fprefix = 'config_otp_'
    otpmapname = fprefix + mfrname + 'map.txt'

    PmbusConfigurationDataName = "pmbus_patch_configuration_data.c" 
    PmbusConfigurationDataHeader = "pmbus_patch_configuration_data.h" 
    LogFilename = "log_" + app + mfrname + ".txt"
    PmbusConfigurationDatafile = os.path.join(binfpath, PmbusConfigurationDataName)
    PmbusConfigurationDatafile2 = os.path.join(binfpath, PmbusConfigurationDataHeader)
    LogFile = os.path.join(binfpath, LogFilename)

 #   otpMapFile = os.path.join(fpath, otpmapname)
    
    PMBUSStructArray = array("B")

    PMBUSDataArrayList = []
    PMBUSDataCeedlingList = []
    LogString = ''

    print "Processing of %s  ..." % app
    if app == 'default':
        app = 'ACF_P5_3P3V_30A'
        process_default = True
    else:
        process_default = False

    try:
        # Generate  PMBus Structure and data
        DataType = {'type': 'PMBUS', 'cmds': 0x0005, 'cmdd': [0x006, 0x007, 0x008, 0x009], 'mfr_en': MFR_en}
        LogString += GetData(app, PMBUSxls, DataType, PMBUSStructArray, PMBUSDataArrayList, PMBUSDataCeedlingList)
    except IOError as e:
        import errno
        if e.errno not in (0, errno.EPIPE):
            raise

    BinArray = array("B")

    # create C doe out of the structure files

    BinArray.extend(PMBUSStructArray)

#    if OtpMap:
#        x = 0
#       OTPMapString = otp_map_limits('PMBUS_structure', x, len(PMBUSStructArray)) 
#        x += len(PMBUSStructArray)        
#        OTPMapString += otp_map_limits('PMBUS_lp0_data', x, len(PMBUSDataArrayList[0]))
#        x += len(PMBUSDataArrayList[0])        
#        OTPMapString += otp_map_limits('PMBUS_lp1_data', x, len(PMBUSDataArrayList[2]))
 
#        OtpMap_store(OTPMapString, otpMapFile)
                    

    # create C code out of the configuration structure files
    #if process_default and MFR_en == 0:
    if process_default:
        pmbus_configuration_data_store(PMBUSDataArrayList, PmbusConfigurationDatafile)
        print "Generated %s  ..." % PmbusConfigurationDatafile
        pmbus_configuration_data_store_header(PmbusConfigurationDatafile2)
        print "Generated %s  ..." % PmbusConfigurationDatafile2
        
    log_store(LogString, LogFile)

def main():
    import sys

    global binfile
    global xlsfile
    global pmbus_xlsfile
    global fwconf_xlsfile
    global fpath
    global binfpath
    global crc_hdr
    global crc_data
    global otpxvalentHexfile
    global otpxvalentBinfile
    

#    fpath = 'otp_files'
    binfpath = sys.argv[2]
    
#    try:
#        os.mkdir(fpath)
#    except:
#        if os.path.isdir(fpath):
#            pass
    try:
        os.mkdir(binfpath)
    except:
        if os.path.isdir(binfpath):
            pass

    crc_hdr = True
    crc_data = True

    if sys.platform == "linux" or sys.platform == "linux2":
        conf = os.environ['CONF']
        prefix = os.environ['PROJECTNAME'] + os.environ['PROJREV']
    else:
        conf = '.'
        prefix = 'shasta'


    #pmbus_xlsfile  = os.path.join(conf, prefix + "_pmbus_patch.xlsx
    pmbus_xlsfile = sys.argv[1]

    
    print "Processing files:"
    print "    " + pmbus_xlsfile


    try:
        PMBUSxls = pd.ExcelFile(pmbus_xlsfile)
    except IOError as e:
        sys.stderr.write("Error reading file: %s\n" % e)
        return 1

    # We only search in PMBUS because its easier
    # Names of the apps should be the same in RegMap
    #apps = GetApps(PMBUSxls)


    apps = ['default']

    # Only create an OTP map for the first application because all other maps are the same
    # except for default

    # Create configs with MFR
    OtpMap = True
    MFR_en = 1
    print "Processing with MFRs"
    for app in apps:        
        #ProcessApps(app, OtpMap, MFR_en, RegMapxls, PMBUSxls)
        ProcessApps(app, OtpMap, MFR_en, PMBUSxls)
        OtpMap = False

    
if __name__ == '__main__':
    import sys

    sys.exit(main())
