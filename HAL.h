#ifndef HAL_H
    #define HAL_H
    
    #include <stdint.h>
    #include <stdlib.h>
    
    /*******************************************************************************
     * API
     ******************************************************************************/
    int8_t open_file(char *str);
    int8_t set_file_ptr_position(long int offset, uint8_t whence);
    int32_t HAL_read_sector(uint32_t index, uint8_t *buff);
    int32_t HAL_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff);
    int32_t HAL_read_bytes_from_file(long int offset, uint8_t whence,
                                      uint16_t number_of_bytes, uint8_t *buff);

#endif /* HAL_H */
