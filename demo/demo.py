import tqdm
import cqdm
import time

print("Iterating with tqdm:")
tqdm_start = time.time()
for i in tqdm.tqdm(range(50_000_000)):
    pass
tqdm_dur = time.time() - tqdm_start
print(f"That took {tqdm_dur:.2f} seconds")

print("Iterating with cqdm:")
cqdm_start = time.time()
for i in cqdm.cqdm(range(50_000_000)):
    pass
cqdm_dur = time.time() - cqdm_start
print(f"That took {cqdm_dur:.2f} seconds")

print(f"That's a speed up of {tqdm_dur / cqdm_dur:.2f}x")
