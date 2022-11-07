import tqdm
import cqdm_native


class cqdm(tqdm.tqdm):
    def __iter__(self):
        if self.disable:
            yield from self.iterable
            return

        self.cqdm = cqdm_native.cqdm(self)
        yield from self.cqdm
