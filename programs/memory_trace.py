import struct

with open("addresses.txt", "r") as trace_file:
    trace = [int(line, 16) for line in trace_file.readlines()]

with open("memory_trace", "wb") as memory_trace_file:
    for location in trace:
        memory_trace_file.write(struct.pack("I", location))
        print(location)
