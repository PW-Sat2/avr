LINTER_PARAMS=--extensions=hpp,cpp,h,c --filter=-legal/copyright,-build/include,-runtime/arrays,-runtime/references,-build/c++11,-build/namespaces,-runtime/explicit,-runtime/printf,-runtime/int,-readability/inheritance --linelength=120

SOURCES := $(shell find . -type f \( -name "*.cpp" -o -name "*.h" \) | grep -v "AVR-HAL" | grep -v "cmake-build-")

all: PLD_build PLD_unit_tests EPS_A_build EPS_A_unit_tests EPS_B_build EPS_B_unit_tests EGSE checkStyle checkFormat
.PHONY: checkStyle format checkFormat PLD_build EPS_A_build EPS_B_build PLD_unit_tests EPS_A_unit_tests EPS_B_unit_tests EGSE 

clean:
	rm -rf PLD/cmake-build-release
	rm -rf EPS_A/cmake-build-release
	rm -rf EPS_B/cmake-build-release

## -------------------------- STYLE -------------------------------------

checkStyle:
	@cpplint $(LINTER_PARAMS) $(SOURCES)

format:
	clang-format -i $(SOURCES)

checkFormat:
	@! clang-format $(SOURCES) -output-replacements-xml | grep -c "<replacement " >/dev/null
	@echo "Format OK"

## -------------------------- BUILD PROJECTS -------------------------------------

PLD_build:
	cd PLD && mkdir -p cmake-build-release && cd cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C PLD/cmake-build-release all

EPS_A_build:
	cd EPS_A && mkdir -p cmake-build-release && cd cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C EPS_A/cmake-build-release all

EPS_B_build:
	cd EPS_B && mkdir -p cmake-build-release && cd cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C EPS_B/cmake-build-release all

## -------------------------- RUN TESTS -------------------------------------

PLD_unit_tests:
	make -C PLD/cmake-build-release unit_tests.run
	make -C PLD/cmake-build-release unit_tests_pld.run

EPS_A_unit_tests:
	make -C EPS_A/cmake-build-release unit_tests.run

EPS_B_unit_tests:
	make -C EPS_B/cmake-build-release unit_tests.run


## -------------------------- NON-FLIGHT STUFF  -------------------------------------

EGSE:
	mkdir -p EGSE/I2C_bridge/firmware/cmake-build-release
	cd EGSE/I2C_bridge/firmware/cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C EGSE/I2C_bridge/firmware/cmake-build-release I2C_bridge.build




