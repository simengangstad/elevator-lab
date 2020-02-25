/**
 * @file
 * @brief Implementation of the Priority Queue.
 */

#include "priority_queue.h"

Node *node_create(const int floor, const HardwareOrder direction) {
    Node *p_new_node = NULL;
    p_new_node = (Node *)malloc(sizeof(Node));

    p_new_node->floor = floor;
    p_new_node->direction = direction;
    p_new_node->next_node = NULL;

    return p_new_node;
}

Node *queue_add_node(Node *p_new_node, Node *p_first_node_of_queue, const int current_floor) {
    if (!p_new_node) {
        return p_first_node_of_queue;
    }

    Node *p_new_first_node_of_queue = NULL;

    // Algorithm for adding node to queue
    if (queue_is_empty(p_first_node_of_queue)) {
        p_new_first_node_of_queue = p_new_node;
    } else {
        // If the new order is on the way to the destination, put it at the top of the queue
        if (((current_floor < p_new_node->floor && p_new_node->floor < p_first_node_of_queue->floor) ||
             (current_floor > p_new_node->floor && p_new_node->floor > p_first_node_of_queue->floor)) &&
            (p_new_node->direction == HARDWARE_ORDER_INSIDE || p_new_node->direction == p_first_node_of_queue->direction)) {
            p_new_node->next_node = p_first_node_of_queue;
            p_new_first_node_of_queue = p_new_node;
        }
        // If the new order is not on the way to the destination, put it at the bottom of the queue
        else {
            p_new_node->next_node = NULL;
            p_new_first_node_of_queue = p_first_node_of_queue;

            // Traverse the queue and place the new order at the bottom
            Node *p_current_node = p_new_first_node_of_queue;
            while (p_current_node->next_node) {
                p_current_node = p_current_node->next_node;
            }
            p_current_node->next_node = p_new_node;
        }

        // Traverse queue, delete orders to duplicate floors
        bool order_on_floor[4] = {false, false, false, false};
        Node *p_current_node = p_new_first_node_of_queue;
        Node *p_previous_node = NULL;

        while (p_current_node) {
            if (order_on_floor[p_current_node->floor]) {
                p_previous_node->next_node = p_current_node->next_node;
                free(p_current_node);
                p_current_node = p_previous_node->next_node;
            } else {
                order_on_floor[p_current_node->floor] = true;
                p_previous_node = p_current_node;
                p_current_node = p_current_node->next_node;
            }
        }
    }

    return p_new_first_node_of_queue;
}

Node *queue_pop(Node *p_first_node_of_queue, int current_floor) {
    if (!p_first_node_of_queue) {
        return NULL;
    }

    if (!p_first_node_of_queue->next_node) {
        // TODO: should be unnecessary, test this
        free(p_first_node_of_queue);
        return NULL;
    }

    Node *p_new_first_node_of_queue = p_first_node_of_queue->next_node;
    free(p_first_node_of_queue);

    return p_new_first_node_of_queue;
}

Node *queue_clear(Node *p_first_node_of_queue) {
    Node *p_traverse_node = p_first_node_of_queue;
    while (p_traverse_node) {
        Node *p_temp_node = p_traverse_node;
        p_traverse_node = p_traverse_node->next_node;
        free(p_temp_node);
    }

    return NULL;
}

bool queue_is_empty(const Node *p_first_node_of_queue) { return !p_first_node_of_queue; }

void queue_print(const Node *p_first_node_of_queue) {
    Node *p_temp_node = p_first_node_of_queue;
    int n = 1;
    while (p_temp_node) {
        printf("%i", n);
        printf(". order\n");
        printf("Pointer address: %p\n", p_temp_node);
        printf("Next pointer address: %p\n", p_temp_node->next_node);
        printf("\t Floor: %i", p_temp_node->floor);
        printf("\n");
        printf("\t Direction: %i", (int)p_temp_node->direction);
        printf("\n");
        n++;

        p_temp_node = p_temp_node->next_node;
    }
    printf("End of queue\n");
    return;
}
