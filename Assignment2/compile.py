#! /usr/bin/env python

import os
import re
import sys
import time
from functools import lru_cache

PROJECT_ROOT = f"{os.getcwd()}/cpp_source".replace("//", "/")


def execute(system_call):
    pretty_display = re.sub(r"-iquote [^ ]* ", "", system_call)
    print(pretty_display)
    os.system(system_call)


def find_directories(root):
    yield root
    for name in os.listdir(root):
        name = os.path.join(root, name)
        if os.path.isdir(name):
            yield from find_directories(name)


def find_files(root_dir, extension):
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for file_name in filenames:
            _, _extension = os.path.splitext(file_name)
            if _extension == extension:
                yield os.path.join(dirpath, file_name)


CPP_DIR = "cpp_source"
OBJ_DIR = "obj"

INCLUDE_DIRS = list(find_directories(PROJECT_ROOT))
INCLUDE_DISPLAY = "\n\t".join(INCLUDE_DIRS)
print(f"Including the follow directories in compile commands:\n\t{INCLUDE_DISPLAY}")

INCLUDE_FLAGS = [f"-iquote {directory}" for directory in INCLUDE_DIRS]
FLAGS = ["-Wall"] + INCLUDE_FLAGS
GCC = f"g++ {' '.join(FLAGS)}"


class SourceFile:
    def __init__(self, path):
        self.full_path = os.path.abspath(path)

        self.dir, self.name = os.path.split(self.full_path)

        self.mtime = os.path.getmtime(self.full_path)

        self._includes = None

    @property
    def object_name(self):
        return self.full_path.replace(CPP_DIR, OBJ_DIR).replace(".cpp", ".o")

    @property
    def includes(self):
        if self._includes is None:
            self._includes = requires(self.full_path)

        return self._includes

    def __repr__(self):
        return self.full_path


HEADERS = {
    header.name: header
    for header in [SourceFile(file) for file in find_files(PROJECT_ROOT, ".h")]
}


@lru_cache
def requires(path):
    with open(path, "r") as source_file:
        data = source_file.readlines()

    includes = set()
    for line in data:
        match = re.search(r'#include "(.+)"', line)
        if match:
            includes.add(match.group(1))

    for include in list(includes):
        if include in HEADERS:
            for requirement in HEADERS[include].includes:
                includes.add(requirement)

    return includes


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
