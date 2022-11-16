import tqdm
import cqdm
import sys

print("Here goes tqdm:")
for i in tqdm.tqdm(range(50_000_000)):
    pass

print("And now for cqdm:")
sys.stderr.flush()
sys.stdout.flush()
for i in cqdm.cqdm(range(50_000_000)):
    pass
