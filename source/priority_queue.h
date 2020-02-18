/**
 * @file
 * @brief Library containing PriorityQueue struct and dependencies, including Node struct.
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "hardware.h"

/**
 * @brief Structure to represent an order in the queue.
 */
typedef struct {
	Node* nextNode;
	int floor;
	HardwareOrder direction;
} Node;

/**
 * @brief Creates a node object representing an order by allocating space on the heap.
 *
 * @param[in] newFloor Floor of the new order.
 * @param[in] newDirection Direction of the new order.
 * /warning{The nextNode-parameter of the new node is always NULL.}
 *
 * @return Pointer to the newly created node.
 */

Node* nodeCreate(int newFloor, HardwareOrder newDirection);

/**
 * @brief Deletes the single and frees the allocated space in memory.
 *
 * /warning{Does not take account of linked nodes, beware of dangling pointers.}
 *
 * @param[in] nodePtr Pointer to node to delete.
 */

void nodeDelete(Node* nodePtr);

/**
 * @brief Returns the floor of a given order.
 * 
 * @param[in] nodePtr Pointer to order to get floor of.
 *
 * @return Floor of the order.
 */
int nodeGetFloor(Node* nodePtr);

/**
 * @brief Returns the direction of a given order.
 *
 * @param[in] nodePtr Pointer to order to get direction of.
 *
 * @return Direction of the order of the type HardwareOrder. A return of HARDWARE_ORDER_INSIDE means the order is directionless (meaning it is called from inside the elevator.
 */
HardwareOrder nodeGetDirection(Node* nodePtr);


/**
 * @brief Adds an order based on a prioritation algorithm. If the queue contains duplicate orders, the lowest priority is deleted.
 *
 * @param[in] newNodePtr Pointer to the order to add to the queue.
 * @param[in] firstNodePtr Pointer to the first order in the queue.
 * @param[in] currentFloor Current floor.
 *
 * @return Returns a pointer to the start of the queue. Returns @p firstNodePtr if @p newNodePtr is NULL.
 */
Node* queueAddNode(Node* newNodePtr, Node* firstNodePtr, int currentFloor);

/*	Ubrukelig, firstNode er queue-objektet
Node* queueGetFirstNode(Node* firstNodePtr);
*/

/**
 * @brief Deletes the first order in the queue, returns a pointer to the second order in the queue.
 *
 * @param[in] firstNodePtr Pointer to the first order in the queue.
 * @param[in] currentFloor Current floor of the elevator.
 *
 * @return Pointer to the second order in the queue.
 */
Node* queuePop(Node* firstNodePtr, int currentFloor);


/**
 * @brief Removes every order in the queue.
 *
 * @param[in] firstNodePtr Pointer to the first order in the queue.
 */
void queueClear(Node* firstNodePtr);

/**
 * @brief Returns true if the queue is empty.
 *
 * @param[in] firstNodePtr Pointer to the first order in the queue.
 *
 * @return True if the queue is empty.
 */
bool queueIsEmpty(Node* firstNodePtr);

/**
 * @brief Prints the queue to terminal, including the order floors and directions.
 *
 * @param[in] firstNodePtr Pointer to first node in queue.
 */
void queuePrint(Node* firstNodePtr);

#endif
