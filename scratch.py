import tqdmc
import tqdm

import timeit


def test_nop(n, q):
    def loop(_n):
        total = 0
        for i in range(_n):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"nop@{n}: {avg_time} seconds")
    return avg_time


def test_dumb(n, q):
    def dumb_yielder(i):
        for bla in i:
            yield bla

    def loop(_n):
        total = 0
        for i in dumb_yielder(range(_n)):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"dumb@{n}: {avg_time} seconds")
    return avg_time


def test_tqdm(n, q):
    def loop(_n):
        total = 0
        for i in tqdm.tqdm(range(_n)):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm@{n}: {avg_time} seconds")
    return avg_time


def test_tqdmc(n, q):
    def loop(_n):
        total = 0
        for i in tqdmc.yielder(range(_n)):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm-c@{n}: {avg_time} seconds")
    return avg_time


N = 1_000_000
Q = 10
base = test_nop(N, Q)
test_dumb(N, Q)
a = test_tqdm(N, Q)
b = test_tqdmc(N, Q)
print(f"Speedup: {a / b:.3}x")
