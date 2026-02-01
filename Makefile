TARGET = stellarium
BUILD_DIR = build
DOCS_DIR = doc

CMAKE_FILES := $(shell find . -path ./$(BUILD_DIR) -prune -o -name "CMakeLists.txt")

# number of parallel jobs for builds
j ?= 10

.PHONY: all clean docs test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all: $(BUILD_DIR)/CMakeFiles
	cd $(BUILD_DIR); \
	cmake --build . -j ${j}
	cp $(BUILD_DIR)/compile_commands.json .

$(BUILD_DIR)/CMakeFiles: $(BUILD_DIR) $(CMAKE_FILES)
	cd $(BUILD_DIR); \
	cmake ..

clean:
	rm -rf $(BUILD_DIR) $(DOCS_DIR)/build compile_commands.json

docs: $(BUILD_DIR)/CMakeFiles
	cd $(BUILD_DIR); \
	cmake --build . --target docs -j ${j}
	
test:
	test/run_tests.sh
