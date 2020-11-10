EXE := duo-core
TESTS:= fetch_test \
	fp_test \
	decode_test \
	execute_test \
	store_test \
	cpu_test \
	memory_test \
	binary_test \
	opcode_test \
	memory_bus_test \
	rv32i_test
SOURCE_ROOT := cpp_source
PROGRAMS := test_program test_program2 fpTest CPU0 CPU1 rv32i
BINARIES := $(addsuffix .bin, ${PROGRAMS})
PROGRAM_DIR := programs

SHELL = /usr/bin/python3
.ONESHELL:
.PHONY=build,clean,run,tests,cpu0,duo-core

assignment: ${BINARIES} cpu0.exe duo-core.exe
	# Both simulations complete
	import os

	# Start assignment3 part 1
	os.system("./cpu0.exe")
	input("Part 1 complete, press enter to continue")
	# Start assignment3 part 2
	os.system("./duo-core.exe")

run: ${EXE}.exe ${BINARIES}
	import os
	os.system("./$<")

cpu0: cpu0.exe CPU0.bin
	import os
	os.system("./cpu0.exe")

duo-core: duo-core.exe CPU0.bin CPU1.bin
	import os
	os.system("./duo-core.exe")

tests: ${BINARIES} ${TESTS} 
	# All tests have been triggered

%_test: %_test.exe ${BINARIES}
	import os
	assert os.system(f"./$<") == 0, "$@ has failed!"

%.bin: ${PROGRAM_DIR}/%.s
	import os
	os.chdir("${PROGRAM_DIR}")
	
	from link import main
	main("$(basename $@).s", binary=True)
	os.replace("$@", "../$@")

%.exe: ${PROGRAM_DIR}/%.cpp
	from compile import main
	main("${SOURCE_ROOT}", "$@", "$<")

clean:
	import os
	os.system("rm -rf obj")
	os.system("rm *.exe")
	os.system("rm *.bin")