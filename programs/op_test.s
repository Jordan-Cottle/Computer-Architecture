addi x1 zero 200  # Set a memory address to use for test in x1
addi x2 zero 42   # Set a number to use for test in x2

sb x2 0(x1)  # Store num in memory
lb x3 0(x1)  # Read byte back into x3
