#ifndef CLUSTER_LINKED_LIST_H
    #define CLUSTER_LINKED_LIST_H

    #include <stdint.h>

    #pragma pack(1)
    /*******************************************************************************
     * Definitions
     ******************************************************************************/
    typedef struct CLUSTER_NODE
    {
        uint8_t CLUSTER_TYPE;
        uint16_t STARTING_CLUSTER_NUMBER;
        struct CLUSTER_NODE *NEXT;
    } CLUSTER_NODE_Typedef;

    #pragma pack()

    /*******************************************************************************
     * API
     ******************************************************************************/
    CLUSTER_NODE_Typedef *init_node(uint16_t cluster_number, uint8_t cluster_type);
    void add_at_tail(CLUSTER_NODE_Typedef *node);
    void print_linked_list();
    void set_current_node(CLUSTER_NODE_Typedef *node);
    void show_current_node();
    CLUSTER_NODE_Typedef *take_node_from_current_by_index(uint32_t index);
    void delete_entire_from_current_node();

#endif /* CLUSTER_LINKED_LIST_H */