from setuptools import Extension, find_packages, setup

cqdm_native = Extension(
    "cqdm_native",
    sources=["cqdm.c"],
)

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="cqdm",
    version="1.0.4",
    description="A drop-in replacement for the popular tqdm library, accelerated with C bindings",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/n-wach/cqdm",
    ext_modules=[cqdm_native],
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.6",
    install_requires=["tqdm"],
)
