from distutils.core import setup, Extension

module1 = Extension(
    "tqdmc",
    sources=["tqdmc.c"],
)

setup(
    name="tqdmc",
    version="1.0",
    description="The popular tqdm library, accelerated with C bindings",
    url="https://github.com/n-wach/tqdm-c",
    ext_modules=[module1],
)
