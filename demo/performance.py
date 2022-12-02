import cqdm
import tqdm
import dis

import timeit

import matplotlib.pyplot as plt


def test_nop(n, q):
    # a boring loop, doing nothing
    def loop(_n):
        total = 0
        for i in range(_n):
            pass

    total = timeit.timeit(lambda: loop(n), number=q)
    avg = total / q
    print(f"nop@{n}: {avg} seconds")
    return avg


def test_tqdm(n, q):
    # the OG tqdm implementation
    def loop(_n):
        total = 0
        for i in tqdm.tqdm(range(_n)):
            pass

    total = timeit.timeit(lambda: loop(n), number=q)
    avg = total / q
    print(f"tqdm@{n}: {avg} seconds")
    return avg


def test_cqdm(n, q):
    # our hacky implementation
    def loop(_n):
        total = 0
        for i in cqdm.cqdm(range(_n)):
            pass

    total = timeit.timeit(lambda: loop(n), number=q)
    avg = total / q
    print(f"cqdm@{n}: {avg} seconds")
    return avg


MAX_ITER = 10
N = 10_000
Q = 1_000

nop_times = []
tqdm_times = []
cqdm_times = []
iterations = []

for i in range(1, MAX_ITER + 1):
    nop_times.append(test_nop(N * i, Q))

    tqdm_times.append(test_tqdm(N * i, Q))
    cqdm_times.append(test_cqdm(N * i, Q))

    iterations.append(N * i)


fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(8, 8))

ax1.set_title("Total Overhead for Large Iterables")
ax1.set_xlabel("Iterations")
ax1.set_ylabel("Total Overhead (seconds)")

ax1.plot(iterations, tqdm_times, "r-", linewidth=2, label="tqdm")
ax1.plot(iterations, cqdm_times, "g-", linewidth=2, label="cqdm")
ax1.plot(iterations, nop_times, "k-", linewidth=2, label="no bar")

improvement = []
for i in range(MAX_ITER):
    improvement.append(tqdm_times[i] / cqdm_times[i])

average = sum(improvement) / MAX_ITER
average = [average] * len(improvement)

ax2.set_title("Speedup for Large Iterables")
ax2.set_xlabel("Iterations")
ax2.set_ylabel("Speedup (tqdm vs cqdm)")
ax2.set_ylim(1.0, 5.5)

ax2.plot(iterations, improvement, color="#1f77b4")
ax2.plot(iterations, average, ":", color="#1f77b4")

legend = ax1.legend(loc="upper left")

# dis.dis(test_cqdm)
# dis.dis(test_tqdm)

plt.show()
