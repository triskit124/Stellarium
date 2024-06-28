TARGET = Stellarium
BUILDDIR = build

.PHONY: clean docs run test


$(BUILDDIR):
	mkdir -p $(BUILDDIR)

all: $(BUILDDIR)
	cd $(BUILDDIR); \
	cmake ..; \
	cmake --build .

clean:
	rm -rf $(BUILDDIR)

docs:
	doxygen $(DOXY_FILE)

run:
	./$(BUILDDIR)/$(TARGET)

test:
	cd test; ./run_tests.sh
