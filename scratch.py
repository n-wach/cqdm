import tqdmc
import tqdm
import tqdm_base

import timeit


def test_nop(n, q):
    # a boring loop, doing nothing
    def loop(_n):
        total = 0
        for i in range(_n):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"nop@{n}: {avg_time} seconds")
    return avg_time


def test_dumb(n, q):
    # the most basic wrapper around an iterable, in python
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


def test_tqdmc_yielder(n, q):
    # identical to dumb_yielder, but using a C extension wrapper
    def loop(_n):
        total = 0
        for i in tqdmc.yielder(range(_n)):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm-c@{n}: {avg_time} seconds")
    return avg_time


def test_tqdm(n, q):
    # the OG tqdm implementation
    def loop(_n):
        total = 0
        for i in tqdm.tqdm(range(_n)):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm@{n}: {avg_time} seconds")
    return avg_time


def test_tqdmc_tqdmiter(n, q):
    # our hacky implementation
    def loop(_n):
        total = 0
        for i in tqdm_base.tqdm(range(_n)):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm-c@{n}: {avg_time} seconds")
    return avg_time


N = 1_000_000
Q = 10

test_nop(N, Q)

test_dumb(N, Q)
test_tqdmc_yielder(N, Q)

a = test_tqdm(N, Q)
b = test_tqdmc_tqdmiter(N, Q)

print(f"Speedup: {a / b:.3}x")
