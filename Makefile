SIM := true 

SOURCES := fsm.c priority_queue.c door.c
TEST_SOURCES := main_tests.c test_util.c door.c door_tests.c priority_queue.c fsm.c fsm_tests.c

SOURCE_DIR := source
TEST_DIR := source/tests

BUILD_DIR := build

OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
TEST_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

DRIVER_ARCHIVE := $(BUILD_DIR)/libdriver.a

ifeq ($(SIM), true)
  DRIVER_SOURCE := hardware_sim.c
else
  DRIVER_SOURCE := hardware.c io.c
endif

CC := gcc
CFLAGS := -O0 -g3 -Wall -Werror -D_GNU_SOURCE -std=c11 -I$(SOURCE_DIR)

ifeq ($(SIM), true)
  LDFLAGS := -L$(BUILD_DIR) -ldriver 
else
  LDFLAGS := -L$(BUILD_DIR) -ldriver -lcomedi
endif

.DEFAULT_GOAL := elevator

elevator : $(OBJ) | $(DRIVER_ARCHIVE)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

tests: $(TEST_OBJ) | $(DRIVER_ARCHIVE)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


$(BUILD_DIR) :
	mkdir -p $@/driver

$(BUILD_DIR)/%.o : $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/driver/%.o : $(SOURCE_DIR)/driver/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DRIVER_ARCHIVE) : $(DRIVER_SOURCE:%.c=$(BUILD_DIR)/driver/%.o)
	ar rcs $@ $^

.PHONY: clean
clean :
	rm -rf $(BUILD_DIR) elevator tests
