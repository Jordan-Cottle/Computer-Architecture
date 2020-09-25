.PHONY: .cpp .python .clean

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
python: ${PYTHON_DIR}/event_queue.py
	python3 ${PYTHON_DIR}/event_queue.py

clean :
	rm *.o

%.o : ${SOURCE_DIR}/%.cpp
	${GCC} -c -o $@ $<

${TARGET}: ${OBJECTS}
	${GCC} -c -o $@.o ${SOURCE_DIR}/$@.cpp
	${GCC} -o $@ $^ $@.o