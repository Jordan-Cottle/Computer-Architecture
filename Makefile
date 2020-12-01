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
	rv32i_test \
	cache_test
SOURCE_ROOT := cpp_source
PROGRAMS := test_program test_program2 fpTest CPU0 CPU1 rv32i A5_CPU0 A5_CPU1
BINARIES := $(addsuffix .bin, ${PROGRAMS})
PROGRAM_DIR := programs

SHELL = /usr/bin/python3
.ONESHELL:
.PHONY=build,clean,run,tests,cpu0,duo-core

assignment: ${BINARIES} cache_performance_test.exe duo-core.exe duo-cache.exe duo-optimum.exe
	import os

	# Start assignment4 part 1
	os.system("./cache_performance_test.exe")

	input("Part 1 complete, press enter to continue")

	# Start assignment4 part 2

	# No Cache
	os.system("./duo-core.exe")
	input("No cache simulation complete, press enter to continue")

	# Provided Cache
	os.system("./duo-cache.exe")
	input("Provided cache simulation complete, press enter to continue")

	# Provided Cache
	os.system("./duo-optimum.exe 256 512 128 1 4 best_results.json")
	input("Optimum cache simulation complete, press enter to continue")

run: ${EXE}.exe ${BINARIES}
	import os
	os.system("./$<")

cpu0: cpu0.exe CPU0.bin
	import os
	os.system("./cpu0.exe")

duo-%: CPU0.bin CPU1.bin duo-%.exe
	import os
	os.system("./$@.exe")

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

%.exe: ${PROGRAM_DIR}/%.cpp ${BINARIES}
	from compile import main
	main("${SOURCE_ROOT}", "$@", "$<")

best_cache: duo-optimum.exe CPU0.bin CPU1.bin
	from find_best_cache import main
	main()


clean:
	import os
	os.system("rm -rf obj")
	os.system("rm *.exe")
	os.system("rm *.bin")
	os.system("rm *.json")