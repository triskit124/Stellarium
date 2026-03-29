TARGET    = stellarium
BUILD_DIR = build
DOCS_DIR  = doc

.DEFAULT_GOAL := all
.PHONY: all setup clean docs test

all: setup
	meson compile -C $(BUILD_DIR)
	cp $(BUILD_DIR)/compile_commands.json .

setup: $(BUILD_DIR)/build.ninja

$(BUILD_DIR)/build.ninja:
	meson setup $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(DOCS_DIR)/build compile_commands.json

docs: setup
	meson compile -C $(BUILD_DIR) docs

test: setup
	meson test -C $(BUILD_DIR) --print-errorlogs
