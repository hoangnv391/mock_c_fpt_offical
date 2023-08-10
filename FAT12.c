#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HAL.h"
#include "FAT12.h"
#include "cluster_linked_list.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void set_boot_sector();
void set_regions_address();
void print_offset_of_regions();
static int8_t check_type_of_entry(FAT12_ENTRY_Typedef *entry);
static int8_t entry_is_empty(FAT12_ENTRY_Typedef *entry);
uint32_t print_entry_from_sector(uint32_t number_of_sector);
int32_t get_entry_value_from_FAT(uint16_t number_of_cluster);
int32_t get_entry_value_from_a_pair_cluster_entry\
        (FAT12_PAIR_CLUSTER_Typedef pair_entry, uint8_t order);
uint16_t get_physical_sector_of_root();
void read_file_on_multi_sector(uint16_t starting_cluster_number);
void print_buffer(uint8_t *buff);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FAT12_BOOT_SECTOR_Typedef s_boot_sector;
static unsigned int s_boot_sector_address = 0u;
static unsigned int s_fat_address = 512u; // Size of Boot Sector in FAT12 is 512 bytes
static unsigned int s_root_directory_address = 0U;
static unsigned int s_data_region_address = 0U;

static uint16_t s_number_of_bytes_per_sector = 0u;
static uint8_t s_number_of_fat = 0u;
static uint16_t s_number_of_sector_per_fat = 0u;
static uint16_t s_physical_sector_of_root = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/
void set_boot_sector()
{
    set_file_ptr_position(0, SEEK_SET);
    HAL_read_sector(0, (uint8_t*)(&s_boot_sector)); // ?? Just read 248 bytes

    // FAT12_BOOT_SECTOR_Typedef *boot_ptr = &s_boot_sector;
    // printf("Manufacture desc: ");
    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%c", *(uint8_t*)(boot_ptr->MANUFACTURE_DESC + i));
    // }
    // printf("\n");
    // printf("Number of bytes per sector: %d bytes\n", *(uint16_t *)(boot_ptr->NUMBER_OF_BYTES_PER_SECTOR));
    return;
}

void set_regions_address()
{
    s_number_of_bytes_per_sector = *(uint16_t *)(s_boot_sector.NUMBER_OF_BYTES_PER_SECTOR);
    s_number_of_fat = *(uint8_t *)(s_boot_sector.NUMBER_OF_FAT);
    s_number_of_sector_per_fat = *(uint16_t *)(s_boot_sector.NUMBER_OF_SECTORS_PER_FAT);
    uint16_t number_of_rdet = *(uint16_t *)(s_boot_sector.NUMBER_OF_RDET); //Number of root directory entry (RDET)

    s_root_directory_address = s_fat_address\
        + s_number_of_fat * (s_number_of_sector_per_fat * s_number_of_bytes_per_sector);

    s_data_region_address = s_root_directory_address\
        + number_of_rdet * SIZE_OF_ENTRY_IN_BYTES;

    s_physical_sector_of_root = SIZE_OF_BOOT_SECTOR_IN_SECTORS + s_number_of_fat * s_number_of_sector_per_fat;
    printf("Starting sector of root: %d\n", s_physical_sector_of_root);
    return;
}

void print_offset_of_regions()
{
    printf("Regions Address (offset in hex): \n");
    printf("Boot sector: %x\n", s_boot_sector_address);
    printf("FAT: %x\n", s_fat_address); 
    printf("Root directory: %x\n", s_root_directory_address);
    printf("Data: %x\n", s_data_region_address);
    return;
}

static int8_t check_type_of_entry(FAT12_ENTRY_Typedef* entry)
{
    ENTRY_TYPE_Typedef entry_type = _OTHER;
    uint8_t value_of_11th_byte = *((uint8_t *)(entry) + 11);

    if (value_of_11th_byte == SIGNATURE_BYTE_OF_LFN_ENTRY)
    {
        entry_type = _LFN;
    }
    else if (entry->FILE_NAME[0] != 0 && entry->FILE_NAME_EXTENSION[0] != 32)
    {
        entry_type = _FILE;
    }
    else if (entry->FILE_NAME[0] != 0 && entry->FILE_NAME_EXTENSION[0] == 32)
    {
        entry_type = _FOLDER;
    }
    else
    {
        entry_type = _OTHER;
    }
    return entry_type;
}

static int8_t entry_is_empty(FAT12_ENTRY_Typedef* entry)
{
    uint64_t check = *(uint64_t *)(entry);
    int i;
    for (i = 0; i < 4; i++)
    {
        if (check != 0)
            return 1;
        check = *((uint64_t *)(entry) + i);
    }
    return -1;
}

void print_file_name_from_LNF_entry(FAT12_LFN_ENTRY_Typedef* entry)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("%c", *((uint16_t *)(entry->LOW_FILE_NAME) + i));
    }

    for (i = 0; i < 6; i++)
    {
        printf("%c", *((uint16_t *)(entry->MID_FILE_NAME) + i));
    }

    for (i = 0; i < 5; i++)
    {
        printf("%c", *((entry->HIGH_FILE_NAME) + i));
    }
}

void printf_file_info_of_entry(FAT12_ENTRY_Typedef* entry)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        printf("%c", (entry->FILE_NAME)[i] );
    }
    printf("    ");

    if (check_type_of_entry(entry) == _FOLDER)
    {
        printf("Folder");
    }
    else
    {
        for (i = 0; i < 3; i++)
        {
            printf("%c", (entry->FILE_NAME_EXTENSION)[i]);
        }
    }

    // uint16_t starting_cluster_number = *(uint16_t *)(entry->STARTING_CLUSTER_NUMBER);
    // printf("    %d", starting_cluster_number);

}

uint32_t print_entry_from_sector(uint32_t physical_sector_index)
{
    uint64_t number_of_jump_bytes = physical_sector_index * s_number_of_bytes_per_sector;
    uint16_t number_of_LFN_entry = 0;
    uint16_t first_cluster_of_parent_dir = 0;
    uint32_t physical_sector_index_of_parent_dir = 0;

    uint8_t current_node_is_set = 0;
    FAT12_ENTRY_Typedef* entry = NULL;
    uint8_t buff[32] = {0};
    int index = 0;
    HAL_read_bytes_from_file(number_of_jump_bytes, SEEK_SET, SIZE_OF_ENTRY_IN_BYTES, (uint8_t *)(&buff));
    entry = (FAT12_ENTRY_Typedef *)(&buff);

    while (entry_is_empty((FAT12_ENTRY_Typedef*)(&buff)) == 1)
    {
        if (*(uint8_t*)(entry) != SIGNATURE_FIRST_BYTE_OF_FOLDER_ENTRY)
        {
            uint8_t entry_type = check_type_of_entry(entry);
            if (entry_type == _LFN)
            {
                printf("%d. ", index);
                print_file_name_from_LNF_entry((FAT12_LFN_ENTRY_Typedef *)(entry));
                index++;
                printf("\n");
                number_of_LFN_entry++;
            }
            else if (entry_type == _FILE)
            {
                uint16_t starting_cluster_number = *(uint16_t *)(entry->STARTING_CLUSTER_NUMBER);
                CLUSTER_NODE_Typedef *new_node = init_node(starting_cluster_number, _FILE);

                if (current_node_is_set == 0)
                {
                    delete_entire_from_current_node();
                    set_current_node(new_node);
                    current_node_is_set = 1;
                }
                add_at_tail(new_node);

                if (number_of_LFN_entry == 0)
                {
                    printf("%d. ", index);
                    printf_file_info_of_entry(entry);
                    index++;
                    printf("\n");
                }
            }
            else if (entry_type == _FOLDER)
            {
                uint16_t starting_cluster_number = *(uint16_t *)(entry->STARTING_CLUSTER_NUMBER);
                CLUSTER_NODE_Typedef *new_node = init_node(starting_cluster_number, _FOLDER);
                
                if (current_node_is_set == 0)
                {
                    delete_entire_from_current_node();
                    set_current_node(new_node);
                    current_node_is_set = 1;
                }
                add_at_tail(new_node);

                printf("%d. ", index);
                printf_file_info_of_entry(entry);
                index++;
                printf("\n");
            }
        }
        else
        {
            if (*(uint16_t*)(entry) == SIGNATURE_FIRST_TWO_BYTES_OF_FOLDER_ENTRY)
            {
                first_cluster_of_parent_dir = *(uint16_t *)(entry->STARTING_CLUSTER_NUMBER);
                if (first_cluster_of_parent_dir == 0)
                {
                    physical_sector_index_of_parent_dir = s_physical_sector_of_root;
                }
                else
                {
                    physical_sector_index_of_parent_dir = PHYSICAL_SECTOR_NUMBER(first_cluster_of_parent_dir);
                }
            }
        }
        number_of_jump_bytes += SIZE_OF_ENTRY_IN_BYTES;
        HAL_read_bytes_from_file(number_of_jump_bytes, SEEK_SET, SIZE_OF_ENTRY_IN_BYTES, (uint8_t *)(&buff));
        entry = (FAT12_ENTRY_Typedef *)(&buff);
    } 
    // printf("\nFirst cluster number of parent dir: %d\n", physical_sector_index_of_parent_dir);
    // show_current_node();
    // printf("\n");
    // print_linked_list();
    return physical_sector_index_of_parent_dir;
}

int32_t get_entry_value_from_FAT(uint16_t number_of_cluster)
{
    // In FAT12, two entry of two cluster is stored in 3 bytes
    uint16_t number_of_jump_bytes = s_fat_address + (uint16_t)(3 * (number_of_cluster / (uint16_t)2) );

    FAT12_PAIR_CLUSTER_Typedef pair_cluster_status;
    if (HAL_read_bytes_from_file(number_of_jump_bytes, SEEK_SET, 3, (uint8_t*)(&pair_cluster_status)) == 0)
    {
        return -1;
    }
    // printf("%d %d %d\n", pair_cluster_status.fst_byte, pair_cluster_status.snd_byte, pair_cluster_status.thrd_byte);
    return get_entry_value_from_a_pair_cluster_entry(pair_cluster_status, (uint8_t)(number_of_cluster % 2));
}

int32_t get_entry_value_from_a_pair_cluster_entry (FAT12_PAIR_CLUSTER_Typedef pair_entry, uint8_t order) // hiểu kiểu lưu trữ entry
{
    int32_t result = -1;
    if (order == _LOWER_ENTRY) // lower entry
    {
        result = (uint16_t)(uint8_t)((pair_entry.snd_byte) <<  4) << 4 | (uint16_t)(pair_entry.fst_byte); // Chú ý ép kiểu ngầm định
    }
    else if (order == _HIGHER_ENTRY) // higher entry
    {
        result = (((uint16_t)(pair_entry.thrd_byte)) << 4) | (((uint16_t)(pair_entry.snd_byte)) >> 4);
    }
    return result;
}

uint16_t get_physical_sector_of_root()
{
    return s_physical_sector_of_root;
}

void read_file_on_multi_sector(uint16_t starting_cluster_number)
{
    uint16_t next_cluster_number = 0;
    uint16_t amount_of_read_bytes = 0;
    uint32_t physical_sector_number = 0;

    next_cluster_number = starting_cluster_number;
    do
    {
        physical_sector_number = PHYSICAL_SECTOR_NUMBER(next_cluster_number);
        uint8_t buff[512] = {0};
        // printf("%d ", next_cluster_number);
        amount_of_read_bytes = HAL_read_sector(physical_sector_number, (uint8_t *)(&buff));
        print_buffer((uint8_t *)(&buff));
        next_cluster_number = get_entry_value_from_FAT(next_cluster_number);
    // } while (next_cluster_number != EOF_VALUE_OF_SECTOR && next_cluster_number != -1);
    } while (next_cluster_number >= MIN_VALUE_OF_DATA_CLUSTER &&\
            next_cluster_number <= MAX_VALUE_OF_DATA_CLUSTER &&\
            next_cluster_number != -1);

    // printf("~");
}

void print_buffer(uint8_t* buff)
{
    uint8_t *temp = buff;
    while (*temp != 0)
    {
        printf("%c", *temp);
        temp++;
    }
}