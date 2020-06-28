MEOS -- Mobility Engine, Open Source
====================================

MEOS or libmeos is a library which makes it easy to work with temporal and spatio-temporal data. It is based on [MobilityDB](https://github.com/ULB-CoDE-WIT/MobilityDB)'s data types and functions. MEOS's code is heavily inspired from a similar library called [GEOS](https://github.com/libgeos/geos). MEOS is written in modern C++ with bindings to python using [pybind11](https://github.com/pybind/pybind11).

⚠️ Currently this library is in very early, completely **experimental** stages - heavy changes are to be expected.

# Usage

## Python

### Installation

```sh
$ pip install git+https://github.com/chaitan94/meos
```

Note: This assumes libgeos-dev is already installed on your system.

### Code sample

```python
import datetime
from pymeos import Geometry
from pymeos.temporal import TInstantGeom, TSequenceGeom


def datetime_utc(year, month, day, hour=0, minute=0, second=0):
    return datetime.datetime(year, month, day, hour, minute, second, tzinfo=datetime.timezone.utc)


# Example creation of trajectory (temporal sequence of geometries)
trajectory = TSequenceGeom([
    TInstantGeom(Geometry(0, 0), datetime_utc(2012, 1, 1, 8, 0)),
    TInstantGeom(Geometry(2, 0), datetime_utc(2012, 1, 1, 8, 10)),
    TInstantGeom(Geometry(2, 1), datetime_utc(2012, 1, 1, 8, 15)),
])

print(trajectory)
```

```
[POINT (0 0)@2012-01-01T08:00:00+0000, POINT (2 0)@2012-01-01T08:10:00+0000, POINT (2 1)@2012-01-01T08:15:00+0000)
```

### Documentation

Docs and API Reference: https://pymeos.netlify.app

More detailed usage guide/quickstart: https://pymeos.netlify.app/quickstart.html

## C++

```cpp
#include <iostream>
#include <meos/types/temporal/TSequence.hpp>

using namespace std;


int main() {
    vector<TInstant<int>> instants = {
        TInstant<int>(2, unix_time_point(2012, 1, 1)),
        TInstant<int>(1, unix_time_point(2012, 1, 2)),
        TInstant<int>(4, unix_time_point(2012, 1, 3)),
        TInstant<int>(3, unix_time_point(2012, 1, 4)),
    };
    TSequence<int> tseq(instants, true, false);
    cout << tseq << endl;
    return 0;
}
```

### Documentation

C++ API Reference: https://meos.netlify.app

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
$ cmake -B build/test -S test
$ cmake --build build/test
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

## Contributing

Issues and pull requests are welcome.

* For proposing new features/improvements or reporting bugs, [create an issue](https://github.com/chaitan94/meos/issues/new/choose).
* Check [open issues](https://github.com/chaitan94/meos/issues) for viewing existing ideas, verify if it is already proposed/being worked upon.
* When implementing new features make sure to add relevant tests and documentation before sending pull requests.
