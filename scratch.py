import tqdmc
import tqdm

import timeit

def test_tqdm(n):
    def loop(_n):
        total = 0
        for i in tqdm.tqdm(range(_n)):
            total += i
        print(total)
    print(f"tqdm@{n}:", timeit.timeit(lambda: loop(n), number=3))


def test_tqdmc(n):
    def loop(_n):
        total = 0
        for i in tqdmc.yielder(range(_n)):
            total += i
        print(total)
    print(f"tqdm-c@{n}:", timeit.timeit(lambda: loop(n), number=3))


N = 10_000_000
test_tqdm(N)
test_tqdmc(N)
