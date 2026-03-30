TARGET    = stellarium
BUILD_DIR = build

.DEFAULT_GOAL := all
.PHONY: all setup clean test

all: setup
	meson compile -C $(BUILD_DIR)
	cp $(BUILD_DIR)/compile_commands.json .

setup: $(BUILD_DIR)/build.ninja

$(BUILD_DIR)/build.ninja:
	meson setup $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) compile_commands.json

test: setup
	meson test -C $(BUILD_DIR) --print-errorlogs
