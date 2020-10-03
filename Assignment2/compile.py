#! /usr/bin/env python

import re
import os
import sys


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


def replace_top_dir(path, target, new_ext=None):
    name, ext = os.path.splitext(path)
    if new_ext is not None:
        ext = new_ext
        if not ext.startswith("."):
            ext = f".{ext}"

    dirs = name.split("/")
    dirs[0] = target

    os.makedirs(os.path.join(*dirs[:-1]), exist_ok=True)
    return f"{os.path.join(*dirs)}{ext}"


CPP_DIR = "cpp_source"
OBJ_DIR = "obj"

PROJECT_ROOT = f"{os.getcwd()}/cpp_source"

INCLUDE_DIRS = list(find_directories(PROJECT_ROOT))
INCLUDE_DISPLAY = "\n\t".join(INCLUDE_DIRS)
print(f"Including the follow directories in compile commands:\n\t{INCLUDE_DISPLAY}")

INCLUDE_FLAGS = [f"-iquote {directory}" for directory in INCLUDE_DIRS]
FLAGS = ["-Wall"] + INCLUDE_FLAGS
GCC = f"g++ {' '.join(FLAGS)}"


def compile_cpp(path):
    output_path = replace_top_dir(path, OBJ_DIR, ".o")
    execute(f"{GCC} -c -o {output_path} {path}")

    return output_path


def build(name, objects):
    execute(f"{GCC} -o {name} {' '.join(objects)}")


def main():
    objects = []
    for cpp_file in find_files(f"{CPP_DIR}", ".cpp"):
        object_file = compile_cpp(cpp_file)
        objects.append(object_file)

    try:
        output_name = sys.argv[1]
    except IndexError:
        output_name = "main"

    build(output_name, find_files(f"{OBJ_DIR}", ".o"))


if __name__ == "__main__":
    main()
