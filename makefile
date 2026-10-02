SRC = src/
BUILD_DIR = build/
EXE_DIR = (BUILD_DIR)exe/
NASM_ARGS = -f elf

.PHONY: all dirs clean task1_1 task1_2 task2 task2_2

all: task1_1 task1_2 task2 task2_2

dirs:
	mkdir -p $(BUILD_DIR) $(EXE_DIR)

# Task 1_1
task1_1: $(EXE_DIR)task_1_1
	$(EXE_DIR)task_1_1: $(BUILD_DIR)task_1_1.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
		gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task_1_1.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task_1_1

	$(BUILD_DIR)task_1_1.o: $(SRC)task_1_1.asm| dirs
		nasm $(NASM_FLAGS) $(SRC)task_1_1.asm -o $(BUILD_DIR)task_1_1.o

# Task 1_2
task1_2: $(EXE_DIR)task_1_2
	$(EXE_DIR)task_1_2: $(BUILD_DIR)task_1_2.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
		gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task_1_2.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task_1_2

	$(BUILD_DIR)task_1_2.o: $(SRC)task_1_2.asm| dirs
		nasm $(NASM_FLAGS) $(SRC)task_1_2.asm -o $(BUILD_DIR)task_1_2.o

# Task_2_2
task2_2: $(EXE_DIR)task_2_2
	$(EXE_DIR)task_2_2: $(BUILD_DIR)task_2_2.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
		gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task_2_2.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task_2_2

	$(BUILD_DIR)task_2_2.o:	$(SRC)task_2_2.asm | dirs
		nasm $(NASM_FLAGS) $(SRC)task_2_2.asm -o $(BUILD_DIR)task_2_2.o

# Task_2
task2: $(EXE_DIR)task_2 
	$(EXE_DIR)task_2: $(BUILD_DIR)task_2.o $(BUILD_DIR)driver.o $(BUILD_DIR)asm_io.o
		gcc -m32 $(BUILD_DIR)driver.o $(BUILD_DIR)task_2.o $(BUILD_DIR)asm_io.o -o $(EXE_DIR)task_2

	$(BUILD_DIR)task_2.o:	$(SRC)task_2.asm | dirs
		nasm $(NASM_FLAGS) $(SRC)task_2.asm -o $(BUILD_DIR)task_2.o

# Common Build Dependencies 
$(BUILD_DIR)driver.o: | dirs
	gcc -m32 -c $(SRC)driver.c -o $(BUILD_DIR)driver.o

$(BUILD_DIR)asm_io.o: | dirs
	nasm $(NASM_FLAGS) $(SRC)asm_io.asm -o $(BUILD_DIR)asm_io.o

clean:
	rm -rf $(BUILD_DIR)