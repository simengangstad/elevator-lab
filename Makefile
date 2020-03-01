SOURCES := main.c fsm.c priority_queue.c door.c 

SOURCE_DIR := source
BUILD_DIR := build

OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

TESTS_ARCHIVE := $(BUILD_DIR)/libtests.a
TESTS_SOURCE := unit_tests.c test_util.c door_tests.c fsm_tests.c priority_queue_tests.c

DRIVER_ARCHIVE := $(BUILD_DIR)/libdriver.a

DRIVER_SOURCE := hardware.c io.c

CC := gcc
CFLAGS := -O0 -g3 -Wall -Werror -D_GNU_SOURCE -std=c11 -I$(SOURCE_DIR)

LDFLAGS := -L$(BUILD_DIR) -ldriver -ltests -lcomedi

.DEFAULT_GOAL := elevator

elevator : $(OBJ) | $(DRIVER_ARCHIVE) $(TESTS_ARCHIVE)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR) :
	mkdir -p $@/driver
	mkdir -p $@/tests

$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o : $(SOURCE_DIR)/tests/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTS_ARCHIVE) : $(TESTS_SOURCE:%.c=$(BUILD_DIR)/tests/%.o)
	ar rcs $@ $^

$(BUILD_DIR)/driver/%.o : $(SOURCE_DIR)/driver/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DRIVER_ARCHIVE) : $(DRIVER_SOURCE:%.c=$(BUILD_DIR)/driver/%.o)
	ar rcs $@ $^

.PHONY: clean
clean :
	rm -rf $(BUILD_DIR) elevator
