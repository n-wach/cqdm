import cqdm
import tqdm
import dis

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


def test_tqdm(n, q):
    # the OG tqdm implementation
    def loop(_n):
        total = 0
        for i in tqdm.tqdm(range(_n)):
            pass

    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"tqdm@{n}: {avg_time} seconds")
    return avg_time


def test_cqdm(n, q):
    # our hacky implementation
    def loop(_n):
        total = 0
        for i in cqdm.cqdm(range(_n)):
            pass

    avg_time = timeit.timeit(lambda: loop(n), number=q)
    print(f"cqdm@{n}: {avg_time} seconds")
    return avg_time


MAX_ITER = 10
N = 10_000
Q = 500

nop_times = []
tqdm_times = []
cqdm_times = []
iterations = []

for i in range(MAX_ITER):
    inc = i + 1
    nop_times.append(test_nop(N * inc, Q))

    tqdm_times.append(test_tqdm(N * inc, Q))
    cqdm_times.append(test_cqdm(N * inc, Q))

    iterations.append(N * inc)

a = test_tqdm(N, Q)
b = test_cqdm(N, Q)
print(f"Speedup: {a / b:.3}x")

fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(8, 8))

ax1.set_title("Time Taken as Iterations Increase")
ax1.set_xlabel("iterations")
ax1.set_ylabel("time in seconds")

ax1.plot(iterations, nop_times, "k-", linewidth=2, label="no bar")
ax1.plot(iterations, tqdm_times, "r-", linewidth=2, label="tqdm")
ax1.plot(iterations, cqdm_times, "g-", linewidth=2, label="cqdm")

improvement = []
for i in range(MAX_ITER):
    improvement.append(tqdm_times[i] / cqdm_times[i])

ax2.set_title("cqdm Speedup Compared to tqdm")
ax2.set_xlabel("iterations")
ax2.set_ylabel("improvement (tqdm time/cqdm time)")
ax2.set_ylim(1.0, 4.0)

ax2.plot(iterations, improvement)

legend = ax1.legend(loc="upper left")

dis.dis(test_cqdm)
dis.dis(test_tqdm)

plt.show()
