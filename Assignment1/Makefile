.PHONY: .cpp .python .clean

HEADER_DIR:= ./headers
SOURCE_DIR:= ./cpp_source
PYTHON_DIR:= ./python_solution

GCC:= g++ -iquote ${HEADER_DIR}

# The default goal is to create the event_queue executable (and clean up afterwards), but not run it
all: event_queue clean

# Call "make cpp" To compile and execute the c++ solution
cpp: event_queue clean
	./event_queue

# Call "make python" to execute the python solution
python: ${PYTHON_DIR}/event_queue.py
	python3 ${PYTHON_DIR}/event_queue.py

clean :
	rm *.o
	rm ${HEADER_DIR}/*.gch

event_queue: main.o event_queue.o event.o
	${GCC} -o event_queue main.o event_queue.o event.o

main.o : ${SOURCE_DIR}/main.cpp event_queue.o event.o
	${GCC} -c ${SOURCE_DIR}/main.cpp

event_queue.o : ${SOURCE_DIR}/event_queue.cpp event.o heap.h.gch
	${GCC} -c ${SOURCE_DIR}/event_queue.cpp

event.o : ${SOURCE_DIR}/event.cpp
	${GCC} -c ${SOURCE_DIR}/event.cpp

heap.h.gch : ${HEADER_DIR}/heap.h
	${GCC} -c ${HEADER_DIR}/heap.h