TARGET = stellarium
BUILDDIR = build
DOCSDIR = doc
DOXY_FILE = doxygen

# number of parallel jobs for builds
j ?= 10

.PHONY: clean docs run test

.vscode:
	mkdir -p .vscode

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(DOCSDIR):
	mkdir -p $(DOCSDIR)

all: $(BUILDDIR) .vscode
	cd $(BUILDDIR); \
	cmake ..; \
	cmake --build . -j ${j}; \
	cp compile_commands.json ../.vscode/

all-no-graphics: $(BUILDDIR) .vscode
	cd $(BUILDDIR); \
	cmake -DBUILD_RENDERING=OFF ..; \
	cmake --build . -j ${j}; \
	cp compile_commands.json ../.vscode/

clean:
	rm -rf $(BUILDDIR) $(DOCSDIR)

docs: $(DOCSDIR)
	doxygen $(DOXY_FILE)

run:
	./$(BUILDDIR)/$(TARGET)

test:
	test/run_tests.sh
