MEOS -- Mobility Engine, Open Source
====

MEOS or libmeos is a library which makes it easy to work with temporal and spatio-temporal data. It is based on [MobilityDB](https://github.com/ULB-CoDE-WIT/MobilityDB)'s data types. MEOS's code is heavily inspired from a similar library called [GEOS](https://github.com/libgeos/geos). MEOS is written in modern C++ with bindings to python using [pybind11](https://github.com/pybind/pybind11).

⚠️ Currently this library is in very early, completely **experimental** stages - heavy changes are to be expected.

# Installation

The package can be installed by running:

```sh
$ pip install git+https://github.com/chaitan94/meos
```

⚠️ Note: currently only python3.6 is supported

# Usage

## Temporal data objects
```python
import pytz
from datetime import datetime
from pymeos import TInstantInt, TInstantFloat, TInstantSetInt, TSequenceFloat

def epoch_millis(year, month, day):
    return int(datetime(year, month, day).replace(tzinfo=pytz.UTC).timestamp() * 1000)

# Temporal data types
t = TInstantInt(10, epoch_millis(2011, 1, 1))
print(t.getValue(), t.getT())

tset = TInstantSetInt({t})
t = tset.getInstants().pop()
print(t.getValue(), t.getT())

tf = TInstantFloat(1.0, epoch_millis(2011, 1, 1))
tseq = TSequenceFloat([tf], False, True)
print(tseq.left_open, tseq.right_open)
t = tseq.getInstants()[0]
print(t.getValue(), t.getT())
```
This would output:
```
10 1293840000000
10 1293840000000
False True
1.0 1293840000000
```

## Deserialization / parsing
```python
from datetime import datetime
from pymeos import DeserializerInt, DeserializerFloat


# Deserialization / parsing of temporal data types
p = DeserializerInt("10@2011-01-01")
t = p.nextTInstant()
print(t.getValue(), t.getT())

p = DeserializerFloat("{1.0@2011-01-01, 2.5@2011-01-02}")
tset = p.nextTInstantSet()
for t in tset.getInstants():
    print(t.getValue(), t.getT())

p = DeserializerInt("[10@2011-01-01, 20@2011-01-02)")
tseq = p.nextTSequence()
print(tseq.left_open, tseq.right_open)
for t in tseq.getInstants():
    print(t.getValue(), t.getT())
```
This would output:
```
10 1293840000000
2.5 1293926400000
1.0 1293840000000
False True
10 1293840000000
20 1293926400000
```

# Development

## Building

MEOS uses the [CMake](https://cmake.org/) build system. Follow these steps to build the project:

```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This builds three artifacts:
 - **libmeos&#46;so** - the compiled shared object for use in C/C++ projects.
 - **meos-tests** - executable to run tests.
 - **libmeos.cpython-36m-x86_64-linux-gnu.so** - python module bindings.

If you make any additional changes, you can just run `make` to get your changes reflected in the compiled outputs.

## Running tests

After building, from the build directory, run:
```sh
$ ./tests/meos-tests
```
If everything is working okay, you should see "All tests passed".

MEOS uses the [Catch2](https://github.com/catchorg/Catch2) library for its tests.

### Testing the python bindings

There are no strict tests yet for the python bindings. However you can run an example script file, making this a sanity test at least.

After building, from the tests/python directory, run:
```sh
$ python3.6 test.py
```
