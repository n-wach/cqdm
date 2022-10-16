from distutils.core import setup, Extension

module1 = Extension(
    "tqdmc",
    define_macros=[("MAJOR_VERSION", "1"), ("MINOR_VERSION", "0")],
    include_dirs=["/usr/local/include"],
    libraries=["tcl83"],
    library_dirs=["/usr/local/lib"],
    sources=["tqdm.c"],
)

setup(
    name="tqdmc",
    version="1.0",
    description="The popular tqdm library, accelerated with C bindings",
    url="https://github.com/n-wach/tqdm-c",
    ext_modules=[module1],
)
