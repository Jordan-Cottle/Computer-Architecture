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
	cache_test \
	op_test
SOURCE_ROOT := cpp_source
PROGRAMS := test_program test_program2 fpTest CPU0 CPU1 rv32i A5_CPU0 A5_CPU1 op_test
BINARIES := $(addsuffix .bin, ${PROGRAMS})
PROGRAM_DIR := programs
DATA_DIR := data
TOOLS_DIR := tools

export EVENT_ENABLED=true
export DEBUG_ENABLED=true
export INFO_ENABLED=true
export WARNING_ENABLED=true
export ERROR_ENABLED=true

VPY := ./.venv/bin/python

SHELL = /usr/bin/python3
.ONESHELL:
.PHONY=build,clean,run,tests,cpu0,duo-core,assignment4,assignment5,best_cache

assignment5: ${BINARIES} gauss-blur.exe gauss-cached.exe .venv MAT_A_DATA.dat MAT_B_DATA.dat
	import os

	# Simulation makes a LOT of noise, turn the logs off so it can run faster
	os.environ["EVENT_ENABLED"] = "false"
	os.environ["DEBUG_ENABLED"] = "false"
	os.environ["INFO_ENABLED"] = "false"

	os.system("./gauss-blur.exe")

	# Create the image files
	os.system("${VPY} ${TOOLS_DIR}/make_image.py ${DATA_DIR}/MAT_A_DATA.txt original.png")
	os.system("${VPY} ${TOOLS_DIR}/make_image.py convolution.txt convolution.png")
	os.system("${VPY} ${TOOLS_DIR}/make_image.py output.txt output.png")

	input("Cacheless blur simulation complete, press enter to continue")

	os.system("./gauss-cached.exe")
	os.system("${VPY} ${TOOLS_DIR}/make_image.py cached-convolution.txt cached-convolution.png")
	os.system("${VPY} ${TOOLS_DIR}/make_image.py cached-output.txt cached-output.png")

assignment4: ${BINARIES} cache_performance_test.exe duo-core.exe duo-cache.exe duo-optimum.exe memory_trace.dat
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

.venv: # Set up the ${VPY} virtual environtment
	import os
	os.system("python3 -m venv .venv")
	os.system("${VPY} -m pip install -r requirements.txt")

%.png: .venv  # Convert .txt data files into actual .png files
	import os
	os.system("${VPY} ${TOOLS_DIR}/make_image.py ${DATA_DIR}/$(basename $@).txt $@")

%.dat: data/%.txt  # Converty .txt files into binart .dat files for the simulations
	from tools.load_binary import main

	try:
	  main("$<", 8)
	except ValueError: # Try with base 16 and full word length
	  main("$<", 32, 16)

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
	from tools.link import main

	main("${PROGRAM_DIR}/$(basename $@).s", binary=True)
	os.replace("${PROGRAM_DIR}/$@", "$@")

%.exe: ${PROGRAM_DIR}/%.cpp ${BINARIES}
	from tools.compile import main
	main("${SOURCE_ROOT}", "$@", "$<")

best_cache: duo-optimum.exe ${BINARIES} memory_trace.dat
	from tools.find_best_cache import main
	main()

clean:
	import os
	os.system("rm -rf obj")
	os.system("rm -rf .venv")
	os.system("rm *.exe")
	os.system("rm *.bin")
	os.system("rm *.json")
	os.system("rm *.png")
	os.system("rm *.jpg")
	os.system("rm *.dat")
	os.system("rm output.txt")
	os.system("rm convolution.txt")