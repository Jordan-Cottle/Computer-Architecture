import sys
import struct

file_name = sys.argv[1]
base = int(sys.argv[2])

with open(file_name, "r") as data_file:
    data = [int(line, base) for line in data_file.readlines()]

with open(file_name.split(".")[0], "wb") as output_file:
    for num in data:
        output_file.write(struct.pack("I", num))
        print(num)
