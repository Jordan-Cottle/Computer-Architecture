.PHONY: .cpp .python .clean .test_python

SOURCE_DIR:= ./cpp_source
PYTHON_DIR:= ./python_solution

OBJECTS := event.o event_queue.o
TARGET := main
GCC:= g++

# The default goal is to create the event_queue executable (and clean up afterwards), but not run it
all: ${TARGET} clean

# Call "make cpp" To compile and execute the c++ solution
cpp: ${TARGET} clean
	./${TARGET}

# Call "make python" to execute the python solution
python:
	python3 ${PYTHON_DIR}/main.py

test_python:
	./run_python_tests.sh

clean :
	rm *.o

%.o : ${SOURCE_DIR}/%.cpp
	${GCC} -c -o $@ $<

${TARGET}: ${OBJECTS}
	${GCC} -c -o $@.o ${SOURCE_DIR}/$@.cpp
	${GCC} -o $@ $^ $@.o