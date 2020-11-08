EXE := main
TESTS:= fetch_test fp_test decode_test
SOURCE_ROOT := cpp_source
PROGRAMS := test_program test_program2 fpTest CPU0 CPU1
PROGRAM_DIR := programs

SHELL = /usr/bin/python3
.ONESHELL:
.PHONY=build,clean,run,test

run: ${EXE}.exe $(addsuffix .bin, ${PROGRAMS})
	import os
	os.system("./$<")

tests: ${TESTS}
	# All tests have been triggered

%_test: %_test.exe
	import os
	os.system(f"./$@.exe")

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