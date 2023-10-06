#!/usr/bin/python

USAGE = '''
This tool generates ARM/Keil linker symbol- and steering- files.
These files are given to the linker to filter out global symbols from
compiler object files or add external symbols to the target ELF
linking process.

The input symbol files filtering is based on a given configuration file.

This configuration file contains symbol names or REGEX expressions,
where one rule or expression is given per line. Lines starting
with ";" are treated as comment and get ignored.

The output files contain only filtered symbols, based on configuration
file filter rules.
The output symbol file contains the filtered symbols, while and the
output sterring file contains "HIDE" rule for the same symbols.

Usage:
   python linker_sim_filter.py [options] <ref_sym_file>

Options:
   -h, --help              Print this help message and exit.

   -m <sym_file>           Output linker symbol file ("*.sym")

   -s <sterring_file>      Output partial linking ARM steering file.
                           This file will contain the "HIDE" rule
                           for the filtered out symbols.

   -c <config_file>        Configuration file

   -v                      Verbose printout

Arguments:
    <ref_sym_file>         One or multiple Input linker symbol files

'''

import re

verbose = False

def print_verbose(x):
    '''
    Print out the given string in case the verbose flag is enabled.
    '''
    if verbose is True:
        print "linker_sim_filter.py: %s" % x

def symFileRead(symInFiles):
    '''This function reads the input symbol files. It returns a string array with all symbol lines.'''
    outputLines =[]

    for symInFile in symInFiles:
        f = open(symInFile, "r")
        symFileList = f.read().splitlines()
        f.close()

        # Search for the symbol in the file and retrieve the corresponding
        # entry point
        #
        # Example: 0x20008001 T patch_init
        for line in symFileList:
            print_verbose("Input file '%s' symbol: '%s'" % (symInFile, line))
            m = re.search('^(0x[0-9A-Fa-f]+) [TDN] ([_0-9a-zA-Z]+)', line) #todo: replace by match
            if m:
                outputLines.append(line)
    return outputLines

def configFileRead(configFile):
    '''Reads the config file and returns an string array'''
    print_verbose("Read configuration file '%s'" % configFile)
    f = open(configFile, "r")
    # Remove EOL termination
    ruleCommentList = f.read().splitlines()
    f.close()

    # Remove lines which are marked as comment ";"
    ruleList = []
    for rule in ruleCommentList:
        if re.match(";.*", rule):
            continue
        ruleList.append(rule)

    return ruleList

def symFileParse(ruleList, symList):
    '''This function scans the symbols for the given REGEX rules. The output symbols are returned.'''

    # Prepare output file content arrays
    outputLines =[]

    # Search for the symbol in the file and retrieve the corresponding
    # entry point
    #
    # Example: 0x20008001 T patch_init
    for line in symList:
        print_verbose("Input file '%s' symbol: '%s'" % (symList, line))
        m = re.match(r'(\w+) (\w+) (\w+)', line)
        if m:
            for rule in ruleList:
                print_verbose("Scan rule: \"%s\"" % rule)
                #search for each rule/symbol
                m2 = re.search(rule, m.group(3))
                if m2:
                    print_verbose("Symbol added: %s" % line)
                    #symbol found according to the given rule
                    outputLines.append(line)
                    break

    return outputLines

def symFileWrite(fileName, symList):
    '''Write the given list of the symbols to a Keil/ARM symbol file'''
    print_verbose("Write output symbol file '%s'" % fileName)
    f = open(fileName, "w")
    # Write symbol file mandatory header
    f.write("#<SYMDEFS># 'linker_sim_filter.py' generated symbol filter output file\n")
    outputLines = list(set(symList)) #Remove duplicate entries
    f.writelines("\n".join(outputLines))
    f.close()

def symlFileWrite(fileName, symList):
    '''Write the given list of the symbols to an AMR GNU symbol file'''
    print_verbose("Write output symbol file '%s'" % fileName)
    f = open(fileName, "w")
    # Write symbol file mandatory header
    # f.write("#<SYMDEFS># 'linker_sim_filter.py' generated symbol filter output file\n")
    outputLines = list(set(symList)) #Remove duplicate entries
    r = []
    for l in outputLines:
        x = l.split()
        r += [x[2] + " = " + x[0] +";"]
    f.writelines("\n".join(r))
    f.close()
    
def sterringFileWrite(fileName, symList):
    '''Write the given list of the symbols to a Keil/ARM steering file. All symbols are masked with "HIDE" rules.'''
    print_verbose("Write output steering file '%s'" % fileName)
    f = open(fileName, "w")

    # Search for the symbol in the file and retrieve the corresponding
    # entry point
    #
    # Example: 0x20008001 T patch_init
    for line in symList:
        print_verbose("Input file '%s' symbol: '%s'" % (symList, line))
        m = re.match(r'(\w+) (\w+) (\w+)', line)
        if m:
            f.write("HIDE %s\n" % m.group(3))
    f.close()

def main():
    import getopt

    symOutFile = None
    symlOutFile = None
    steeringFile = None
    configFile = None

    opts, args = getopt.getopt(sys.argv[1:], "hg:m:c:s:v", ["--help"])
    for o, a in opts:
        if o in ("-h", "--help"):
            print(USAGE)
            return 0
        elif o in ("-g"):
            symlOutFile=a
        elif o in ("-m"):
            symOutFile=a
        elif o in ("-s"):
            steeringFile=a
        elif o in ("-c"):
            configFile=a
        elif o in ("-v"):
            global verbose
            verbose = True

    if configFile is None:
        raise getopt.GetoptError('Configuration file not given (option: \'-c\')')

    if symlOutFile is None and symOutFile is None and steeringFile is None:
        raise getopt.GetoptError('Neither output symbol nor steering file are given (option: \'-m\', \'-s\')')

    if len(args) == 0:
        raise getopt.GetoptError('No input symbol file given')

    symList = symFileParse(configFileRead(configFile), symFileRead(args))

    if symOutFile is not None:
        symFileWrite(symOutFile, symList)

    if symlOutFile is not None:
        symlFileWrite(symlOutFile, symList)

    if steeringFile is not None:
        sterringFileWrite(steeringFile, symList)

    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main())
