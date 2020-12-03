import sys
import os
import struct


def main(file_name, bit_length=32, base=10):

    with open(file_name, "r") as data_file:
        data = [int(line, base) for line in data_file.readlines()]

    STRUCT_SIZE = {8: "B", 16: "H", 32: "I"}

    base_name, _ = os.path.splitext(os.path.basename(file_name))
    with open(f"{os.getcwd()}/{base_name}.dat", "wb") as output_file:
        for num in data:
            value = struct.pack(STRUCT_SIZE[bit_length], num)
            output_file.write(value)
            print(value.hex())


if __name__ == "__main__":
    file_name = sys.argv[1]

    try:
        bit_length = int(sys.argv[2])
    except IndexError:
        bit_length = 32

    try:
        base = int(sys.argv[3])
    except IndexError:
        base = 10

    main(file_name, base, bit_length)
