#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "HAL.h"
#include "status_code.h"

#define SIZE_OF_SECTOR_IN_BYTES 512u // viet ham de thay doi gia tri nay???
#define SIZE_OF_UINT8_T_IN_BYTES 1u

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
uint8_t open_file(char *str);
uint8_t set_file_ptr_position(long int offset, uint8_t whence);
uint64_t HAL_read_sector(uint32_t index, uint8_t *buff);
int32_t HAL_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff);
uint64_t HAL_read_bytes_from_file(long int offset, uint8_t whence,
                                  uint16_t number_of_bytes, uint8_t *buff);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static FILE *s_file = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/
uint8_t open_file(char *str)
{
    s_file = fopen(str, "r");
    if (s_file == NULL)
    {
        return -1;
    }
    return 1;
}

uint8_t set_file_ptr_position (long int offset, uint8_t whence)
{   // This function returns zero if successful, or else it returns a non-zero value. 
    return fseek(s_file, offset, whence); 
}

uint64_t HAL_read_sector(uint32_t index, uint8_t *buff)
{
    if (index < 0 || buff == NULL)
    {
        return -1;
    }

    uint16_t amount_of_jump_bytes = SIZE_OF_SECTOR_IN_BYTES * index;
    fseek(s_file, amount_of_jump_bytes, SEEK_SET);
    return fread(buff, SIZE_OF_UINT8_T_IN_BYTES, SIZE_OF_SECTOR_IN_BYTES, s_file);
}

int32_t HAL_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff)
{
    // if (index < 0 || num < 0 || buff == NULL)
    // {
    //     return -1;
    // }

    // uint16_t amount_of_jump_bytes = SIZE_OF_SECTOR_IN_BYTES * index;
    // uint64_t amount_of_read_bytes = 0;
    // uint64_t total_number_of_read_bytes = 0;

    // fseek(s_file, amount_of_jump_bytes, SEEK_SET);
    // amount_of_read_bytes = fread(buff, SIZE_OF_UINT8_T_IN_BYTES, SIZE_OF_SECTOR_IN_BYTES, s_file);
    // total_number_of_read_bytes += amount_of_read_bytes;

    // if (amount_of_read_bytes == SIZE_OF_SECTOR_IN_BYTES)
    // {
    //     while (num > 1 && amount_of_read_bytes > 0)
    //     {
    //         buff += amount_of_read_bytes;
    //         amount_of_read_bytes = fread(buff, SIZE_OF_UINT8_T_IN_BYTES, SIZE_OF_SECTOR_IN_BYTES, s_file);
    //         total_number_of_read_bytes += amount_of_read_bytes;
    //         num--;
    //     }
    // }
    // return total_number_of_read_bytes;
    if (num < 1 || buff == NULL)
    {
        return _STATUS_FAIL;
    }

    uint32_t sector_couter = 0u;
    int32_t amount_of_read_bytes = 0u;
    uint8_t *temp = buff;
    int32_t total_number_of_read_bytes = 0u;

    while (sector_couter < num)
    {
        amount_of_read_bytes = HAL_read_sector(index, temp);
        total_number_of_read_bytes += amount_of_read_bytes;
        sector_couter++;
        index++;
        temp += SIZE_OF_SECTOR_IN_BYTES;
    }
    return total_number_of_read_bytes;
}

uint64_t HAL_read_bytes_from_file(long int offset, uint8_t whence,\
                                    uint16_t number_of_bytes, uint8_t *buff)
{
    fseek(s_file, offset, whence);
    return fread(buff, SIZE_OF_UINT8_T_IN_BYTES, number_of_bytes, s_file);
}