USAGE = '''
Usage:
   python git_projects.py [command] [command options]

This script supports to generate and manage new
customer product (project) in GIT.
It generates the corresponding product branches and helps to tag and merge
firmware releases (e.g. development version, released version).

Note: This script is changing the current working branch while the script runs.
      Ensure that you do not have uncommitted work inside your GIT clone!

Commands:
test-patch      Compile patch version without commit to repo.
                 Parameter:
                 --project=<project-name>    Firmware project name under the
                                             "/projects" sub-directory, where to
                                             generate the version.\
                 --versionoption=<0 or 1>    "0" append localtime version number to bin file
                                             "1" append utc version number to bin file
                 --zipoption=<0 or 1>        "1" zip the src folder.

Options:

-h, --help             Print this help message and exit.

-j                     Allow multiple "make" jobs at once.

--verbose              Print out a lot of debug information

'''

import os
import re
import subprocess
import getopt
import sys
import xml.etree.ElementTree as ET
from time import time, gmtime, localtime, strftime

def AddRelativeLibPath(path):
    """Add Python library include path"""
    current_dir = os.path.normpath(__file__+"/../")
    lib_dir = os.path.normpath(current_dir+"/"+path)
    sys.path.insert(0, lib_dir)

AddRelativeLibPath("lib")

import configfile

#retrieve the current system UTC time
utc_time = int(time())

###################################################################################################
#Global Definitions
###################################################################################################
# ReleaseNotes.txt file location, related to the GIT root directory
release_notes_location = 'ReleaseNotes.txt'

###################################################################################################

###################################################################################################
# Printout Environment
###################################################################################################
#verbose printout of debug information
b_verbose_printout = False

def print_verbose(text):
    '''
    Printout debug information in case verbose printout is enabled as command line option
    '''
    if b_verbose_printout is True:
        print (text)
###################################################################################################

class GitProjectException(Exception):
    '''Exception class for this script'''
    pass

###################################################################################################
# System Call Support Routine Library
###################################################################################################
#Windows has some problems to launch a shell command in case
#the 'subprocess.check_output' and 'subprocess.call' are called with 'shell=False'. This
#variable must be set to False for CYGWIN and LINUX.
if 'posix' in os.name:
    shell = False
else:
    shell = True

def system_execute_return(cmd_arg, cwd = None):
    '''
    Performs a system call and checks if the return value is a valid
    string (length unequal zero).

    Set the 'cwd' parameter in order to change into a dedicated working
    directory before executed the command with arguments 'cmd_arg'.

    This function raises an OSError exception in case the execution
    failed or the function did not perform any string output.
    '''
    retvalue = None

    try:
        print_verbose('System Command Execute: "%s"' % ' '.join(cmd_arg))
        retvalue = subprocess.check_output(cmd_arg, cwd = cwd, shell = shell)
    except:
        raise OSError('Command Execution failed: "%s"' % ' '.join(cmd_arg))
    else:
        if retvalue == '':
            raise OSError('Command returned empty string: "%s"' % ' '.join(cmd_arg))
        return retvalue

def system_execute(cmd_arg, cwd = None):
    '''
    Performs a system call.

    Set the 'cwd' parameter in order to change into a dedicated working
    directory before executed the command with arguments 'cmd_arg'.

    This function raises an OSError exception in case the execution
    failed.
    '''
    try:
        print_verbose('System Command Execute: "%s"' % ' '.join(cmd_arg))
        if 0 != subprocess.call(cmd_arg, cwd = cwd, shell = shell):
            raise OSError('Command returned non-empty string: "%s"' % ' '.join(cmd_arg))
    except:
        raise OSError('Command Execution failed: "%s"' % ' '.join(cmd_arg))

###################################################################################################

#git root directory
try:
    git_root_directory = system_execute_return(['git', 'rev-parse', '--show-toplevel']).strip('\ \t\r\n')
except:
    print 'ERROR: GIT call inside the repository failed'
    exit(2)

#specify the configuration file
configuration_file = git_root_directory + '/git_projects.cfg'
###################################################################################################
# Local Class Declarations
###################################################################################################
class text_file:
    '''
    Class reading and writing text files. The text content of the file
    can be retrieved and modified over string array accesses.
    '''
    filename = None

    def __init__(self, filename = ''):
        self.filename = filename


    def file_read(self):
        '''Reads the given file and return a file content string list'''
        #read the current content source code file
        print_verbose('Read file: "%s"' % self.filename)
        f = open(self.filename, 'r')
        content = f.readlines()
        f.close()
        return content


    def file_write(self, new_content):
        '''
        Write the given content to the file.
        The given content is a string list.
        '''
        #write the modified/updated content back to the source code file
        print_verbose('Write file: "\%s"' % self.filename)
        f = open(self.filename, 'w+')
        print_verbose('Write content')
        print_verbose(new_content)
        f.writelines(new_content)
        f.close()

def ReleaseNote_update_test(utc, proj_name):
    '''
    Updates and adds a new header at the top of the release note file.
    This file is located in the GIT root directory.

    Parameter "version" specified the version number.
    Parameter "utc" specified an 32-bit UTC integer timestamp.
    Parameter "proj_name" specified the project name.
    '''

    # try:
    #     git_user = system_except_return_value(['git', 'config', '--get', 'user.name']).strip('\ \t\r\n')
    #     git_email = system_except_return_value(['git', 'config', '--get', 'user.email']).strip('\ \t\r\n')
    # except:
    #     git_email = git_user = ''
    #     print 'WARNING: Could not read the "git" repo user parameter. Ensure that you configure the user parameter, otherwise the revision information in GIT do not contain valid user information.'
    #     pass

    try:
        domain = os.environ['USERDOMAIN']
        username = os.environ['USERNAME']
    except:
        domain = None
        username = None
        pass

    new_content = []
    new_content.append('===============================================================================\n')
    new_content.append('%s ' % (strftime('%Y-%m-%d %H:%M:%S', localtime(utc))))
    new_content.append('Firmware Details:\n')
    new_content.append('       Version UTC Tag:    0x%08X\n' % utc)
    new_content.append('       Firmware Project:   "%s"\n' % proj_name)

    if domain != None and username != None:
        new_content.append('User:\n')

    if domain != None and username != None:
        new_content.append('       WIN Login:          "%s\\%s"\n' % (domain, username))

    new_content.append('===============================================================================\n')

    project_dir = '%s/projects/%s' % (git_root_directory, proj_name)

    path = project_dir + '/' + release_notes_location
    isExist = os.path.exists(path)
    if not isExist:
        open(path,'w')

    #read the current content of the version information source code file
    f = text_file(project_dir + '/' + release_notes_location)

    #write the modified/updated content back to the source code file
    new_content += f.file_read()
    f.file_write(new_content)

def zip_test_generate(zip_path, ver_name, proj_name):
    '''
    generate the target ZIP file name out of the project-, version, and timestamp-
    information
    '''
    zip_file_name = "%s/%s.zip" % (zip_path,ver_name)
    zip_list = ["zip", "-r", zip_file_name]
    zip_list += ['projects/%s/src' % proj_name]
    system_execute(zip_list)

def command_test_patch_version(args):
    #Evaluate command line arguments
    product = None
    project = None
    make_multi_jobs = False

    zipoption = '0'
    versionoption = '0'
    opts, args = getopt.getopt(args, 'j', ['project=', 'skip-pull-cleanup','versionoption=','zipoption=' ])
    for o, a in opts:
        if o in ('-j'):
            make_multi_jobs = True
        elif o in ('--project'):
            project = a
        elif o in ('--versionoption'):
            versionoption = a
        elif o in ('--zipoption'):
            zipoption = a

    if project is None:
        raise getopt.GetoptError('Command "version" requires "--project" parameter')
    
    project_dir = '%s/projects/%s' % (git_root_directory, project)

    # create versions folder in source project
    bin_path = "%s/versions" %project_dir
    isExist = os.path.exists(bin_path)
    if not isExist:
        os.makedirs(bin_path)
        
    #merging from 'development' to 'testing' branch requires:
    # - Go to source branch
    # - Update version information in "ReleaseNotes.txt"
    # - Update the 'git_projects.cfg' configuration file
    # - Update the UTC timestamp of the new version in the partial patch configuration file
    # - Compile the target image
    # - Copy the generated "*.sym" and "*.elf" to the "/versions" directory below the GIT root directory.
    #   Rename the target file to the GIT version tag
    # - Generate ZIP distribution package
    # - Commit the updated "ReleaseNotes.txt", "version.c" & "git_projects.cfg" in the 'development' branch
    # - Merge 'development' branch into the 'testing' branch
    # - Commit the merge and generate the version tag

    #update version information in "ReleaseNotes.txt"
    #version = git_development_version_get(product) + 1
    ReleaseNote_update_test(utc_time, project)

    #update the 'git_projects.cfg' configuration file
    #git_development_version_set(product, version)

    # Update the UTC timestamp of the new version in the partial patch configuration file
    if versionoption == '0':
        change_filename = "%s/versions/change_%s.diff" % (project_dir, strftime("%Y%m%d_%H%M%S", localtime(utc_time))) 
    elif versionoption == '1':
        change_filename = "%s/versions/change_0x%08X.diff" % (project_dir, utc_time) 

    config_filename = "%s/src/partial_patch.cfg" % project_dir
    config_file=configfile.ConfigFile(config_filename)

    with open(config_filename, "r") as file:
        # Read the file contents into a string
        text = file.read()
        # Find the index of the substring
        substring = "newversion = "
        index = text.find(substring)
        if index != -1:
            patchversion = text[index + len(substring):]
            patchversion = patchversion.replace("newversion = ", "")
            patchversion = patchversion.strip()
        else:
            print("String not found in file.")

    
    config_file.set("Version", "NewVersion", "0x%08X" % utc_time)
    config_file.write_file()
    system_execute(['git', 'diff', '>',change_filename])
    
    # update patch_init.c with the latest utc version number
    patch_init_filename = "%s/src/patch_init.c" % project_dir
    search_version = "SCU_SPARE_FF__SET(" + patchversion +"UL);"
    replace_version = "SCU_SPARE_FF__SET(0x%08XUL); " % utc_time
    
    # Opening our patch_init.c file in read only
    # mode using the open() function
    with open(patch_init_filename, 'r') as file:
      
        # Reading the content of the file
        # using the read() function and storing
        # them in a new variable
        data = file.read()
        # Searching and replacing the text
        # using the replace() function
        data = data.replace(search_version, replace_version)
      
    # Opening our atch_init.c in write only
    # mode to write the replaced content
    with open(patch_init_filename, 'w') as file:
      
        # Writing the replaced data in our
        # text file
        file.write(data)  

    #clean the target image
    print_verbose('Compile target project')
    make_cmd = ['make', 'clean', '-C', project_dir]
    system_execute(make_cmd)

    #compile the target image
    print_verbose('Compile target project')
    #make_cmd = ['make', 'patch', 'TOOLCHAIN=ARM_GCC', '-C', project_dir]
    make_cmd = ['make', '-C', project_dir]
    if make_multi_jobs is True:
        make_cmd.append('-j')
    system_execute(make_cmd)

    # Copy the generated "patch*.sym" and "*.elf" to the "/versions" directory below the GIT root directory.
    # Rename the target file to the GIT version tag
    import shutil, glob
    
    excel_files = glob.glob(os.path.join("%s/src" % project_dir, "*.xlsx"))
    if len(excel_files) > 1:
        raise GitProjectException ("Multiple 'excel' files are found in project '%s': %s" %(project_dir, ",".join(excel_files)))
    if versionoption == '0':
        target_excel = "%s/versions/shasta_pmbus_%s.xlsx" % (project_dir, strftime("%Y%m%d_%H%M%S", localtime(utc_time))) 
    elif versionoption == '1':
        target_excel = "%s/versions/shasta_pmbus_0x%08X.xlsx" % (project_dir, utc_time)
    shutil.copy(excel_files[0], target_excel) 
    
    bin_prefix = "%s_%s" % (project, strftime("%Y%m%d_%H%M%S", localtime(utc_time)))
    bin_prefix_2 = "%s_0x%08X" % (project, utc_time)
    bin_files = glob.glob(os.path.join("%s/*/*" % project_dir, "*.bin"))
    if len(bin_files) > 1:
        raise GitProjectException ("Multiple 'bin' files are found in project '%s': %s" %(project_dir, ",".join(bin_files)))
    # project source storage location
    if versionoption == '0':
        target_file = "%s/%s.bin" % (bin_path, bin_prefix)
    elif versionoption == '1':
        target_file = "%s/%s.bin" % (bin_path, bin_prefix_2)
    shutil.copy(bin_files[0], target_file) 
        
    if zipoption == '1':
        #generate ZIP package
        zip_test_generate(bin_path, bin_prefix, project)

###################################################################################################

###################################################################################################
# Main Routine
###################################################################################################
def main():

    #Evaluate command line arguments
    try:
        if len(sys.argv) > 1:
            args = sys.argv[1:]
            cmd_args = []
            for o in args:
                if o in ('--verbose'):
                    #enable verbose printout of debug information
                    global b_verbose_printout
                    b_verbose_printout = True
                else:
                    cmd_args.append(o)

            command = cmd_args[0]
            if command == 'test-patch':
                return command_test_patch_version(cmd_args[1:])
    except GitProjectException as msg:
        print('ERROR: '+str(msg))
    except getopt.GetoptError, msg:
        print('ERROR: '+str(msg))
    except OSError as msg:
        print('ERROR: '+str(msg))
    else:
        print(USAGE)
        return 0

    print(USAGE)
    return 2

# Make this file executable as a script.
if __name__ == '__main__':
    sys.exit(main())