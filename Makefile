TARGET = Stellarium
BUILDDIR = build
DOCSDIR = doc
DOXY_FILE = doxygen

.PHONY: clean docs run test


$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(DOCSDIR):
	mkdir -p $(DOCSDIR)

all: $(BUILDDIR)
	cd $(BUILDDIR); \
	cmake ..; \
	cmake --build .

clean:
	rm -rf $(BUILDDIR) $(DOCSDIR)

docs: $(DOCSDIR)
	doxygen $(DOXY_FILE)

run:
	./$(BUILDDIR)/$(TARGET)

test:
	cd test; ./run_tests.sh
