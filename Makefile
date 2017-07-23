LINTER_PARAMS=--extensions=hpp,cpp,h,c --filter=-legal/copyright,-build/include,-runtime/arrays,-runtime/references,-build/c++11,-build/namespaces,-runtime/explicit,-runtime/printf,-runtime/int,-readability/inheritance --linelength=120

SOURCES := $(shell find . -type f \( -name "*.cpp" -o -name "*.h" \) | grep -v "AVR-HAL" | grep -v "cmake-build-")

all: PLD_build PLD_unit_tests EPS_A_build EPS_A_unit_tests EPS_B_build EPS_B_unit_tests SunS_build SunS_unit_tests EGSE checkStyle checkFormat
.PHONY: checkStyle format checkFormat PLD_build EPS_A_build EPS_B_build PLD_unit_tests EPS_A_unit_tests EPS_B_unit_tests SunS_build SunS_unit_tests EGSE 

clean:
	rm -rf PLD/cmake-build-release
	rm -rf EPS_A/cmake-build-release
	rm -rf EPS_B/cmake-build-release
	rm -rf SunS/cmake-build-release

## -------------------------- STYLE -------------------------------------

checkStyle:
	@cpplint $(LINTER_PARAMS) $(SOURCES)

format:
	@clang-format-5.0 -i $(SOURCES)

checkFormat:
	@! clang-format-5.0 $(SOURCES) -output-replacements-xml | grep -c "<replacement " >/dev/null
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

SunS_build:
	cd SunS && mkdir -p cmake-build-release && cd cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C SunS/cmake-build-release all

## -------------------------- RUN TESTS -------------------------------------

PLD_unit_tests:
	make -C PLD/cmake-build-release unit_tests.run
	make -C PLD/cmake-build-release unit_tests_pld.run

EPS_A_unit_tests:
	make -C EPS_A/cmake-build-release unit_tests.run

EPS_B_unit_tests:
	make -C EPS_B/cmake-build-release unit_tests.run

SunS_unit_tests:
	make -C SunS/cmake-build-release unit_tests.run
	make -C SunS/cmake-build-release unit_tests_suns.run

## -------------------------- NON-FLIGHT STUFF  -------------------------------------

EGSE:
	mkdir -p EGSE/OBCMock/firmware/cmake-build-release
	cd EGSE/OBCMock/firmware/cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C EGSE/OBCMock/firmware/cmake-build-release I2C_bridge.build

EGSE-SunS:
	mkdir -p EGSE/SunS_Fixture/firmware/cmake-build-release
	cd EGSE/SunS_Fixture/firmware/cmake-build-release && cmake .. -DCMAKE_BUILD_TYPE=RELEASE
	make -C EGSE/SunS_Fixture/firmware/cmake-build-release I2C_bridge.build
