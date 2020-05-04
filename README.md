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

# Usage (Python)

### For all below examples, the following imports and helpers are assumed, in the favor of brevity.
```python
import pytz
from datetime import datetime
from pymeos import TInstantBool, TInstantInt, TInstantFloat, TInstantText, TInstantGeom,\
    TInstantSetBool, TInstantSetInt, TSequenceFloat,\
    SerializerInt, SerializerFloat, DeserializerInt, DeserializerFloat, \
    Geometry, initGEOS, finishGEOS


def epoch(year, month, day):
    return int(datetime(year, month, day).replace(tzinfo=pytz.UTC).timestamp() * 1000)
```
## 1. Using the temporal data objects
### Example creation of the temporal objects (bool, int, float, text, geom) are supported
```python
# Example creation of temporal instant objects
tb = TInstantBool(True, epoch(2011, 1, 1))
ti = TInstantInt(10, epoch(2011, 1, 1))
tf = TInstantFloat(1.25, epoch(2011, 1, 1))
tt = TInstantText("testing", epoch(2011, 1, 1))
tg = TInstantGeom(Geometry(10.0, 15.0), epoch(2011, 1, 1))  # Spatiotemporal!

# Example creation of temporal instant set
tsetb = TInstantSetBool({tb})

# Example creation of temporal sequence
tseqf = TSequenceFloat([tf], False, True)
```

### Let's verify what we've done
```python
assert (ti.getValue(), ti.getT()) == (10, 1293840000000)
assert (tt.getValue(), tt.getT()) == ("testing", 1293840000000)
assert (tg.getValue().toWKT(), tg.getT()) == ("POINT (10 15)", 1293840000000)

tb = tsetb.getInstants().pop()
assert (tb.getValue(), tb.getT()) == (True, 1293840000000)

tf = tseqf.getInstants()[0]
assert (tseqf.left_open, tseqf.right_open) == (False, True)
assert (tf.getValue(), tf.getT()) == (1.25, 1293840000000)
```

## 2. Serialization
```python
# Let's get some temporal objects ready, which we can use to show serialization examples
ti1 = TInstantInt(10, epoch(2011, 1, 1))
ti2 = TInstantInt(20, epoch(2019, 1, 1))
tseti = TInstantSetInt({ti1, ti2})

tf1 = TInstantFloat(1.0, epoch(2011, 1, 1))
tf2 = TInstantFloat(2.5, epoch(2011, 1, 2))
tseqf = TSequenceFloat([tf1, tf2], False, True)

# Example serialization of these objects
si = SerializerInt()
assert (si.write(ti1) == "10@2011-01-01T00:00:00+0000")

sf = SerializerFloat()
assert (sf.write(tseqf) == "[1.000000@2011-01-01T00:00:00+0000, 2.500000@2011-01-02T00:00:00+0000)")

# For sets, ordering might vary, so we need to check accordingly
serialized = si.write(tseti)
assert len(serialized) == 58
assert serialized[0] == "{"
assert serialized[-1] == "}"
assert {"10@2011-01-01T00:00:00+0000", "20@2019-01-01T00:00:00+0000"} == set(serialized[1:-1].split(", "))
```

## 3. Deserialization
```python
di = DeserializerInt("10@2011-01-01")
ti = di.nextTInstant()
assert (ti.getValue(), ti.getT()) == (10, epoch(2011, 1, 1))

df = DeserializerFloat("{1.0@2011-01-01, 2.5@2011-01-02}")
tset = df.nextTInstantSet()
actual = {(tf.getValue(), tf.getT()) for tf in tset.getInstants()}
expected = {(1.0, epoch(2011, 1, 1)), (2.5, epoch(2011, 1, 2))}
assert actual == expected

di = DeserializerInt("[10@2011-01-01, 20@2011-01-02)")
tseq = di.nextTSequence()
assert (tseq.left_open, tseq.right_open) == (False, True)
actual = [(ti.getValue(), ti.getT()) for ti in tseq.getInstants()]
expected = [(10, epoch(2011, 1, 1)), (20, epoch(2011, 1, 2))]
assert actual == expected
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
