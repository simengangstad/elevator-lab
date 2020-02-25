#include "test_util.h"

void testUtilWaitUntilEnterKeyIsPressed() {
    while (1) {
        char ch = fgetc(stdin);

        if (ch == 0x0A) {
            break;
        }

        // Read dummy character to clear input buffer, which inserts after character input
        ch = getchar();
    }
}