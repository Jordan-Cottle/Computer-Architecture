EXE := main

run: build
	./${EXE}

build: cpp_source  # cpp_source directory needs to exist
	./compile.py ${EXE}

clean:
	rm -rf obj