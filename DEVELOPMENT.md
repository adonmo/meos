# Development
MEOS uses the [CMake](https://cmake.org/) and [setuptools](https://setuptools.readthedocs.io/en/latest/) for the build system, and [Catch2](https://github.com/catchorg/Catch2) and [pytest](https://docs.pytest.org/en/latest/) for tests.

## Building

```sh
$ cmake -B build -S .
$ cmake --build build
```

## Testing

### C++

```sh
$ cmake -B build/test -S test -DCMAKE_BUILD_TYPE=Release
$ cmake --build build/test --config Release
$ ./build/test/libmeos-tests
```

### Python

```sh
$ pip install .
$ pip install -r tests/python/requirements.txt
$ pytest
```

## Building docs

### C++ (Doxygen)

```sh
$ cmake -B build/docs -S documentation
$ cmake --build build/docs
$ cmake --build build/docs --target cppdocs
```
You can then proceed to host the docs locally, for example on http://0.0.0.0:8000/
```sh
$ python -m http.server --directory build/docs/doxygen/html
```

### Python (Sphinx)

```sh
$ pip install .[docs]
$ cmake -B build/docs -S documentation
$ cmake --build build/docs
$ cmake --build build/docs --target pydocs
```
You can then proceed to host the docs locally, for example on http://0.0.0.0:8000/
```sh
$ python -m http.server --directory documentation/python/_build/html
```
