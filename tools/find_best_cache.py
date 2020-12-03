import json
import os
from subprocess import DEVNULL, Popen
from itertools import combinations_with_replacement


def pows_of_two(start, max_size):
    i = start
    while i <= max_size:
        yield i
        i *= 2


def run_tests():
    MAX_SIZE = 1536
    MEM_PER_CPU = MAX_SIZE / 2

    sim_number = 0
    for i_size, d_size in combinations_with_replacement(
        pows_of_two(32, MEM_PER_CPU), 2
    ):
        if i_size + d_size > MEM_PER_CPU:
            continue

        min_size = min(i_size, d_size)
        for line_size in pows_of_two(32, min_size):
            for i_associativity in pows_of_two(1, i_size / line_size):
                for d_associativity in pows_of_two(1, d_size / line_size):

                    Popen(
                        [
                            "./duo-optimum.exe",
                            str(i_size),
                            str(d_size),
                            str(line_size),
                            str(i_associativity),
                            str(d_associativity),
                            f"{sim_number}.json",
                        ],
                        stderr=DEVNULL,
                        stdout=DEVNULL,
                    )

                    sim_number += 1

    return sim_number


class Result:
    def __init__(self, data) -> None:
        self.data = data

    def __lt__(self, other):
        return self.data["overall_cpi"] < other.data["overall_cpi"]

    def __str__(self) -> str:
        data_cache = self.data["cpu0"]["data_cache"]
        instruction_cache = self.data["cpu0"]["instruction_cache"]

        return f"""
Overall Cpi: {self.data['overall_cpi']}
Line Size: {data_cache['cache_block_size']}
Data Cache:
    Size: {data_cache['cache_size']}
    Associativity: {data_cache['cache_associativity']}
    Access Time: {data_cache['cache_access_time']}
    Hit Rate: {data_cache['hit_rate']}
Instruction Cache:
    Size: {instruction_cache['cache_size']}
    Associativity: {instruction_cache['cache_associativity']}
    Access Time: {instruction_cache['cache_access_time']}
    Line Size: {instruction_cache['cache_block_size']}
    Hit Rate: {instruction_cache['hit_rate']}
"""


def check_results(test_count):

    results = []
    for i in range(test_count):
        file_name = f"{i}.json"
        test_success = os.path.isfile(file_name)

        if not test_success:
            continue

        with open(file_name, "r") as result_file:
            data = json.load(result_file)

            results.append(Result(data))

    for result in sorted(results)[:10]:
        print(result)
        input("Press enter to continue")


def main():
    count = run_tests()
    check_results(count)


if __name__ == "__main__":
    main()
