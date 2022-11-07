from distutils.core import setup, Extension

cqdm_native = Extension(
    "cqdm_native",
    sources=["cqdm.c"],
)

setup(
    name="cqdm",
    version="1.0",
    description="A drop-in replacement for the popular tqdm library, accelerated with C bindings",
    url="https://github.com/n-wach/cqdm",
    ext_modules=[cqdm_native],
    install_requires=["tqdm"],
)
