#!/usr/bin/python
import sys
import os.path
from types import InstanceType
from sre_constants import RANGE

USAGE = """
Generates the partial patch RAM memory range parameter for the
linking process.

It concatenates the memory regions/sections of all given ELF images,
to generate an overall memory usage summary.

The generated output linker configuration then contains a
memory range which is not used by any given ELF file and is inside
the requested configuration memory area.

Usage:
   python elf_ram_allocater.py [options] <elf_files>

Options:
    -h, --help          Print this help message and exit.

    -c <cfg_file>       Configuration file.

                        The configuration file should contain parameter like:
                        - RAM start and end address where to search for free space
                        - Minimum RAM size which is required for the patch
                        The format of the file follows the standard 'ini'-file style.

                        Example for the configuration file with the corresponding tags:
                            [Patch]
                            startaddress = 0x20063000
                            endaddress = 0x20063fff
                            minramsize = 0x400
                            offset = 0

                       The 'offset' parameter places additional dummy Bytes in front of the
                       allocated memory range.
                       This additional memory could be used for additional protocol framing.

                       Note: The 'offset' parameter is optional and could be skipped.
                       A default offset of 0 Bytes is used in case this parameter is not given.

    -a <addr_h_file>    Output address header file. It contains the RAM
                        address region(s) which the patch linker process
                        can use to store code and data.

    -g <gnu__ls_file>   GNU Linker configuration output file. It contains the RAM
                        address region(s) which the patch linker process
                        can use to store code and data.

Arguments:
    <elf_files>        One or multiple ELF image files, which consist of the
                       firmware ROM code and additional partial patches.
                       These files are the references when generating the
                       partial patch RAM location.
"""

def AddRelativeLibPath(path):
    """Add Python library include path"""
    current_dir = os.path.normpath(__file__+"\\..")
    lib_dir = os.path.normpath(current_dir+"\\"+path)
    sys.path.insert(0, lib_dir)

AddRelativeLibPath("../python/pyelftools")
AddRelativeLibPath("lib")


import configparser 

verbose = False
def print_verbose(x):
    '''
    Print out the given string in case the verbose flag is enabled.
    '''
    if verbose is True:
        print "%s: %s" % (sys.argv[0],x)

class MemRegion(object):
    """memory region"""
    def __init__(self, name, address, size):
        self.name = name
        self.address = address
        self.size = size

    def __cmp__(self, other):
        return cmp(str(self.address)+str(self.size), str(other.address)+str(other.size))

    def copy(self):
        """Return an object copy"""
        return MemRegion(self.name, self.address, self.size)

    def dump(self):
        """Dump the memory region"""
        print_verbose("Name: %s, Address:0x%08X, Size: 0x%X(%d)\n" % (self.name, self.address, self.size, self.size))

    def do_ranges_overlap(self, other):
        """Return True in case of range overlap"""
        if self.address >= other.address and self.address < other.address+other.size:
            return True
        if other.address >= self.address and other.address < self.address+self.size:
            return True
        return False

    def do_ranges_hit(self, other):
        """Return True in case the range boundaries hit"""
        if other.address == self.address+self.size:
            return True
        if self.address == other.address+other.size:
            return True
        return False

    def merge(self, other):
        """Return a merged region with covers both region, including any optional memory gap between both."""
        address = min(self.address,other.address)
        end_address = max(self.address+self.size,other.address+other.size)
        return MemRegion(self.name+"/"+other.name, address, end_address - address)

class MemRegions(list):
    """memory region set"""

    def copy(self):
        """Return an object copy"""
        new_regions = MemRegions()
        for region in self:
            new_regions.append(region.copy())
        return new_regions

    def merge_regions(self):
        """
        Merge adjacent regions.
        Regions are merged if the memory gap between both
        does not exceed the given gap size.

        @return Return a new MemRegions() element with the merged region elements
        """
        copy_MemRegions = self.copy() # Only work with a data copy
        regions = sorted(copy_MemRegions)

        i = 0
        while i < len(regions):
            while ((i+1 < len(regions)) and (regions[i].do_ranges_overlap(regions[i+1]) or regions[i].do_ranges_hit(regions[i+1]))):
                regions[i] = regions[i].merge(regions[i+1])
                del regions[i+1]
            i = i+1

        copy_MemRegions.regions = regions
        return copy_MemRegions

    def find_unused_range(self, start, end, size):
        """
        Find an unused region within the given area and has the given size.
        @param start The search algorithm starts searching for a memory region at the given start address
        @param end The search algorithm stops searching for a memory region at the given end address. The returned memory range MUST not exceed this end address.
        @param size Minimum size of returned memory region
        """

        if start+size > end:
            raise Exception("Free requested RAM section size %d exceeds specified range 0x%08X--0x%08X" % (size, start, end))

        address = start
        while True:
            range_corrected = False
            for elem in sorted(self):
                if elem.do_ranges_overlap(MemRegion("unused", address, size)):
                    if elem.address+elem.size < end - size:
                        address = elem.address+elem.size
                        range_corrected = True
                    else:
                        #no free range found
                        raise Exception("No free RAM section found with the given range 0x%08X--0x%08X (size=%d)" % (start, end, size))
            if range_corrected is False:
                    return MemRegion("unused", address, size)

    def dump(self):
        """Dump the memory regions"""
        for region in self:
            region.dump()

class ELFImage(object):
    """Parsing the input ELF files and extract the used ROM/RAM memory regions."""
    def __init__(self, filename):
        """Parse and analyse the given ELF file"""
        self.filename = filename

        from elftools.elf.elffile import ELFFile
        from elftools.elf.sections import SymbolTableSection

        try:
            elffile = ELFFile(open(filename, "rb"))
        except:
            raise Exception("Could not load ELF file: '%s'" % filename)

        self.sections = []
        for section in elffile.iter_sections():
            if isinstance(section, SymbolTableSection):
                # Symbol tables do not contain any memory range usage information
                continue
            if section["sh_flags"] == 0:
                # No code or data segments
                continue
            self.sections.append(section)

    def get_mem_range(self):
        """Get a memory region describing the whole ELF file content"""
        regions = MemRegions()
        for section in self.sections:
            regions.append(MemRegion(section.name, section['sh_addr'], section['sh_size']))
        return regions

def GnuLdFileWrite(gnu_ld_file, freeregion):
    """
    GNU ld address file writer writes the free ram location to a GNU linker configuration file
    @param gnu_ld_file GNU Linker config file
    @param freeregion container for ram start address and ram size
    
    """
    #lines = "// python geterated linker configuration file\n"
    #lines += "// it contains the ram start address for data at the end of everything\n"
    lines = "ramstartaddr = 0x%x;\n" % freeregion.address
    lines += "ramsize = 0x%x;\n" % freeregion.size
    f = open(gnu_ld_file,'w')
    f.write(lines)
    f.close
           
class AddressHeaderFile(object):
    """Output address header file, containing the linker address ranges."""
    def __init__(self, filename):
        self.filename = filename
        self.defines = []

    def add_key(self, definition, value, description):
        """
        Add a compiler defintion to the output header file.

        @param definition Compiler defintion
        @param value Value for the compiler defintion
        @param description Description for the compiler defintion
        """
        self.defines.append("/* %s */" % description)
        self.defines.append("#define %s %s" % (definition,value))

    def write_file(self):
        """Write the file"""
        try:
            file = open(self.filename, "w")
        except:
            raise Exception("Could not create file '%s'" % self.filename)

        file_header = r"""/* ============================================================================
** Copyright (C) 2015 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/**
 * @file    __FILE_NAME__
 * @brief   This file contains the RAM patch location and size parameter.
 *          It is generated by usage of
 *          'elf_ram_allocater.py'.
 *
 *        !!! DO NOT MANUALLY UPDATE THIS GENERATED FILE !!!
 *        !!! USE THE GENERATOR TOOL INSTEAD FOR UPDATES !!!
 */
#ifndef __INCLUDE_GUARD__
#define __INCLUDE_GUARD__

"""
        # Update the file name and include guard inside the header
        #  description and write it to the target file
        basename = os.path.basename(self.filename)
        include_guard = basename.replace(".","_").upper()
        header = file_header.replace("__INCLUDE_GUARD__", include_guard)
        header = header.replace("__FILE_NAME__", basename)
        file.write(header)

        # Write the keys and values to the header output file
        file.write("\n".join(self.defines))

        # Write the include guard updated file footer
        file.write("\n\n#endif /* __INCLUDE_GUARD__ */\n".replace("__INCLUDE_GUARD__", include_guard))

        file.close()

################################# main #################################
def main():
    import getopt

    args = None
    header_file = None
    gnu_ld_file = None

    opts, args = getopt.getopt(sys.argv[1:], "a:c:g:hl:v", ["help"])
    for o, a in opts:
        if o in ("-h", "--help"):
            print(USAGE)
            return 0
        elif o in ("-a"):
            header_file=a
        elif o in ("-g"):
            gnu_ld_file=a
        elif o in ("-l"):
            log_file=a
        elif o in ("-c"):
            cfg_file=a
        elif o in ("-v"):
            global verbose
            verbose=True

    if (header_file is None) and (gnu_ld_file is None) :
        raise Exception("Output address header file not specified! See option '-a or -g'")
    if cfg_file is None:
        raise Exception("Configuration file not specified! See option '-c'")

    if len(args) == 0:
        raise Exception("No <elf_files> given!")

    elf_files = args

    # Read in the configuration file
    config = configparser.RawConfigParser()
    config.read(cfg_file)

    # Read all elf files and concatenate them to one single object
    overall_mem_region = MemRegions()
    for i in elf_files:
        print_verbose("Reading '%s' file" % i)
        elf = ELFImage(i)
        overall_mem_region += elf.get_mem_range()
    overall_mem_region.dump()

    # Find the optional 'offset' parameter
    # Use a default value in case this parameter is not given
    # For OPUS patches, the 32 Byte describe the patch header which will be located in RAM just before the patch binary.
    offset_added = 0
    try:
        offset_added = config.getint("Patch", "offset")
    except:
        pass

    # Find an unused region inside the merged regions
    # Extend the memory range search procedure by the additional header offset. The returned range address is also shifted by the additional offset.
    #free_region = overall_mem_region.find_unused_range(0x20032800, 0x20033FFF, 0x400 + offset_added)

    startaddress = int(config.get("Patch", "startaddress"), 16)
    
    endaddress = int(config.get("Patch", "endaddress"), 16)
    minramsize = int(config.get("Patch", "minramsize"), 16)

    free_region = overall_mem_region.find_unused_range(startaddress, endaddress, minramsize + offset_added)
    free_region.address = free_region.address + offset_added

    if header_file is not None:
        # Generate output header file
        out_file = AddressHeaderFile(header_file)
        out_file.add_key("LINKER_RAM_RANGE_START", "0x%08X" % free_region.address, "RAM range start address")
        out_file.add_key("LINKER_RAM_RANGE_SIZE", str(free_region.size), "RAM range end address")
        out_file.add_key("LINKER_RAM_RANGE_END", "0x%08X" % (free_region.address+free_region.size), "RAM range end address")
        out_file.write_file()

    if gnu_ld_file is not None:
        # Generate the gnu linker ile
        GnuLdFileWrite(gnu_ld_file, free_region)


if __name__ == '__main__':
    try:
        main()
        sys.exit(0)
    except Exception, msg:
        txt = "ERROR: "+str(msg)  # that's required to get not-so-dumb result from 2to3 tool
        print(txt)
        print "Usage:"
        print(USAGE)
        sys.exit(2)
