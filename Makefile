EXE := main.exe
PROGRAMS := test_program.s

run: build link
	./${EXE}

build: cpp_source  # cpp_source directory needs to exist
	python3 compile.py ${EXE}

link: programs
	cd programs && python3 link.py ${PROGRAMS}

clean:
	rm -rf obj
	rm ${EXE}