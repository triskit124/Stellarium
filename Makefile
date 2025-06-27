TARGET = stellarium
BUILDDIR = build
DOCSDIR = doc
DOXY_FILE = doxygen

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
	cmake --build .; \
	cp compile_commands.json ../.vscode/

all-no-graphics: $(BUILDDIR) .vscode
	cd $(BUILDDIR); \
	cmake -DBUILD_RENDERING=OFF ..; \
	cmake --build .; \
	cp compile_commands.json ../.vscode/

clean:
	rm -rf $(BUILDDIR) $(DOCSDIR)

docs: $(DOCSDIR)
	doxygen $(DOXY_FILE)

run:
	./$(BUILDDIR)/$(TARGET)

test:
	test/run_tests.sh
