#!/usr/bin/python

USAGE = '''
sym_add_static_symbols.py: Reads linker.log file, extracts a local symbol table and appends it to the symbol file. 


Usage:
  sym_add_static_symbols.py [options]

Options:
   -h, --help            Print this help message and exit.

   -i <logfile>          Linker fog file from linker

   -o <symfile>          Output symbolfile

'''

import sys
import os
import time

def symbols_store(symbolStr, file):
    """
    This function updates the sym file with static symbols

    :param symbolList: List of static symbol entries
    :param file: symbolfile with coplete path
    :return:
    """
    # Read symbol File
    with open(file, 'r') as content_pFile:
        pContentstr = content_pFile.read()

    # Append statoc symbols
    pContentstr += symbolStr
    
    # Write File
    with open(file, 'r') as content_pFile:
        content_pFile.write(pContentstr)

def ReadSymbolsFromLinkerLog(linkerfile):
    """
    This function reads the static symbols from the linker log file

    :param linkerfile: linkler log file with complete path
    :return string: String with static symbols
    """

    with open(linkerfile, "r") as infile:
        lines = infile.readlines()

    # Function has a T symbol 
    SymbolList = []        
    is_locals = False
    
    for l in lines:

        if 'Local Symbols' in l:
            is_locals = True

        if 'Global Symbols' in l:
            is_locals = False
            
        if is_locals:
            SymbolString = ''

            words = l.split()
            
            
            if len(words) >= 4 and 'Code' in words[3]:
                #symbol_address = "{0:#0{1}x}".format(int(words[1][2:], 16) - 1, 10)
                symbol_address = words[1]
                SymbolString = '%s T %s_%s' % (symbol_address, words[0], words[-1].split('.o')[0])

            if len(words) >= 4 and 'Data' in words[2]:
                SymbolString = '%s D %s_%s' % (words[1], words[0], words[-1].split('.o')[0])

            if SymbolString is not '':
                #print "Symbol %s" % SymbolString
                SymbolList += [SymbolString]
    
    return '\n'.join(SymbolList) + '\n'


def SymbolFileUpdate(linkerfile, symbolfile):
    """
    This function updates the sym file with static symbols
    
    :param linkerfile: linkler log file with complete path
    :param symbolfile: symbol file with complete path
     :return:
    """
    
    symbolStr = ReadSymbolsFromLinkerLog(linkerfile)

    with open(symbolfile, 'r') as symbol_pFile:
        pContentstr = symbol_pFile.read()

    # Append statoc symbols
    pContentstr += '# <STATIC symbols> by python script. Last Updated: %s\n' % time.strftime("%c")
    pContentstr += symbolStr
    
    # Write File
    with open(symbolfile, 'w') as content_pFile:
        content_pFile.write(pContentstr)
    

def main():
    import getopt
        
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hi:o:",
            ["help"])
        for o, a in opts:
            if o in ("-h", "--help"):
                print(USAGE)
                return 0
            elif o in ("-i"):
                inputfile=a
            elif o in ("-o"):
                outputfile=a

        if inputfile is None:
            raise getopt.GetoptError('Input hex file \'-i <hexfile>\' not specified')
        if outputfile is None:
            raise getopt.GetoptError('Ouput hex file \'-o <hexfile>\' not specified')

    except getopt.GetoptError, msg:
        txt = 'ERROR: '+str(msg)  # that's required to get not-so-dumb result from 2to3 tool
        print(txt)
        print(USAGE)
        return 2

    try:
        SymbolFileUpdate(inputfile, outputfile)
    except IOError as e:
        sys.stderr.write("Error reading file: %s\n" % e)
        return 1

if __name__ == '__main__':
    sys.exit(main())
