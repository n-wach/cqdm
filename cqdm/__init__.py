import tqdm
import cqdm_native


class cqdm(tqdm.tqdm):
    def __setattr__(self, key, value):
        super(cqdm, self).__setattr__(key, value)
        if key == "miniters" and hasattr(self, "_cqdm"):
            self._cqdm.set_miniters(value)

    def __iter__(self):
        if self.disable:
            return iter(self.iterable)

        self._cqdm = cqdm_native.cqdm(self)
        return self._cqdm
