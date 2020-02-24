/**
 * @file
 * @brief Convenience functions for testing.
 */

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>

/**
 * @brief Will block until enter key is pressed.
 */
void testUtilWaitUntilEnterKeyIsPressed() {
	while(1) {
		char ch = fgetc(stdin);
		
		if (ch == 0x0A) {
			break;
		}

		// Read dummy character to clear input buffer, which inserts after character input
		ch = getchar();
	}
}

#endif
