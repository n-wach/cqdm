import tqdmc
import tqdm
import tqdm_base

import timeit
import time

import matplotlib.pyplot as plt

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
    print(f"tqdm-yielder@{n}: {avg_time} seconds")
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
        bar = tqdm_base.tqdm(range(_n))
        for i in iter(bar):
            pass
    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm-c@{n}: {avg_time} seconds")
    return avg_time

MAX_ITER = 10
N = 1_000_000
Q = 10

nop = []
dumb = []
yielder = []
tqdm_orig = []
tqdm_iter = []
iterations = []

for i in range(MAX_ITER):
    inc = i+1
    nop.append(test_nop(N*inc, Q))

    dumb.append(test_dumb(N*inc, Q))
    yielder.append(test_tqdmc_yielder(N*inc, Q))

    tqdm_orig.append(test_tqdm(N*inc, Q))
    tqdm_iter.append(test_tqdmc_tqdmiter(N*inc, Q))

    iterations.append(N*inc)

a = test_tqdm(N, Q)
b = test_tqdmc_tqdmiter(N, Q)
print(f"Speedup: {a / b:.3}x")

fig, (ax1, ax2) = plt.subplots(1, 2)

ax1.set_title('Time Taken as Iterations Increase For Different Operations')
ax1.set_xlabel('number of iterations')
ax1.set_ylabel('time in seconds')

ax1.plot(iterations, nop, 'ko', linewidth=2, label='no operations')
ax1.plot(iterations, dumb, 'bo', linewidth=2, label='basic iterable wrapper')
ax1.plot(iterations, yielder, 'yo', linewidth=2, label='c basic iterable')
ax1.plot(iterations, tqdm_orig, 'ro', linewidth=2, label='original tqdm')
ax1.plot(iterations, tqdm_iter, 'go', linewidth=2, label='c tqdm')

improvement = []
for i in range(MAX_ITER):
    improvement.append(tqdm_orig[i]/tqdm_iter[i])

ax2.set_title('Tqdm With C Speedup Compared to Tqdm')
ax2.set_xlabel('iterations')
ax2.set_ylabel('improvement (tqdm time/tqdm-c time)')

ax2.plot(iterations, improvement)

legend = ax1.legend(loc='upper left')

plt.show()