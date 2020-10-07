import os
import re
from functools import lru_cache

STRUCT_START = re.compile(r"struct ([^ *]+)")
CLASS_START = re.compile(r"class ([^ *]+)")

CLASS_PATTERS = (STRUCT_START, CLASS_START)

CPP_DIR = "cpp_source"
OBJ_DIR = "obj"


@lru_cache(32)
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


class FunctionHeader:
    def __init__(self, modifiers, return_type, name, signature):
        self.class_ = None
        self.modifiers = modifiers.strip()
        self.return_type = return_type
        self.name = name
        self.signature = signature

    @property
    def empty_implementation(self):
        return f"{repr(self)}\n{{\n    \n}}"

    def __repr__(self):
        name = self.name
        if self.class_:
            name = f"{self.class_}::{name}"

        string = f"{name}({self.signature})"

        if self.return_type:
            string = " ".join((self.return_type, string))

        if self.modifiers:
            string = " ".join((self.modifiers, string))

        return string


def class_name(line):
    for pattern in CLASS_PATTERS:
        match = re.match(pattern, line)
        if match:
            return match.group(1)

    return None


FUNCTION_HEADER_PATTERN = re.compile(r"(.*) ([\w:]*) ([^ ]+)\((.*)\);")


def function_header(line):
    match = re.search(FUNCTION_HEADER_PATTERN, line)

    if match:
        return FunctionHeader(*match.groups())

    return None


def find_functions(data):
    class_in_scope = None

    functions = []
    for line in data:
        if line.startswith("};"):
            class_in_scope = None
            continue

        class_found = class_name(line)
        if class_found:
            class_in_scope = class_found

        header = function_header(line)

        if not header:
            continue

        if class_in_scope:
            header.class_ = class_in_scope

        functions.append(header)

    return functions


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


PROJECT_ROOT = f"{os.getcwd()}/cpp_source".replace("//", "/")
HEADERS = {
    header.name: header
    for header in [SourceFile(file) for file in find_files(PROJECT_ROOT, ".h")]
}