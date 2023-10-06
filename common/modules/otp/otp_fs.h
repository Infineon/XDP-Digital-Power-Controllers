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
 * @file    otp_fs.h
 * @brief   OTP file system layer.
 */
#ifndef OTP_FS_H
#define OTP_FS_H

/* Data types */
#include <stdint.h>     /*  uint*_t */
#include "fw_bool.h"    /*  boolean type */
#include "fw_params.h"    /* OTP structs */

/* ============================================================================
 * Public data types
 * ========================================================================= */
#define PING_PONG_MAX_BUFFER_SIZE (1024)
/* ----------------------------------------------------------------------------
 * OTP File System Command Formats
 * ------------------------------------------------------------------------- */
/** Command Field Definition.
 * This field defines the kind of file system command.
 *
 * How to handle different version of file system command (e.g. an existing command need to be changed):
 * The file system routine ignores unknown file system command (e.g. new defined command or old unsupported commands).
 * Updating or changing a command requires to assign a new command enumerator number.
 * This method ensures that message definition changes can be easily detected,
 * because it only processes commands which are known and where the content is as expected.
 * The upper byte of the cmd is used as xvalent id and can be in the range of 0 - 15*/
typedef enum
{
    /** Stops parsing the patches in case this command word is found */
    OTP_FS_CMD_TERMINATION = 0x0000u,
    /** TRIM Structure. This table defines the structure of the trim registers  */
    OTP_FS_CMD_TRIM_STRUCTURE = 0x0001u,
    /** TRIM Data. Content of the trim registers  */
    OTP_FS_CMD_TRIM_DATA = 0x0002u,
    /** Config Structure. This table defines the structure of the config registers  */
    OTP_FS_CMD_CONFIG_STRUCTURE = 0x0003u,
    /** Configuration Data. Content of the configuration registers  */
    OTP_FS_CMD_CONFIG_DATA = 0x0004u,
    /** Default PMBUS Structural Data. Describes the PMBUS commands  */
    OTP_FS_CMD_PMBUS_STRUCTURE = 0x0005u,
    /** Default PMBUS Data. Content of the PMBUS Configuration  */
    OTP_FS_CMD_PMBUS_DEFAULT_LP0 = 0x0006u,
    /** Default PMBUS Data. Content of the PMBUS Configuration  */
    OTP_FS_CMD_PMBUS_USER_LP0 = 0x0007u,
    /** Default PMBUS Data. Content of the PMBUS Configuration  */
    OTP_FS_CMD_PMBUS_DEFAULT_LP1 = 0x0008u,
    /** Default PMBUS Data. Content of the PMBUS Configuration  */
    OTP_FS_CMD_PMBUS_USER_LP1 = 0x0009u,
    /** Partial Configuration for conf registers. */
    OTP_FS_CMD_CONF_PARTIAL = 0x000Au,
    /** Partial Configuration for PMBUS Commands. */
    OTP_FS_CMD_PMBUS_PARTIAL = 0x000Bu,
    /** Reserved. */
    OTP_FS_RESERVED3 = 0x000Cu,
	/** Reserved. */
    OTP_FS_RESERVED4 = 0x000Eu,
    /** Reserved. */
    OTP_FS_RESERVED5 = 0x000Fu,
    /** Patch Image with firmware version check. Loads this patch after the RTX OS is successfully started.
     * Then it copies the attached data with the given length to the provided 'ram_address'.
     * It checks if the current running firmware version UTC timestamp fits to firmware version where the
     * patch is generated for.
     * Afterwards it calls the patch entry callback routine (if given == unequal zero). The callback
     * routine could be linked against the ROM code and could use the full RTX OS functionality.
     * The patch contains a new firmware version information. The current firmware UTC timestamp information
     * is updated based on the patch information. */
    OTP_FS_PATCH_IMAGE_VERSION = 0x0010u,
    /** Reserved. */
    OTP_FS_RESERVED6 = 0x0011u,
    /** Reserved. */
    OTP_FS_RESERVED7 = 0x0012u,
    /** Reserved.*/
    OTP_FS_RESERVED8 = 0x0013u,

    /** Invalid command, will be ignored and the parser jumps to the next command message */
    OTP_FS_CMD_INVALID = 0xFFFFu
} Otp_File_System_Cmd_Types_t;

/** OTP file system command option field. Used by the option
 * field in the Otp_File_System_Cmd_Hdr_t structure.
 * If this option field is set to '1' the patch is considered to be compiled for an
 * absolute OTP address.
 * Otherwise it assumes that the patch was compiled for the beginning of
 * the corresponding patch partition.
 */
#define OTP_FS_PATCH_ABSOLUTE              (0x1u)

/** OTP file system command header structure */
typedef struct
{
    /** Lower 8 bits File system command <br>
     *  Higher 8 bits: Xvalent selector
     * (supported field selection: @ref Otp_File_System_Cmd_Types_t).
     * */

    uint16_t cmd;

    /** Option field. Supported bit values are:
     * - @ref OTP_FS_PATCH_ABSOLUTE
     */
    uint16_t option;

    /** Overall file system command size in Bytes (includes the command and size parameter). */
    uint16_t size;
    /** Overall file system command size in Bytes (includes the command and size parameter). */
    uint16_t size_red;

    /** CRC-32 value of the command header blocks (e.g. Otp_File_System_Cmd_Hdr_t + Patch_Cmd_Image_t).
     * This checksum field is ignored in case it is set to zero.
     * The checksum calculation includes the @ref Otp_File_System_Cmd_Hdr_t header fields 'cmd', 'option',
     * 'size' and 'size_red'.
     * It also includes all field of any sub-command header
     * (e.g. @ref Patch_Cmd_Image_t for command @ref OTP_FS_CMD_PATCH_IMAGE).
     *
     * Files with a wrong checksum value are ignored and the following file system commands are processed.
     */
    uint32_t crc_hdr;

} Otp_File_System_Cmd_Hdr_t;

/** CRC-32 value of the overall command data (excluding command header blocks).
 * This checksum is ignored in case the @ref OTP_FS_CMD_OPT_DATA_CRC bit is not set in the
 * option field and the header checksum is zero.
 * The checksum calculation includes all command data Bytes
 * (data bytes that follow the command header, or sub-command header respectively).
 * This guy is now moved to the end of the data.*/

typedef uint32_t crc_data_t;

/** This structure defines the command parameter for the @ref OTP_FS_PATCH_IMAGE_VERSION command.
 * The @ref Patch_Cmd_Image_Version_t structure data are followed by the patch data. The amount of
 * data is calculated as <code>Patch_Cmd_t.size - sizeof(Otp_File_System_Cmd_Hdr_t) - sizeof(Patch_Cmd_Image_Version_t)</code>.
 */
typedef struct
{
    /** Target OTP address where to store the overall patch (including the patch command header). */
    uint32_t location;
    /** Target OTP entry address where to execute the patch image (callback function). */
    uint32_t execute;
    /** Firmware version UTC timestamp where the patch can be applied on. The patch is not applied and discarded in case
     *  the current firmware has a different UTC timestamp.
     */
    uint32_t version_base;
    /** New firmware version information. The current firmware UTC timestamp information
     * is updated based on the patch information. */
    uint32_t version_new;
} Patch_Cmd_Image_Version_t;

/* ----------------------------------------------------------------------------
 * OTP File System Scan Command Callback Type Definitions
 * ------------------------------------------------------------------------- */

/** Type of OTP file system command handler callback.
 * @param partition specifies the OTP partition.
 * @param otp_offset address in side the otp
 * @param otp_end specifies the offset where the next command is expected.
 *
 * @throws command handler specific exception.
 */
typedef  void (*Otp_File_System_Cmd_Callback_t)(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint32_t otp_end);

/** OTP file system command handler mapping.
 * A callback handler is mapped to handler OTP file system command of the given command type. */
typedef struct
{
    /** File system command type where to call the @ref handler callback to handle such commands. */
    Otp_File_System_Cmd_Types_t cmd_type;
    /** Additional information about the file type */
    uint16_t option;
    /** File system sub-command header size. This parameter is needed to check the file system consistency. */
    uint8_t cmd_hdr_size;
    /** File system minimum command size. This parameter is needed to check the file system consistency. */
    uint8_t cmd_size_min;
    /** File system command handler to call in case a valid @ref cmd_type file system command is found in OTP. */
    Otp_File_System_Cmd_Callback_t handler;
} Otp_File_System_Cmd_Handler_t;

/** OTP file system write buffer.
 * This type is used to hold information about the write process  */
typedef struct
{
    /** State variable to keep track of which buffer is currently active */
    uint8_t is_ping;
    /** Status of the write transaction */
    uint32_t status;
    /** Fault variable to store whether an error has occurred or not */
    uint8_t is_write_fault;
    /** Number of current byte which is processed */
    uint32_t current_otp_byte_offset;
} Otp_File_System_Write_Buffer_t;
/* ============================================================================
 * API functions
 * ========================================================================= */

/* ----------------------------------------------------------------------------
 * OTP File System APIs
 * ------------------------------------------------------------------------- */

/**
 * This function scans the overall OTP file system for valid OTP file system commands.
 * Commands which are listed in @ref Otp_File_System_Cmd_Types_t are supported.
 *
 * This routine is called with a list of callback handler for dedicated file system command types.
 * Those handler callback routines are called in case such file system commands are found in OTP.
 *
 * @param partition Parameter to indicate used OTP partition
 * @param handler Pointer to an file system command handler array. Each array entry maps a file system command to a dedicated callback handler.
 * @param cmd_header return variable containing pointer to the address of the header in the OTP
 *
 * @remark Invalid file system command (e.g. @ref OTP_FS_CMD_INVALID) are ignored.
 *
 * @throws CORRUPT_MEMORY if correct file system command exceeds the OTP size.
 * @throws ILLEGAL_PARAMETER from otp_read.
 * @throws TIMEOUT from otp_read.
 */
void otp_fs_scan(Otp_Partition_Selector_t partition, const Otp_File_System_Cmd_Handler_t* handler, Otp_File_System_Cmd_Hdr_t** cmd_hdr);
/**
 * This function stores data into the OTP.
 * The caller of this routine needs to make sure that the data
 * - is consistent (header and data crc are correct, size is correct)
 * - is not exceeding the size of the respective partition
 * This function supports to store multiple data files.
 * A ping pong buffer of 1 kByte each is provided to store the data
 *
 * @param partition Parameter to indicate used OTP partition
 *              - 0 for data partitions
 *              - 1 - 16 for patch partitions
 * @param is_hdr Parameter to indicate if this is the first chunk of data including the first header.
 * @param ptr RAM address location of the data
 * @param num Size of the data (number of Bytes) maximum 1024
 *
 * @throws ILLEGAL_PARAMETER
 *      - if the command's size and redundant size are inconsistent.
 *      - the command's size is not big enough to be a file system header.
 *      - the command's size exceeds the overall given size (defined through @p num).
 *      - from otp_read and otp_write.
 * @throws OUT_OF_RESOURCE if the file cannot be stored in the remaining free OTP space.
 * @throws CORRUPT_MEMORY if header cmd or header checksum values is wrong.
 * @throws TIMEOUT from otp_read and otp_write.
 */
void otp_fs_data_store(Otp_Partition_Selector_t partition, uint8_t is_hdr, void* ptr, uint32_t num);


/** The otp_fs_data_store_status_get retrieves the status of the data_store command
 *
 *  @return  status of the data_store
 *              - 0 ... free
 *              - 1 ... ping buffer is busy
 *              - 2 ... pong buffer is busy
 *              - 3 ... both are busy.
 *
 */
uint32_t otp_fs_data_store_status_get(void);

/** The otp_fs_data_store_fault_get retrieves a fault of the data_store command
 *
 *  @return  fault status of the otp_fs_store command
 *              - 0 ... no error
 *              - 1 ... write fault occurred
 */
uint8_t otp_fs_data_store_fault_get(void);

/** The otp_fs_data_store_fault_clear clears a write fault
 *
 */
void otp_fs_data_store_fault_clear(void);

/**																
 * This function programs the given file into the OTP.
 * Only files with a valid command header should be stored in OTP.
 * This function supports to store multiple files (consecutive placed in RAM).
 *
 * @param partition Parameter to indicate used OTP partition
 * @param ptr RAM address location of the file(s)
 * @param num Size of the file (number of Bytes)
 *
 *
 * @throws ILLEGAL_PARAMETER
 *      - if the command's size and redundant size are inconsistent.
 *      - the command's size is not big enough to be a file system header.
 *      - the command's size exceeds the overall given size (defined through @p num).
 *      - from otp__read and otp__write.
 * @throws OUT_OF_RESOURCE if the file cannot be stored in the remaining free OTP space.
 * @throws CORRUPT_MEMORY if header cmd or header checksum values are wrong.
 * @throws TIMEOUT from otp_read and otp_write.
 */
void otp_fs_command_store(Otp_Partition_Selector_t partition, void* ptr, uint32_t num);


/**
 * This function invalidates all OTP file system commands of the given type @ref cmd_type.
 *
 * @param partition Parameter to indicate used OTP partition
 * @param cmd_type File system command type *
 * @param option   File system option field
 *
 * @throws OUT_OF_RESOURCE from otp_fs_scan and otp_fs_command_store.
 * @throws ILLEGAL_PARAMETER from otp_fs_scan and otp_fs_command_store.
 * @throws TIMEOUT from otp_fs_scan and otp_fs_command_store.
 */
void otp_fs_command_invalidate(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Types_t cmd_type, uint16_t option);

/**
 * Update the OTP file system header information (e.g. setting redundant length field and
 * update the header and data checksum).
 *
 * Afterwards it programs the command to the OTP.
 *
 * @param partition Parameter to indicate used OTP partition
 * @param p_cmd_hdr Pointer to OTP FS header, which is already filled out (e.g. command
 *                  type, option field, length field). Header is followed by the command data.
 */
void otp_fs_command_header_update_program(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Hdr_t* p_cmd_hdr);

/**
 * Compares the crc of 2 different crc sources. This function is useful to check if
 * the data to be stored in OTP are already there.
 * In this case no action will be taken.
 * The size of the OTP file system command should already be configured correctly.
 *
 * @param p_old_cmd_hdr Pointer to the command header in OTP.
 * @param p_new_cmd_hdr Pointer to the command header in RAM.
 *
 * @return @c boolean_true in case of they checksum is equal, otherwise @c boolean_false.
 */
boolean otp_fs_ram_compare_crc(Otp_File_System_Cmd_Hdr_t* p_old_cmd_hdr, Otp_File_System_Cmd_Hdr_t* p_new_cmd_hdr);

/**
 * Calculates and checks the CRC value of the OTP file system header blocks that are stored in RAM.
 * The size of the OTP file system command should already be configured correctly.
 *
 * @param p_cmd_hdr Pointer to the command header.
 * @param sub_hdr_size Size in Byte of the sub-command header which follows the command header.
 * This sub-command header is included in the checksum calculation.
 *
 * @return @c boolean_true in case of a valid checksum, otherwise @c boolean_false.
 */
boolean otp_fs_ram_crc_hdr_is_valid(const Otp_File_System_Cmd_Hdr_t *p_cmd_hdr, uint16_t sub_hdr_size);

/**
 * Calculates and updates the CRC value of the OTP file system header blocks that are stored in RAM.
 * The size of the OTP file system command should already be configured correctly.
 * This function also sets the CRC option bit in the command header.
 *
 * @param p_cmd_hdr Pointer to the command header.
 * @param sub_hdr_size Size in Byte of the sub-command header which follows the command header.
 * This sub-command header is included in the checksum calculation.
 */
void otp_fs_ram_crc_hdr_generate(Otp_File_System_Cmd_Hdr_t *p_cmd_hdr, uint16_t sub_hdr_size);

/**
 * Calculates and checks the CRC value of the OTP file system data blocks that are stored in RAM.
 * The size of the OTP file system command should already be configured correctly.
 *
 * @param p_cmd_hdr Pointer to the command header.
 * @param sub_hdr_size Size in Byte of the sub-command header which follows the command header.
 *
 * @return @c boolean_true in case of a valid checksum, otherwise @c boolean_false.
 */
boolean otp_fs_ram_crc_data_is_valid(Otp_File_System_Cmd_Hdr_t *p_cmd_hdr, uint16_t sub_hdr_size);

/**
 * Calculates and updates CRC value of the OTP file system data blocks that are stored in RAM.
 * The size of the OTP file system command should already be configured correctly.
 *
 * @param p_cmd_hdr Pointer to the command header.
 * @param sub_hdr_size Size in Byte of the sub-command header which follows the command header.
 */
void otp_fs_ram_crc_data_generate(Otp_File_System_Cmd_Hdr_t *p_cmd_hdr, uint16_t sub_hdr_size);

/**
 * Loads the OTP Files system header and the command specific sub-header. It verifies the header
 * checksum, which includes the sub-header fields.
 *
 * @param partition Parameter to indicate used OTP partition
 * @param otp_offset offset where to start reading the header and sub-header
 * @param p_cmd_hdr Pointer to the RAM location where to store the header and sub-header
 * @param sub_hdr_size Sub-command specific sub-Header size. This parameter is needed in order to read the
 *        sub-header and perform the header checksum check.
 *
 * @throws CORRUPT_MEMORY in case the header has a wrong checksum.
 * @throws ILLEGAL_PARAMETER from otp_read.
 * @throws TIMEOUT from otp_read.
 */
void otp_fs_hdr_load(Otp_Partition_Selector_t partition, uint32_t otp_offset, uint8_t** p_cmd_hdr, uint16_t sub_hdr_size);

/**
 * Loads the OTP Files system command. It verifies the header
 * checksum, which includes the sub-header fields.
 *
 * @param partition Parameter to indicate used OTP partition
 * @param otp_offset OTP offset where to start reading the command
 * @param p_cmd_hdr Pointer to the RAM location where to store the command
 * @param cmd_size Overall command size.
 */
void otp_fs_cmd_load(Otp_Partition_Selector_t partition, uint32_t otp_offset, Otp_File_System_Cmd_Hdr_t** p_cmd_hdr, uint16_t cmd_size);

/**
 * Checks the used size in byte in the OTP
 * @param partition Parameter to indicate used OTP partition
 *
 * @return @c remaining size in the given OTP partition.
 */

uint32_t otp_fs_used_size(Otp_Partition_Selector_t partition);

/**
 * Gets the maximum available xvalent configuration
 * @param partition Parameter to indicate used OTP partition
 * @param cmd indicates which command to check
 *
 * @return @c maximum available xvalent.
 */

uint8_t otp_fs_max_xvalent(Otp_Partition_Selector_t partition, Otp_File_System_Cmd_Types_t cmd);

#endif /* OTP_FS_H */
