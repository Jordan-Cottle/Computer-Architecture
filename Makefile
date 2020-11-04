EXE := main.exe
SOURCE_ROOT := cpp_source
PROGRAMS := test_program test_program2

SHELL = /usr/bin/python3
.ONESHELL:

run: build $(addsuffix .bin, ${PROGRAMS})
	import os
	os.system("./${EXE}")

build: cpp_source  # cpp_source directory needs to exist
	from compile import main
	main("${SOURCE_ROOT}", "${EXE}")

%.bin: programs/%.s
	import os
	os.chdir("programs")
	
	from link import main
	main("$(basename $@).s", binary=True)
	os.replace("$@", "../$@")

clean:
	import os
	os.system("rm -rf obj")
	os.system("rm ${EXE}")
	os.system("rm *.bin")