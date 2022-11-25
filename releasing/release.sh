#!/bin/sh

# build wheels
docker run --rm -e PLAT=manylinux2010_x86_64 -v `pwd`:/io quay.io/pypa/manylinux2010_x86_64 /io/releasing/build_wheels.sh

# deploy to PyPI
read -p "cqdm PyPI token: " token
TWINE_USERNAME=__token__ TWINE_PASSWORD=$token twine upload wheelhouse/*
