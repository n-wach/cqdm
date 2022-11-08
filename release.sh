#!/bin/sh

docker run --rm -e PLAT=manylinux2010_x86_64 -v `pwd`:/io quay.io/pypa/manylinux2010_x86_64 /io/build_wheels.sh
read -p "cqdm PyPI token: " token
TWINE_USERNAME=__token__ TWINE_PASSWORD=$token twine upload wheelhouse/*
