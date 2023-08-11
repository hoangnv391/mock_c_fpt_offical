#ifndef FAT12_H
    #define FAT12_H

    #include <stdint.h>

    #pragma pack(1)

    /*******************************************************************************
     * Definitions
     ******************************************************************************/
    #define SIZE_OF_ENTRY_IN_BYTES 32u
    #define SIZE_OF_BOOT_SECTOR_IN_SECTORS 1u

    #define SIGNATURE_BYTE_OF_LFN_ENTRY 0x0F
    #define NUMBER_OF_LOW_FILE_NAME_CHARACTER 5
    #define NUMBER_OF_MID_FILE_NAME_CHARACTER 6
    #define NUMBER_OF_HIGH_FILE_NAME_CHARACTER 5

    #define SIGNATURE_FIRST_BYTE_OF_FOLDER_ENTRY 0x2E
    #define SIGNATURE_FIRST_BYTE_OF_FOLDER_ENTRY 0x2E
    #define SIGNATURE_FIRST_TWO_BYTES_OF_FOLDER_ENTRY 0x2E2E

    #define MIN_VALUE_OF_DATA_CLUSTER 0x002
    #define MAX_VALUE_OF_DATA_CLUSTER 0xFEF

    #define PHYSICAL_SECTOR_NUMBER(sector_number) (33 + (sector_number) - 2)
    


    typedef enum _ENTRY_TYPE
    {
        _LFN = 0u,
        _FILE = 1u,
        _FOLDER = 2u,
        _OTHER = 3u
    } ENTRY_TYPE_Typedef;

    typedef enum _ENTRY_ORDER
    {
        _LOWER_ENTRY = 0u,
        _HIGHER_ENTRY = 1u
    } ENTRY_ORDER_Typedef;

    typedef struct FAT12_SECTOR
    {
        uint8_t BYTES[512];
    } FAT12_SECTOR_Typedef;

    typedef struct FAT12_BOOT_SECTOR
    {
        uint8_t PART_OF_BOOTSTRAP[3];
        uint8_t MANUFACTURE_DESC[8];
        uint8_t NUMBER_OF_BYTES_PER_SECTOR[2];
        uint8_t NUMBER_OF_SECTORS_PER_CLUSTER[1]; // 1
        uint8_t NUMBER_OF_RESERVED_SECTORS[2];
        uint8_t NUMBER_OF_FAT[1]; // 1
        uint8_t NUMBER_OF_RDET[2]; //Number of root directory entry (RDET)
        uint8_t TOTAL_NUMBER_OF_SECTORS[2];
        uint8_t MEDIA_DESC[1]; // 1
        uint8_t NUMBER_OF_SECTORS_PER_FAT[2];
        uint8_t NUMBER_OF_SECTORS_PER_TRACK[2];
        uint8_t NUMBER_OF_HEADS[2];
        uint8_t NUMBER_OF_HIDDEN_SECTORS[2];
        uint8_t BOOTSTRAP[480];
        uint8_t SIGNATURE[2];
    } FAT12_BOOT_SECTOR_Typedef;

    typedef struct FAT12_ENTRY
    {
        uint8_t FILE_NAME[8];
        uint8_t FILE_NAME_EXTENSION[3];
        uint8_t FILE_ATTRIBUTES[1];
        uint8_t RESERVED[10];
        uint8_t TIME_CREATED[2]; // or last updated
        uint8_t DATE_CREATED[2]; // or last updated
        uint8_t STARTING_CLUSTER_NUMBER[2];
        uint8_t FILE_SIZE_IN_BYTES[4];
    } FAT12_ENTRY_Typedef;

    typedef struct FAT12_LFN_ENTRY
    {
        uint8_t FILE_INDEX[1];
        uint8_t LOW_FILE_NAME[10];
        uint8_t FILE_ATTRIBUTE[1];
        uint8_t FILE_RESERVED_1[1];
        uint8_t CHECK_SUM[1];
        uint8_t MID_FILE_NAME[12];
        uint8_t FILE_RESERVED_2[1];
        uint8_t HIGH_FILE_NAME[5];
    } FAT12_LFN_ENTRY_Typedef;

    typedef struct FAT12_PAIR_CLUSTER
    {
        uint8_t fst_byte;
        uint8_t snd_byte;
        uint8_t thrd_byte;
    } FAT12_PAIR_CLUSTER_Typedef; 

    #pragma pack()

    /*******************************************************************************
     * API
     ******************************************************************************/
    int8_t set_boot_sector();
    int8_t set_regions_address();
    int8_t print_offset_of_regions();
    uint32_t print_entry_from_sector(uint32_t number_of_sector);
    int32_t get_entry_value_from_FAT(uint16_t number_of_cluster);
    int32_t get_entry_value_from_a_pair_cluster_entry\
            (FAT12_PAIR_CLUSTER_Typedef pair_entry, uint8_t order);
    uint16_t get_physical_sector_of_root();
    void read_file_on_multi_sector(uint16_t starting_cluster_number);
    int8_t setup_FAT12_file();

#endif /* FAT12_H */