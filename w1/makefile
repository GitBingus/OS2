SRC = w1/src/
BUILD_DIR = build/
EXE_DIR = $(BUILD_DIR)exe/

NASM_FLAGS = -f elf -I$(SRC)

.PHONY: all dirs clean 

all: task1_1 task1_2 task2 task2_2

dirs:
	mkdir -p $(BUILD_DIR) $(EXE_DIR)


# =========================
# Task 1_1
# =========================

task1_1: $(EXE_DIR)task1_1

$(EXE_DIR)task1_1: $(BUILD_DIR)task1_1.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
	gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task1_1.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task1_1

$(BUILD_DIR)task1_1.o: $(SRC)task1_1.asm | dirs
	nasm $(NASM_FLAGS) $< -o $@


# =========================
# Task 1_2
# =========================

task1_2: $(EXE_DIR)task1_2

$(EXE_DIR)task1_2: $(BUILD_DIR)task1_2.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
	gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task1_2.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task1_2

$(BUILD_DIR)task1_2.o: $(SRC)task1_2.asm | dirs
	nasm $(NASM_FLAGS) $< -o $@


# =========================
# Task 2_2
# =========================

task2_2: $(EXE_DIR)task2_2

$(EXE_DIR)task2_2: $(BUILD_DIR)task2_2.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
	gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task2_2.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task2_2

$(BUILD_DIR)task2_2.o: $(SRC)task2_2.asm | dirs
	nasm $(NASM_FLAGS) $< -o $@


# =========================
# Task 2
# =========================

task2: $(EXE_DIR)task2

$(EXE_DIR)task2: $(BUILD_DIR)task2.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
	gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task2.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task2

$(BUILD_DIR)task2.o: $(SRC)task2.asm | dirs
	nasm $(NASM_FLAGS) $< -o $@


# =========================
# Common dependencies
# =========================

$(BUILD_DIR)driver.o: $(SRC)driver.c | dirs
	gcc -m32 -c $< -o $@

$(BUILD_DIR)asm_io.o: $(SRC)asm_io.asm | dirs
	nasm $(NASM_FLAGS) $< -o $@


# =========================
# Clean
# =========================

clean:
	rm -rf $(BUILD_DIR)
