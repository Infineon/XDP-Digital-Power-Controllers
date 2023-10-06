/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
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
 * @file    configurator_doxy.h
 * @brief   Doxygen overview of the configurator.
 */


#ifndef CONFIGURATOR_DOXY_H_
#define CONFIGURATOR_DOXY_H_

/** @page pgShasta_Configurator   Configurator FW Module

@section sctShasta_Configurator_Intro  Introduction

The configurator is a module that manages all configuration which are stored/loaded to/from OTP.
Following files are available in OTP:
- Trim Structure            - Trim Register information (address, number of bits)
- Trim Data                 - Trim Data (default register setting, also contains FW default configuration) defined by ATE
- Configuration Structure   - Configuration registers information (address, number of bits)
- Configuration Data        - Configuration data which can be changed by the user, also contains FW configuration
- PMBus Structure           - PMBus command information
- PMBus Default             - PMBus default data for loop 0 and loop 1
- PMBus User                - PMBus user data for loop 0 and loop 1
- Partial Register          - Partial Configuration register data
- Partial PMBus             - Single PMBus Command Data

@anchor Image_Configurator
@image html configurator_icon.svg
@image latex configurator_icon.png

- Configurator files are loaded automatically during boot.
- Structure files are directly compiled into ROM in order to save OTP space.
- Data files can be stored and restored via PMBus commands, see pmbus_api.h.

@section sctShasta_Configurator_xvalent  Xvalent File Selector

Configurator files can be selected via a @c xvalent pin, its value ranges from 0 to 15. <br>
There are three categories of xvalent controlled files

| Category         |    Definition                   |
|------------------|---------------------------------|
| xv_full          | Configuration Data\n PMBus User |
| xv_partial       | Partial Register Configuration  |
| xv_partial_pmbus | Partial PMBus Configuration     |

If there are less files in a given category than the actual xvalent setting proposes, then the actual xvalent
setting is modulo divided by the number of available files.
Here is an example. Let's assume that the actual xvalent is @c 13. <br>
The table below depicts which file will be chosen in each categore depending on the number of store files.

| Category         | Files stored in OTP | Selected File Number |
|------------------| :-----------------: |:-------------------: |
| xv_full          | 3                   | 1                    |
| xv_partial       | 8                   | 5                    |
| xv_partial_pmbus | 16                  | 13                   |

@note @c xv_full represents @b two file categories by itself. The number of files is determined by the number *Configuration Data* files.
- Configuration Data
- PMBus User

Below is an example
| Category           | Files stored in OTP | Selected File Number |
|--------------------| :-----------------: | :------------------: |
| Configuration Data | 9                   | 4                    |
| PMBus User         | 8                   | @b error             |

In the @b error case *PMBUS User* is not loaded and the *PMBUS Default* configuration will be applied instead.

@section sctShasta_Configurator_Register_Structure  Register Structure File

Register information is based on
<a href="../../../../../tools/scripts/otp_file_gen/shasta_revB_register_map.xlsx">Register Map</a>

The structural data format describes the structure of how configuration data are stored in the OTP.
It is also stored as a data file in OTP besides the data itself.
The structural data format is represented by a sequence of uint8_t numbers as described below.

| Term     |    Definition                                                              |
|----------|----------------------------------------------------------------------------|
| Block          | A block describes the registers of a functional block (e.g. analog, pwm)   |
| Instance       | A block contains registers and it can be instantiated several times.<br>The address of instance is reflected by a base address (e.g. 0x7000_0000 <br>is the base address for the analog module, or 0x7000_0100 is the base<br>address of vsen0, or 0x7000_0200 is the base address of vsen1,the second <br>instance of vsen)|
| Register       | Each register has an offset address which adds to the base address of <br> the instantiation. (e.g VC_VRAMP1 has the address 0x7000_0404 because it <br>is in block vcontrol0 which has a base address of 0x7000_0400 and an <br>offset address of 0x04.|
| Subfield       | Each 32-bit wide register contains subfields which are defined by start position and length |
| start position | Subfield starting bit position inside the register |
| length         | Subfield length in bits |

See figure below:

@image html data_structure_format.png
@image latex data_structure_format.png

@section sctShasta_Configurator_Register_FW_Config FW Configuration

FW configuration data are appended to the end of the trim and config data files.<br>
The structure is defined in @ref FW_TRIM_t for trim and @ref FW_CONFIG_t for configuration. <br>

FW configurations data are defined in <br>
https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/00_Requirements/FWConfig/Shasta_FW_Config.xlsx

The tables must start with following entries at the beginning:

 * | Field Name |  size      |         Comment                   |
 * | :-------:  | :------- : | ----------------------------------|
 * |   size     |  uint16_t  | size of fw configuration in bytes |
 * |   version  |  uint16_t  | version number                    |
 *
 *
@subsection sctShasta_Configurator_Register_FW_Config_Trim  FW Trim Data

FW Trim data are located at RAM address *0x2005C508* for @b Shasta.A11.ROM.v4_20180124_124011_rom_image and contain following data:

 * | Field Name |  size      | Offset | Default |          Comment                   |
 * | :-------:  | :------- : | :----: | :-----: |  ----------------------------------|
 * |   size     |  uint16_t  |    0   |   42    |  size of fw configuration in bytes |
 * |   version  |  uint16_t  |    2   |    2    |  version number                    |
 * |   bootdesc |  uint32_t  |    4   |    3    |  boot descriptor                   |
 * |   datas    |  uint16_t  |    8   |  16384  |  size of data partition            |
 * |   patchs1  |  uint16_t  |   10   |  16384  |  size of patch partition 1         |
 * |   patchs2  |  uint16_t  |   12   |  16384  |  size of patch partition 2         |
 * |   patchs3  |  uint16_t  |   14   |  16384  |  size of patch partition 3         |
 * |   patchs4  |  uint16_t  |   16   |    0    |  size of patch partition 4         |
 * |   patchs5  |  uint16_t  |   18   |    0    |  size of patch partition 5         |
 * |   patchs6  |  uint16_t  |   20   |    0    |  size of patch partition 6         |
 * |   patchs7  |  uint16_t  |   22   |    0    |  size of patch partition 7         |
 * |   patchs8  |  uint16_t  |   24   |    0    |  size of patch partition 8         |
 * |   patchs9  |  uint16_t  |   26   |    0    |  size of patch partition 9         |
 * |   patchs10 |  uint16_t  |   28   |    0    |  size of patch partition 10        |
 * |   patchs11 |  uint16_t  |   30   |    0    |  size of patch partition 11        |
 * |   patchs12 |  uint16_t  |   32   |    0    |  size of patch partition 12        |
 * |   patchs13 |  uint16_t  |   34   |    0    |  size of patch partition 13        |
 * |   patchs14 |  uint16_t  |   36   |    0    |  size of patch partition 14        |
 * |   patchs15 |  uint16_t  |   38   |    0    |  size of patch partition 15        |
 * |   patchs16 |  uint16_t  |   40   |    0    |  size of patch partition 16        |
 *

@subsection sctShasta_Configurator_Register_FW_Config_Config  FW Config Data

FW Configuration data are located at RAM address *0x2005C500* for @b Shasta.A11.ROM.v4_20180124_124011_rom_image and contain following data:

 * | Field Name                    |  size      | Offset | Default |          Comment                   |
 * | :---------------------------: | :------- : | :----: | :-----: |  ----------------------------------|
 * | size                          |  uint16_t  |    0   |    8    |  size of fw configuration in bytes |
 * | version                       |  uint16_t  |    2   |    1    |  version number                    |
 * | xv_en                         |  uint8_t   |    4   |    1    |  enables xvalent selection\n @c 0 = xvalent disabled\n @c 1 = xvalent enabled |
 * | addr_pin_xv                   |  uint8_t   |    5   |    0    |  configurator xvalent pin selection\n  @c 0 = xaddr1\n @c 1 = xaddr2\n |
 * | addr_pin_pmbus_address_offset |  uint8_t   |    6   |    0    |  pmbus address offset xvalent pin selection\n  @c 0 = xaddr1\n @c 1 = xaddr2\n |
 * | addr_pin_i2c_address_offset   |  uint8_t   |    7   |    0    |  i2c address offset xvalent pin selection\n  @c 0 = xaddr1\n @c 1 = xaddr2\n |

@section sctShasta_Configurator_PMBUS_Structure  PMBUS Structure file

PMBUS configuration is based on <br>
https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/00_Requirements/PMBus/Shasta_PMBus.xlsx

The PMBus structure is described as follows:

- i0 The PMBus number of instances @c (numi = 1)
- repeated for the number of commands
    - r0  The command (8 Bits, 0 - 255)
    - r1  is command enabled (1 Bit, 0 - 1)
    - r2  number of bits (9 Bits, 1 - 256)

@section sctShasta_Configurator_partial_register_configurations   Partial Register Configurations

In order to save OTP space configuration register data can also be stored partially in otp. Instead of storing
the whole register space data can be stored as address/data pairs. <br>
These partial files are loaded successively at startup into the registers. <br>

For the file format please see @ref sctShasta_otp_file_system_configuration_file_format.

@section sctShasta_Configurator_partial_pmbus_configurations   Partial PMBus Configurations

Configurations for single PMBus commands can be stored in partial PMBus configuration. Theses files use a different
format as outlined below. <br>
The header contains a field for the loop and a field for the respective PMBus command.


 * | Field Name |  size     |         Comment                                                        |
 * | :-------:  | :-------: | :--------------------------------------------------------------------: |
 * |   cmd      |  uint8_t  | file identifier (see @ref Otp_File_System_Cmd_Types_t)                 |
 * |   xvalent  |  uint8_t  | identifies the xvalent selector                                        |
 * |   pcmd     |  uint8_t  | pmbus command field (0 - 255)                                          |
 * |   lp       |  uint8_t  | loop field (0 or 1)                                                    |
 * |   len      |  uint16_t | length of the file in bytes                                            |
 * |   len_red  |  uint16_t | redundant length field for safety                                      |
 * |   crc      |  uint32_t | crc for header protection                                              |
 * |   data     |           | arbitrary data                                                         |
 * |   crc      |  uint32_t | crc for data protection                                                |

@section sctShasta_Configurator_otp_file_generation   OTP File Generation

Configuration data defined in the excel files referenced in previous chapters need to be converted
into different formats.

 * | File type  |         Use Case                   |
 * | :-------:  | -----------------------------------|
 * |  *.hex     | RTL simulation                     |
 * |  *.bin     | Ceedling and ATE processing        |
 * |  *.txt     | FPGA verification                  |
 *
 * The files are created for each topology, e.g. config_otp_ACF_P5_3P3V_30A files are related to the
 * topology ACF_P5_3P3V_30A.
 *
 * There are special ceedling files which support the module testing of PMBUS and register configuration content.
 *
 * | File                  |         Content          |
 * | --------------------- | ----------------------   |
 * |  ceedling_pmbus_*.c   | PMBUS configuration      |
 * |  register_dump_*.c    | Register configuration   |

 * Following files are created for debugging.
 *
 * | File                  |         Content          |
 * | --------------------- | ----------------------   |
 * |  log_*.txt            | Useful debug information |
 * |  config_otp_map.txt   | Map of the OTP           |
 *

The  @b xvalent files contain a merge of all configurations with each one having a specific xvalent id assigned.

The @b _mfr configurations require the @c patch_user_app to be loaded otherwise operation will fail
due to a mismatch in the PMBUS configuration.

The figure below depicts the conversion process.

@image html shasta_otp_conf_gen.png "Conversion Process"
@image latex shasta_otp_conf_gen.png "Conversion Process"

@section sctShasta_Configurator_Error_Handling   Error Handling

As the configuration parameters define somehow the behavior of the firmware and the device, special care should be taken in the
following scenarios:
- During initialization: if no valid OTP files are found in OTP, FW parameters and registers get default values, PMBus parameters
  are zero.
- On store operation the original files in OTP get invalidated before the new one is stored. Thus it is guaranteed that only one
  valid file of each type is in OTP.

In any case, an invalid crc (for the otp command header or the data itself) will generate an error message.
The load or store function in an error event is terminated thus leaving the device with default values in
case of a read.

More information can be found in configurator.h<br>
Some useful information regarding file system management can be found in otp_fs.h
*/

#endif /* CONFIGURATOR_DOXY_H_ */
