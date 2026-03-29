TARGET    = stellarium
BUILD_DIR = build
DOCS_DIR  = doc

.PHONY: all clean docs test

$(BUILD_DIR)/build.ninja:
	meson setup $(BUILD_DIR)

all: $(BUILD_DIR)/build.ninja
	meson compile -C $(BUILD_DIR)
	cp $(BUILD_DIR)/compile_commands.json .

clean:
	rm -rf $(BUILD_DIR) $(DOCS_DIR)/build compile_commands.json

docs: $(BUILD_DIR)/build.ninja
	meson compile -C $(BUILD_DIR) docs

test: $(BUILD_DIR)/build.ninja
	meson test -C $(BUILD_DIR) --print-errorlogs
