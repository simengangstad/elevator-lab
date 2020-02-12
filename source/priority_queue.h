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
 */
void queueAddNode(Node* newNodePtr, Node* firstNodePtr);

/*	Ubrukelig, firstNode er queue-objektet
Node* queueGetFirstNode(Node* firstNodePtr);
*/

/**
 * @brief Deletes the first order in the queue, returns a pointer to the second order in the queue.
 *
 * @param[in] firstNodePtr Pointer to the first order in the queue.
 *
 * @return Pointer to the second order in the queue.
 */
Node* queuePop(Node* firstNodePtr);


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


#endif
