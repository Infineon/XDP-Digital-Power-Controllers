# XDPP1100 Firmware Development Environment

## Instruction 
- Install Python version 2.7.18 in PC
- Install Eclipse IDE version above neon 3
- Install Java Runtime accordingly to requirement of Eclipse IDE
- Edit user_env.bat "PYTHON_INSTALL_DIR","ECLIPSE_DIR" and "JAVAHOME" to the store location of your specific PC
- Example: 
  - set PYTHON_INSTALL_DIR=c:\Python2_7
  - set ECLIPSE_DIR=C:\eclipse\eclipse
  - set JAVAHOME="C:\Program Files (x86)\Common Files\Oracle\Java\javapath"

## Projects Directory

### patch_empty_app
- Clean version of Project for to create new Firmware Patch

### patch_empty_app_partition_2
- Clean version of Project for to create new Firmware Patch store in partition 2

### patch_errata
- Firmware patch for errata.
- To be store in partition 1

### patch_user_app
- Sample project with examples codes  
