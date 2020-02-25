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

Node *queue_add_node(Node *p_new_node, const Node *p_first_node, const int current_floor) {
    if (!p_new_node) {
        return p_first_node;
    }

    Node *p_new_first_node = NULL;

    // Algorithm for adding node to queue
    if (queue_is_empty(p_first_node)) {
        p_new_node->next_node = NULL;
        p_new_first_node = p_new_node;
    } else {
        // If the new order is on the way to the destination, put it at the top of the queue
        if (((current_floor < p_new_node->floor && p_new_node->floor < p_first_node->floor) ||
             (current_floor > p_new_node->floor && p_new_node->floor > p_first_node->floor)) &&
            (p_new_node->direction == HARDWARE_ORDER_INSIDE || p_new_node->direction == p_first_node->direction)) {
            p_new_node->next_node = p_first_node;
            p_new_first_node = p_new_node;
        }
        // If the new order is not on the way to the destination, put it at the bottom of the queue
        else {
            p_new_node->next_node = NULL;
            p_new_first_node = p_first_node;

            // Traverse the queue and place the new order at the bottom
            Node *p_current_node = p_new_first_node;
            while (p_current_node->next_node) {
                p_current_node = p_current_node->next_node;
            }
            p_current_node->next_node = p_new_node;
        }

        // Traverse queue, delete orders to duplicate floors
        bool orderOnFloor[4] = {false, false, false, false};
        Node *currentNodePtr = newFirstNodePtr;
        Node *prevNodePtr = NULL;

        while (currentNodePtr) {
            if (orderOnFloor[currentNodePtr->floor]) {
                prevNodePtr->nextNode = currentNodePtr->nextNode;
                free(currentNodePtr);
                currentNodePtr = prevNodePtr->nextNode;
            } else {
                orderOnFloor[currentNodePtr->floor] = true;
                prevNodePtr = currentNodePtr;
                currentNodePtr = currentNodePtr->nextNode;
            }
        }
    }

    return newFirstNodePtr;
}

Node *queuePop(Node *firstNodePtr, int currentFloor) {
    if (!firstNodePtr) {
        return NULL;
    }

    if (!firstNodePtr->nextNode) {
        // TODO: should be unnecessary, test this
        free(firstNodePtr);
        return NULL;
    }

    Node *newFirstNodePtr = firstNodePtr->nextNode;
    free(firstNodePtr);

    return newFirstNodePtr;
}

Node *queueClear(Node *firstNodePtr) {
    Node *traverseNodePtr = firstNodePtr;
    while (traverseNodePtr) {
        Node *tempNodePtr = traverseNodePtr;
        traverseNodePtr = traverseNodePtr->nextNode;
        free(tempNodePtr);
    }

    return NULL;
}

bool queueIsEmpty(const Node *firstNodePtr) { return !firstNodePtr; }

void queuePrint(Node *firstNodePtr) {
    Node *tempNodePtr = firstNodePtr;
    int n = 1;
    while (tempNodePtr) {
        printf("%i", n);
        printf(". order\n");
        printf("Pointer address: %p\n", tempNodePtr);
        printf("Next pointer address: %p\n", tempNodePtr->nextNode);
        printf("\t Floor: %i", tempNodePtr->floor);
        printf("\n");
        printf("\t Direction: %i", (int)tempNodePtr->direction);
        printf("\n");
        n++;

        tempNodePtr = tempNodePtr->nextNode;
    }
    printf("End of queue\n");
    return;
}
