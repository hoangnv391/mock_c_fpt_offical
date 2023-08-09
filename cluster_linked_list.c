#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "cluster_linked_list.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
CLUSTER_NODE_Typedef *init_node(uint16_t cluster_number, uint8_t cluster_type);
void add_at_tail(CLUSTER_NODE_Typedef *node);
void print_linked_list();
void set_current_node(CLUSTER_NODE_Typedef *node);
void show_current_node();
void delete_entire_from_current_node();

/*******************************************************************************
 * Variables
 ******************************************************************************/
static CLUSTER_NODE_Typedef *s_head = NULL;
static CLUSTER_NODE_Typedef *s_tail = NULL;
static CLUSTER_NODE_Typedef *s_current_node = NULL;
static CLUSTER_NODE_Typedef *s_prev_current_node = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/
CLUSTER_NODE_Typedef *init_node(uint16_t cluster_number, uint8_t cluster_type)
{
    CLUSTER_NODE_Typedef *new_node = (CLUSTER_NODE_Typedef *)malloc(sizeof(CLUSTER_NODE_Typedef));
    if (new_node != NULL)
    {
        new_node->CLUSTER_TYPE = cluster_type;
        new_node->STARTING_CLUSTER_NUMBER = cluster_number;
        new_node->NEXT = NULL;
    }
    return new_node;
}

void add_at_tail(CLUSTER_NODE_Typedef* node)
{
    if (s_head == NULL)
    {
        s_head = node;
        s_tail = node;
    }
    else
    {
        s_tail->NEXT = node;
        s_tail = node;
    }
}

void print_linked_list()
{
    if (s_head == NULL)
    {
        printf("Linked list trong!!!");
    }
    else
    {
        CLUSTER_NODE_Typedef *temp = s_head;
        while (temp != NULL)
        {
            printf("Starting cluster number: %d\n", temp->STARTING_CLUSTER_NUMBER);
            temp = temp->NEXT;
        }
        
    }
}

void set_current_node(CLUSTER_NODE_Typedef* node)
{
    if (s_tail != NULL)
    {
        s_prev_current_node = s_tail;
//        printf("Nhay vao day\n");
    }
    else
    {
        s_prev_current_node = NULL;
    }
    s_current_node = node;
//    printf("Chay het r\n");
    return;
}

void show_current_node()
{
    printf("Starting cluster number of current node is %d, type is %d", \
        s_current_node->STARTING_CLUSTER_NUMBER, s_current_node->CLUSTER_TYPE);
}

CLUSTER_NODE_Typedef* take_node_from_current_by_index(uint32_t index)
{
    CLUSTER_NODE_Typedef *temp = s_current_node;
    while (temp != NULL && index > 0)
    {
        temp = temp->NEXT;
        index--;
    }
    // if (temp != NULL)
    // {
    // printf("Your choice - starting cluster number is %d and type is %d",\
    //  temp->STARTING_CLUSTER_NUMBER);
    // }
    return temp;
    // printf("Choice choice: %d", temp->STARTING_CLUSTER_NUMBER);
}

void delete_entire_from_current_node()
{
    if (s_current_node != NULL)
    {
        CLUSTER_NODE_Typedef* delete_node = s_current_node;
        CLUSTER_NODE_Typedef *temp = NULL;
        while (delete_node != NULL)
        {
            temp = delete_node->NEXT;
            free(delete_node);
            delete_node = temp;
        }
        s_current_node = NULL;
    }
//    printf("Qua cho nay");
    if (s_prev_current_node != NULL)
    {
        s_tail = s_prev_current_node;
    }
//    printf("Chay het nay");
    return;
}
