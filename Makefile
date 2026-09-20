CMAKE ?= cmake
BUILD_DIR ?= build
PRESET ?= debug

.PHONY: all debug release clean test run

all: debug

debug:
	$(CMAKE) --preset $(PRESET)
	$(CMAKE) --build --preset $(PRESET)

release:
	$(CMAKE) --preset release
	$(CMAKE) --build --preset release

relwithdebinfo:
	$(CMAKE) --preset relwithdebinfo
	$(CMAKE) --build --preset relwithdebinfo

test: debug
	ctest --preset debug --output-on-failure

run: debug
	./build/debug/shadowdeep

clean:
	rm -rf build

install: release
	$(CMAKE) --install build/release

fmt:
	find include src -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i || true
