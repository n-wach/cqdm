import tqdmc
import tqdm

import timeit


def test_tqdm(n, q):
    def loop(_n):
        total = 0
        for i in tqdm.tqdm(range(_n)):
            total += i
        print(total)
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm@{n}: {avg_time} seconds")
    return avg_time


def test_tqdmc(n, q):
    def loop(_n):
        total = 0
        for i in tqdmc.yielder(range(_n)):
            total += i
        print(total)
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm-c@{n}: {avg_time} seconds")
    return avg_time


N = 1_000_000
Q = 1
a = test_tqdm(N, Q)
b = test_tqdmc(N, Q)
print(f"Speedup: {a / b:.3}x")
