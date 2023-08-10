#include <stdio.h>
#include <stdint.h>
#include "HAL.h"
#include "FAT12.h"
#include "cluster_linked_list.h"


int main()
{
    printf("Result: %d\n", open_file("floppy.img"));
    set_boot_sector();
    set_regions_address();
//    print_offset_of_regions();
    print_entry_from_sector(19);
    CLUSTER_NODE_Typedef *chosen_node = NULL;
    int choice;
    uint32_t physical_sector_index_of_parent_dir = 0;
    uint32_t physical_sector_index_of_current_folder = 0;
    while (choice != -2)
    {
        printf("\n");
        printf("Nhap lua chon cua ban: ");
        scanf("%d", &choice);
        if (choice != -2)
        {
            if (choice == -1 && physical_sector_index_of_parent_dir != 0)
            {
                // delete_entire_from_current_node();
                printf("Chay vao -1, vi tri cha: %d\n", physical_sector_index_of_parent_dir);
                physical_sector_index_of_parent_dir =
                    print_entry_from_sector(physical_sector_index_of_parent_dir);
                // printf("Quay ve vi tri: %d", physical_sector_index_of_parent_dir);

            }
            else
            {
                if (choice > -1 )
                {
                    chosen_node = take_node_from_current_by_index(choice);
//                    printf("here");
                    if (chosen_node == NULL)
                    {
                        printf("Lua chon cua ban khong hop le\n");
                    }
                    else
                    {
                        uint32_t physical_sector_number = \
                            PHYSICAL_SECTOR_NUMBER(chosen_node->STARTING_CLUSTER_NUMBER);

                        if (chosen_node->CLUSTER_TYPE == _FOLDER)
                        {    
                            // printf("\n");
                            physical_sector_index_of_current_folder = PHYSICAL_SECTOR_NUMBER(chosen_node->STARTING_CLUSTER_NUMBER);
                            physical_sector_index_of_parent_dir = print_entry_from_sector(physical_sector_number);
                            printf("Da chon vao folder, vi tri cha la %d, vi tri cua folder la %d (neu folder trong, chi nhap -1 de quay ve)", \
                                    physical_sector_index_of_parent_dir, physical_sector_index_of_current_folder);
                        }
                        else if (chosen_node->CLUSTER_TYPE == _FILE)
                        {
                            if (physical_sector_index_of_parent_dir == 0)
                            {
                                physical_sector_index_of_parent_dir = get_physical_sector_of_root();
                            }
                            else
                            {
                                physical_sector_index_of_parent_dir = physical_sector_index_of_current_folder;
                            }
                            // int32_t result = get_entry_value_from_FAT(chosen_node->STARTING_CLUSTER_NUMBER);
                            // printf("\nThis is a cluster value: %d", result);
                            // read_file_on_multi_sector(chosen_node->STARTING_CLUSTER_NUMBER);
                            read_file_on_multi_sector(chosen_node->STARTING_CLUSTER_NUMBER);
                            printf("\nDa chon vao file co cluster bat dau la %d, vi tri cha: %d, hay nhap -1 de quay ve\n",\
                                chosen_node->STARTING_CLUSTER_NUMBER, physical_sector_index_of_parent_dir);
                        }
                    }
                }
                else
                {
                    printf("Lua chon cua ban khong hop le!!!\n");
                }
            }
        }
    }
    printf("Ket thuc chuong trinh!!!\n");
    return 0;
}
