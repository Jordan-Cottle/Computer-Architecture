#! /usr/bin/env python

import os
import re
import sys
import time

from file_utils import (
    CPP_DIR,
    HEADERS,
    OBJ_DIR,
    PROJECT_ROOT,
    SourceFile,
    execute,
    find_directories,
    find_files,
)

INCLUDE_DIRS = list(find_directories(PROJECT_ROOT))
INCLUDE_DISPLAY = "\n\t".join(INCLUDE_DIRS)
print(f"Including the follow directories in compile commands:\n\t{INCLUDE_DISPLAY}")

INCLUDE_FLAGS = [f"-iquote {directory}" for directory in INCLUDE_DIRS]
FLAGS = ["-Wall"] + INCLUDE_FLAGS
GCC = f"g++ {' '.join(FLAGS)}"


def needs_compile(cpp_file: SourceFile):
    output_path = cpp_file.object_name

    if not os.path.isfile(output_path):
        print(f"Compiling {cpp_file} because {output_path} does not exist")
        return True

    object_modified = os.path.getmtime(output_path)

    if cpp_file.mtime > object_modified:
        print(f"Compiling {cpp_file} because it is newer than {output_path}")
        return True

    for include in cpp_file.includes:
        header = HEADERS[include]
        if header.mtime > cpp_file.mtime:
            print(f"Compiling {cpp_file} because {header} is newer than it")
            return True

    return False


def compile_cpp(path):
    cpp_file = SourceFile(path)
    output_path = cpp_file.object_name

    output_dir, _ = os.path.split(output_path)
    os.makedirs(output_dir, exist_ok=True)

    if needs_compile(cpp_file):
        execute(f"{GCC} -c -o {cpp_file.object_name} {cpp_file.full_path}")

    return cpp_file.object_name


def build(name, objects):
    execute(f"{GCC} -o {name} {' '.join(objects)}")


def touch_files():
    finished = time.time()
    for file in find_files(CPP_DIR, ".cpp"):
        os.utime(file, (finished, finished))

    for file in find_files(CPP_DIR, ".h"):
        os.utime(file, (finished, finished))

    for file in find_files(OBJ_DIR, ".o"):
        os.utime(file, (finished, finished))


def main():
    objects = []
    for cpp_file in find_files(f"{CPP_DIR}", ".cpp"):
        object_file = compile_cpp(cpp_file)
        objects.append(object_file)

    try:
        output_name = sys.argv[1]
    except IndexError:
        output_name = "main"

    touch_files()
    build(output_name, find_files(OBJ_DIR, ".o"))


if __name__ == "__main__":
    main()
