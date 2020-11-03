""" Module for compiling the assembly into a simulation-ready binary format """
import re
import os
import sys
import struct


class BinarySection:
    def __init__(self, start, length, name):
        self.end = start
        self.length = length
        self.start = start - length + 1

        self.name = name

    @property
    def high(self):
        assert "imm" in self.name
        match = re.match(IMMEDIATE_PATTERN, self.name)

        return int(match.group(1))

    @property
    def low(self):
        assert "imm" in self.name
        match = re.match(IMMEDIATE_PATTERN, self.name)

        if not match.group(2):
            return int(match.group(1))

        return int(match.group(2))

    def __str__(self):
        return f"{self.end} {self.name} {self.start}"

    @property
    def slice(self):
        return slice(self.start, self.end + 1)


IMMEDIATE_PATTERN = re.compile(r"imm\[(\d+):?(\d+)?\]")


class Format:
    def __init__(self, format_str):

        self.sections = {}
        sections = format_str.split(",")

        bit_idx = 31
        for section in sections:

            if "imm" not in section:
                name, length = section.split(":")
            else:
                name = section
                match = re.match(IMMEDIATE_PATTERN, section)

                if match.group(2):
                    length = 1 + int(match.group(1)) - int(match.group(2))
                else:
                    length = 1

            length = int(length)

            section = BinarySection(bit_idx, length, name)
            self.sections[name] = section

            bit_idx = section.start - 1

        if bit_idx != -1:
            raise ValueError(
                f"{format_str} is an invalid format! All instructions must be 32 bits"
            )

    @property
    def immediates(self):
        return [section for section in self.sections.values() if "imm" in section.name]

    def __getattr__(self, name):
        return self[name]

    def __getitem__(self, name):
        return self.sections[name]

    def __str__(self):
        line = [str(section) for section in self.sections.values()]

        return f"|{'|'.join(line)}|"


R_TYPE = Format("funct7:7,rs2:5,rs1:5,funct3:3,rd:5,opcode:7")
R3_TYPE = Format("rs3:5,funct2:2,rs2:5,rs1:5,funct3:3,rd:5,opcode:7")
I_TYPE = Format("imm[11:0],rs1:5,funct3:3,rd:5,opcode:7")
S_TYPE = Format("imm[11:5],rs2:5,rs1:5,funct3:3,imm[4:0],opcode:7")
B_TYPE = Format("imm[12],imm[10:5],rs2:5,rs1:5,funct3:3,imm[4:1],imm[11],opcode:7")
U_TYPE = Format("imm[31:12],rd:5,opcode:7")
UJ_TYPE = Format("imm[20],imm[10:1],imm[11],imm[19:12],rd:5,opcode:7")
R4_TYPE = Format("rs3:5,fmt:2,rs2:5,rs1:5,rm:3,rd:5,opcode:7")

R_FIELDS = "rd rs1 rs2"
R4_FIELDS = "rd rs1 rs2 rs3 rm"
I_FIELDS = "rd rs1 imm12"
SHIFT_FIELDS = "rd rs1 shamt"
S_FIELDS = "imm12hi rs1 rs2 imm12lo"
B_FIELDS = "bimm12hi rs1 rs2 bimm12lo"
U_FIELDS = "rd imm20"
UJ_FIELDS = "rd jimm20"
FP1_FIELDS = "rd rs1"
FP1R_FIELDS = "rd rs1 rm"
FP_FIELDS = "rd rs1 rs2 rm"

INSTRUCTION_TYPES = {
    R_FIELDS: R_TYPE,
    FP_FIELDS: R_TYPE,
    FP1_FIELDS: R_TYPE,
    FP1R_FIELDS: R_TYPE,
    R4_FIELDS: R4_TYPE,
    I_FIELDS: I_TYPE,
    SHIFT_FIELDS: I_TYPE,
    S_FIELDS: S_TYPE,
    B_FIELDS: B_TYPE,
    U_FIELDS: U_TYPE,
    UJ_FIELDS: UJ_TYPE,
}

for instruction_type in INSTRUCTION_TYPES.values():
    print(instruction_type)


class InstructionTemplate:
    def __init__(self, fields, bits=None):
        self.keyword, fields = fields.split(" ", 1)
        self.format = INSTRUCTION_TYPES[fields]
        self.bits = list(bits)

    def get_bits(self, section):
        return "".join(self.bits[::-1][section.slice][::-1])

    @property
    def immediate(self):
        immediates = [
            section
            for section in self.format.sections.values()
            if "imm" in section.name
        ]

        if not immediates:
            return ""

        bits = {}
        for immediate in immediates:
            match = re.match(IMMEDIATE_PATTERN, immediate.name)
            idx = int(match.group(1))

            data = self.bits[::-1][immediate.slice][::-1]

            bits[idx - len(data) + 1] = data

        idx = min(bits)
        value = []
        while idx in bits:
            partial = bits.pop(idx)
            value = partial + value
            idx += len(partial)

        assert not bits, "All bits should have been used"

        return "".join(value)

    @immediate.setter
    def immediate(self, value):
        immediates = self.format.immediates

        value = value[::-1]

        assert (
            immediates
        ), "Cannot set immediate value on an instruction without an immediate value!"

        offset = min(immediate.low for immediate in immediates)

        bits = {}
        for immediate in immediates:
            idx = immediate.start

            data = value[immediate.low - offset : immediate.high + 1 - offset][::-1]

            bits[immediate.end] = data

        idx = 31
        string = []
        inverse = self.bits[::-1]
        while idx >= 0:
            if idx not in bits:
                string.append(inverse[idx])
                idx -= 1
                continue

            partial = bits.pop(idx)
            string.extend(partial)

            idx -= len(partial)

        assert not bits, "All bits should have been used"

        self.bits = string

    def __getattr__(self, name):
        if name in self.format.sections:
            return self[name]

    def __getitem__(self, name):
        return self.get_bits(self.format[name])

    @property
    def binary(self):
        return "".join(self.bits)

    def __str__(self):
        string = []
        for section in self.format.sections.values():
            string.append(self.get_bits(section))

        return f"{self.keyword} {'|'.join(string)}"


def twos_compliment(num, bits):
    b = (num).to_bytes(bits // 4 + 1, byteorder="big", signed=True)
    return bin(int.from_bytes(b, byteorder="big"))[-bits:]


ROUNDING_MODE = "000"


class Instruction(InstructionTemplate):
    def __init__(self, template):
        self.keyword = template.keyword
        self.format = template.format
        self.bits = template.bits

    @classmethod
    def parse(cls, line, labels):
        keyword, *args = line.split()
        template = INSTRUCTIONS[keyword]

        instruction = cls(template)

        immediate_instruction = False
        for section in instruction.format.sections.values():
            if "?" not in instruction.get_bits(section):
                continue

            if "funct3" in section.name and instruction.keyword.startswith("f"):
                instruction.set_bits(section, ROUNDING_MODE)
                continue

            if "imm" in section.name:
                immediate_instruction = True
                continue
            else:
                for arg in args:
                    match = re.search("[xf](\d+)", arg)
                    if match:
                        if not re.search(r"\d+\(", arg):
                            args.remove(arg)

                        binary = bin(int(match.group(1)))[2:]

                        while len(binary) < section.length:
                            binary = "0" + binary

                        instruction.set_bits(section, binary)
                        break
                else:
                    print("Did you miss something??", section, args)

        if immediate_instruction:
            assert len(args) == 1, "Last arg should contain the immediate value"
            arg = args[0]
            match = re.search(r"(\d+)\(", arg)
            if match:  # Memory offset
                value = int(match.group(1))
            else:
                try:
                    value = int(arg)
                except ValueError:
                    value = labels[arg]

            target_length = len(instruction.immediate)
            offset = min(immediate.low for immediate in instruction.format.immediates)
            if value < 0:
                value = twos_compliment(value, target_length + offset)

                if offset:
                    value = value[:-offset]

            else:
                value = bin(value)[2:]

                if offset:
                    value = value[:-offset]

                while len(value) < target_length:
                    value = "0" + value

            instruction.immediate = value

            print(offset, instruction.immediate)

        return instruction

    def set_bits(self, section, data):
        assert (
            len(data) == section.length
        ), "New data must be exactly the same length as the section is is to be put it"

        updated = self.bits[::-1]
        updated[section.slice] = data[::-1]
        self.bits = updated[::-1]


instruction = InstructionTemplate(f"foo {R_FIELDS}", "1234567r1r2rR3R4Rfffd2d3do1o2o3o")
assert instruction.opcode == "o1o2o3o", instruction.opcode
assert instruction.rd == "d2d3d", instruction.rd
assert instruction.rs1 == "R3R4R", instruction.rs1
assert instruction.rs2 == "r1r2r", instruction.rs2
assert instruction.funct7 == "1234567", instruction.funct7

instruction = InstructionTemplate(f"foo {I_FIELDS}", "BA9876543210R3R4Rfffd2d3do1o2o3o")
assert instruction.opcode == "o1o2o3o", instruction.opcode
assert instruction.rd == "d2d3d", instruction.rd
assert instruction.rs1 == "R3R4R", instruction.rs1
assert instruction.immediate == "BA9876543210", instruction.immediate

"0123456789ABCDEFGHIJK"

instruction = InstructionTemplate(
    f"foo {UJ_FIELDS}", "KA987654321BJIHGFEDCd2d3do1o2o3o"
)
assert instruction.immediate == "KJIHGFEDCBA987654321", instruction.immediate
assert instruction.opcode == "o1o2o3o", instruction.opcode
assert instruction.rd == "d2d3d", instruction.rd

with open("opcodes.txt", "r") as opcode_file:
    data = opcode_file.readlines()

INSTRUCTIONS = {}
for i, line in enumerate(data):
    if re.match("[a-z]", line):
        fields = line.strip()
        bits = data[i + 1].strip()
        template = InstructionTemplate(fields, bits)
        INSTRUCTIONS[template.keyword] = template

instruction = InstructionTemplate(f"beq {B_FIELDS}", "?????????????????000?????0001111")
instruction.immediate = "CBA987654321"

assert instruction.immediate == "CBA987654321", instruction.immediate
assert instruction["imm[12]"] == "C", instruction["imm[12]"]
assert instruction["imm[11]"] == "B", instruction["imm[11]"]
assert instruction["imm[10:5]"] == "A98765", instruction["imm[10:5]"]
assert instruction["imm[4:1]"] == "4321", instruction["imm[4:1]"]

instruction = InstructionTemplate(
    f"foo {UJ_FIELDS}", "?????????????????????????1000011"
)
instruction.immediate = "KJIHGFEDCBA987654321"

assert instruction.immediate == "KJIHGFEDCBA987654321", instruction.immediate
assert instruction["imm[20]"] == "K", instruction["imm[20]"]
assert instruction["imm[19:12]"] == "JIHGFEDC", instruction["imm[19:12]"]
assert instruction["imm[11]"] == "B", instruction["imm[11]"]
assert instruction["imm[10:1]"] == "A987654321", instruction["imm[10:1]"]
assert instruction.binary == "KA987654321BJIHGFEDC?????1000011"


def read_file(file_name):
    with open(file_name, "r") as assembly_file:
        data = [
            line.strip()
            for line in assembly_file.readlines()
            if not line.startswith("#")
        ]
    return data


def assemble_program(data):
    labels = {}
    mem_address = 0
    for line in list(data):
        # print(line)

        match = re.match(r"(.+):", line)
        if match:
            labels[match.group(1)] = mem_address
            data.remove(line)
        elif line.strip():
            mem_address += 4

    print(labels, data)

    program = []
    for line in data:
        print(line)

        program.append(Instruction.parse(line, labels))

    return program


def to_bytes(binary_string):

    print(int(binary_string, 2))
    return struct.pack("I", int(binary_string, 2))


def write_program(file_name, program, binary=False):

    if binary:
        write_mode = "wb"
        ext = "bin"
    else:
        write_mode = "w"
        ext = "sim"

    with open(f"{file_name}.{ext}", write_mode) as sim_file:
        for instruction in program:
            bin_string = instruction.binary

            if binary:
                sim_file.write(to_bytes(bin_string))
            else:
                print(bin_string, file=sim_file)


def main(file, binary=False):

    data = read_file(file)
    program = assemble_program(data)

    output_file, ext = os.path.splitext(file)
    write_program(output_file, program, binary)


if __name__ == "__main__":
    for file in sys.argv[1:]:
        main(sys.argv[1:])
