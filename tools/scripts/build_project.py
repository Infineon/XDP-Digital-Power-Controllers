#!/usr/bin/python

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
test-patch    Create a new development patch version for the given product.

                 Copies and renames the compile output '*.elf' and 'patch.sym' files
                 to the git repository root directory '/versions'. The new name
                 follows the git version tag format. These files are later used
                 as reference for patches.

                 The script cleans up the local repository (e.g. remove
                 untracked file and local changes). It then call 'git pull' on
                 the product specific 'development' branch, to
                 synchronize to the latest GERRIT HEAD.

                 Parameter:
                 --project=<project-name>    Firmware project name under the
                                             "/projects" sub-directory, where to
                                             generate the version.

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
# version.h file location, related to the GIT root directory
version_h_location = 'projects/version/version.h'
# version.c file location, related to the GIT root directory
version_c_location = 'projects/version/version.c'
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
        print(text)
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
        retvalue = subprocess.check_output(cmd_arg, cwd = cwd, shell = shell, encoding="utf8")
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
    print('ERROR: GIT call inside the repository failed')
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

    Parameter "utc" specified an 32-bit UTC integer timestamp.
    Parameter "proj_name" specified the project name.
    '''

    # try:
    #     git_user = system_except_return_value(['git', 'config', '--get', 'user.name']).strip('\ \t\r\n')
    #     git_email = system_except_return_value(['git', 'config', '--get', 'user.email']).strip('\ \t\r\n')
    # except:
    #     git_email = git_user = ''
    #     print('WARNING: Could not read the "git" repo user parameter. Ensure that you configure the user parameter, otherwise the revision information in GIT do not contain valid user information.')
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
    # if git_root_directory not in release_notes_location:
    #     f = text_file(git_root_directory + '/' + release_notes_location)
    # else:
    #     f = text_file(release_notes_location)
    f = text_file(project_dir + '/' + release_notes_location)

    #write the modified/updated content back to the source code file
    new_content += f.file_read()
    f.file_write(new_content)

def zip_distribution_generate(product, version, utc, proj_name):
    '''
    generate the target ZIP file name out of the project-, version, and timestamp-
    information
    '''
    zip_file_name = "%s.development_v%d_%s.zip" % (product, version, strftime("%Y%m%d_%H%M%S", localtime(utc)))
    zip_list = ["zip", "-r", zip_file_name]
    zip_list += [release_notes_location, version_h_location, version_c_location, 'common', 'projects/%s' % proj_name]
    #add generate "ReleaseNotes.txt" file to the next GIT commit
    system_execute(['git', 'add', release_notes_location])

    system_execute(zip_list)

###################################################################################################
# git repo commands
###################################################################################################

def git_version_tag_get(product, branch, version):
    '''
    returns the GIT tag for a dedicated product, branch and version.
    '''
    for tag in system_execute_return(['git', 'tag']).splitlines(False):
        if re.search('%s\.%s_v%d' % (product, branch, version), tag) != None:
            return tag
    return None


def git_development_version_get(product):
    '''
    Reads the 'git_projects.cfg' configuration file inside the git root directory.
    It returns the current development version number that was placed inside the configuration file.

    Return version #1 in case no configuration file could be found.
    '''
    try:
        tree = ET.parse(configuration_file)
    except:
        print_verbose("Could not read '%s'" % configuration_file)
        return 0

    tree = ET.parse(configuration_file)
    for child in tree.getroot().findall("./distribution[@type='development']"):
        if child.get('product') != product:
            raise GitProjectException( "configuration file '%s' does not contain valid \"<root>/<distribution product=\"%s\">\" attribute." % (configuration_file, product))
        return int(child.get('version'), 10)

    raise GitProjectException( "No valid 'development' version info found in '%s'" %configuration_file)

def git_test_version_get(product):
    '''
    Reads the 'git_projects.cfg' configuration file inside the git root directory.
    It returns the current development version number that was placed inside the configuration file.

    Return version #1 in case no configuration file could be found.
    '''
    try:
        tree = ET.parse(configuration_file)
    except:
        print_verbose("Could not read '%s'" % configuration_file)
        return 0

    tree = ET.parse(configuration_file)
    for child in tree.getroot().findall("./distribution[@type='development']"):
        return int(child.get('version'), 10)

    raise GitProjectException( "No valid 'development' version info found in '%s'" %configuration_file)



def git_development_version_set(product, version):
    '''
    Updates the current development version number in the 'git_projects.cfg' configuration file,
    which is located in the git root directory.
    '''
    try:
        tree = ET.parse(configuration_file)
    except:
        print_verbose("Could not read and update'%s'. Create a new file instance" % configuration_file)
        root = ET.fromstring('<root><distribution product=\"%s\" type=\"development\" version=\"1\"/></root>' % product)
        tree = ET.ElementTree(root)

    for child in tree.findall("./distribution[@type='development']"):
        child.set('version', str(version))
        try:
            tree.write(configuration_file)
        except:
            raise GitProjectException("Could not write '%s'" % configuration_file)
        return


def git_release_version_get(product):
    '''
    retrieves all GIT tags and filters out the correct latest version tag.
    The number of the version found version is returned.

    This routine checks if there is no "release" version for the latest
    development version.

    It returns '0' in case no valid version tag is found.
    '''
    version = git_development_version_get(product)

    tag = git_version_tag_get(product, 'release', version)

    if tag is not None:
        raise GitProjectException('GIT repository contains already release tag "%s" for product "%s", version "%s"' % (tag, product, version))

    if version == 0:
        raise GitProjectException('GIT repository does not contain any "development" to be released')

    return version


def check_if_branch_exist(branch):
    '''Raise an exception if the given branch name does not exist on the repo.'''
    branch_exist = False
    for i in system_execute_return(['git', 'branch', '-a']).splitlines(False):
        if branch in i:
            branch_exist = True
    if branch_exist is False:
        raise GitProjectException('Given branch name "%s" does not exist in the GIT repository' % branch)


def get_current_branch():
    '''Return the current branch name'''
    for line in system_execute_return(['git', 'branch', '-a']).splitlines(False):
        m = re.match(r'\* +([^\ \r\n\t]+)', line)
        if m:
            return m.group(1)
    raise GitProjectException('You currently not attached to any GIT branch')


def check_current_branch(branch):
    '''Check if the current branch is the given branch'''
    check_if_branch_exist(branch)
    current_branch = get_current_branch()

    if branch != current_branch:
        raise GitProjectException('Current checked out branch \"%s\", but for the given product and command you should have checked out branch \"%s\"' % (current_branch, branch))


def get_project_from_branch(branch_name):
    '''
    Extract the project name from the given branch name.
    The branch name convention: "project/<product_name>.development"
    '''
    rx = re.compile('^projects\/|\.development|\_development$')
    return rx.sub('', branch_name)


def git_repo_config():
    '''
    Adapt the local repo configuration for our version management tasks.

    It configures the repo to be file mode agnostic.
    '''
    print_verbose('Adapt GIT repository configuration to ensure that file modes are ignored during the merge')
    system_execute(['git', 'config', '--local', 'core.fileMode', 'false'])

###################################################################################################

###################################################################################################
# Program Commands
###################################################################################################
def command_test_patch_version(args):
    #Evaluate command line arguments
    product = None
    project = None
    make_multi_jobs = False

    zipoption = '0'
    versionoption = '0'

    # opts, args = getopt.getopt(args, 'j', ['project=', 'skip-pull-cleanup'])
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
    ReleaseNote_update_test(utc_time, project)

    #update the 'git_projects.cfg' configuration file
    #git_development_version_set(product, version)

    if versionoption == '0':
        change_filename = "%s/versions/change_%s.diff" % (project_dir, strftime("%Y%m%d_%H%M%S", localtime(utc_time))) 
    elif versionoption == '1':
        change_filename = "%s/versions/change_%s_0x%08X.diff" % (project_dir, strftime("%Y%m%d_%H%M%S", localtime(utc_time)),utc_time) 

    # Update the UTC timestamp of the new version in the partial patch configuration file
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

    system_execute(['git', 'diff', '>',change_filename])

    #clean the target image
    print_verbose('Compile target project')
    make_cmd = ['make', 'clean', '-C', project_dir]
    system_execute(make_cmd)

    #compile the target image
    print_verbose('Compile target project')
    make_cmd = ['make', '-C', project_dir, 'patch']
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
        target_excel = "%s/versions/shasta_pmbus_%s_0x%08X.xlsx" % (project_dir, strftime("%Y%m%d_%H%M%S", localtime(utc_time)),utc_time)
        # target_excel = "%s/versions/shasta_pmbus_0x%08X.xlsx" % (project_dir, utc_time)
    shutil.copy(excel_files[0], target_excel)  

    bin_prefix = "%s_%s" % (project, strftime("%Y%m%d_%H%M%S", localtime(utc_time)))
    bin_prefix_2 = "%s_%s_0x%08X" % (project, strftime("%Y%m%d_%H%M%S", localtime(utc_time)), utc_time)
    # bin_prefix_2 = "%s_0x%08X" % (project, utc_time)
    bin_files = glob.glob(os.path.join("%s/*/*" % project_dir, "*.bin"))
    if len(bin_files) > 1:
        raise GitProjectException ("Multiple 'bin' files are found in project '%s': %s" %(project_dir, ",".join(bin_files)))
    # project source storage location
    if versionoption == '0':
        target_file = "%s/%s.bin" % (bin_path, bin_prefix)
    elif versionoption == '1':
        target_file = "%s/%s.bin" % (bin_path, bin_prefix_2)
    shutil.copy(bin_files[0], target_file) 

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
    except getopt.GetoptError as msg:
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
###################################################################################################
