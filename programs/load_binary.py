import sys
import struct

file_name = sys.argv[1]
base = int(sys.argv[2])
try:
    bit_length = int(sys.argv[3])
except IndexError:
    bit_length = 32

with open(file_name, "r") as data_file:
    data = [int(line, base) for line in data_file.readlines()]

STRUCT_SIZE = {8: "B", 16: "H", 32: "I"}

with open(file_name.split(".")[0], "wb") as output_file:
    for num in data:
        value = struct.pack(STRUCT_SIZE[bit_length], num)
        output_file.write(value)
        print(value.hex())
