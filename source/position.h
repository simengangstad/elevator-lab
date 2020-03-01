/**
 * @file
 * @brief The position of the elevator.
 */

#ifndef POSITION_H
#define POSITION_H

/**
 * @brief Specifies where the elevator is relative to a given floor.
 */
typedef enum Offset {
    OFFSET_AT_FLOOR,
    OFFSET_BELOW,
    OFFSET_ABOVE,
    OFFSET_UNDEFINED
} Offset;

/**
 * @brief Information about where the elevator is in relation to a given floor. 
 * 
 * @note If the elevator is between the 3rd and the 2nd floor going down, this 
 *       struct would hold 3rd floor at OFFSET_BELOW from the #Offset enum.
 */
typedef struct Position {
    /**
     * @brief The floor of the position.
     */
    int floor;

    /**
     * @brief The offset of the position.
     */
    Offset offset;
} Position;

#endif