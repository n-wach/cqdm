import tqdmc
import tqdm

import timeit

def test_tqdm(n):
    def loop(_n):
        for _ in tqdm.tqdm(range(_n)):
            pass
    print(f"tqdm@{n}:", timeit.timeit(lambda: loop(n), number=3))


def test_tqdmc(n):
    def loop(_n):
        tqdmc.count(range(n))
    print(f"tqdm-c@{n}:", timeit.timeit(lambda: loop(n), number=3))


N = 10_000_000
test_tqdm(N)
test_tqdmc(N)
