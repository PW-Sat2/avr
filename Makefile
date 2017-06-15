LINTER_PARAMS=--extensions=hpp,cpp,h,c --filter=-legal/copyright,-build/include,-runtime/arrays,-runtime/references,-build/c++11,-build/namespaces,-runtime/explicit,-runtime/printf,-runtime/int,-readability/inheritance --linelength=120

SOURCES := $(shell find . -type f \( -name "*.cpp" -o -name "*.h" \) | grep -v "AVR-HAL" | grep -v "cmake-build-")

all: checkStyle checkFormat

clean:
	rm -rf tmp

tmp:
	mkdir -p tmp

tmp/cpplint.py: tmp
	wget https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py -O tmp/cpplint.py

checkStyle: tmp/cpplint.py
	@python tmp/cpplint.py $(LINTER_PARAMS) $(SOURCES)

format: force
	clang-format -i $(SOURCES)

checkFormat:
	@! clang-format $(SOURCES) -output-replacements-xml | grep -c "<replacement " >/dev/null
	@echo "Format OK"

PLD_build:
	cd PLD && mkdir -p cmake-build-debug && cd cmake-build-debug && cmake ..
	make -C PLD/cmake-build-debug all

PLD_unit_tests: force
	make -C PLD/cmake-build-debug unit_tests.build
	make -C PLD/cmake-build-debug unit_tests.run
	make -C PLD/cmake-build-debug unit_tests_pld.build
	make -C PLD/cmake-build-debug unit_tests_pld.run

EGSE: force
	mkdir -p EGSE/I2C_bridge/firmware/cmake-build-release
	cd EGSE/I2C_bridge/firmware/cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C EGSE/I2C_bridge/firmware/cmake-build-release I2C_bridge.build

Jenkins: clean PLD_build PLD_unit_tests EGSE checkStyle checkFormat


force:

