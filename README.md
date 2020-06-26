MEOS -- Mobility Engine, Open Source
====

MEOS or libmeos is a library which makes it easy to work with temporal and spatio-temporal data. It is based on [MobilityDB](https://github.com/ULB-CoDE-WIT/MobilityDB)'s data types. MEOS's code is heavily inspired from a similar library called [GEOS](https://github.com/libgeos/geos). MEOS is written in modern C++ with bindings to python using [pybind11](https://github.com/pybind/pybind11).

⚠️ Currently this library is in very early, completely **experimental** stages - heavy changes are to be expected.

# Installation

The package can be installed by running:

```sh
$ pip install git+https://github.com/chaitan94/meos
```

# Usage (Python)

### For all below examples, the following imports and helpers are assumed, in the favor of brevity.
```python
import pytz
from datetime import datetime
from pymeos import Geometry
from pymeos.io import (DeserializerFloat, DeserializerGeom, DeserializerInt,
                       SerializerFloat, SerializerInt)
from pymeos.temporal import (TInstantBool, TInstantFloat, TInstantGeom,
                             TInstantInt, TInstantSetBool, TInstantSetInt,
                             TInstantText, TSequenceFloat)
from pymeos.time import Period, PeriodSet


def epoch(year, month, day):
    return int(datetime(year, month, day).replace(tzinfo=pytz.UTC).timestamp() * 1000)


def unix_dt(year, month, day, hour=0, minute=0, second=0):
    return pytz.UTC.localize(datetime.datetime(year, month, day, hour, minute, second))
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
assert (tb.getValue(), ti.getTimestamp()) == (True, 1293840000000)
assert (ti.getValue(), ti.getTimestamp()) == (10, 1293840000000)
assert (tf.getValue(), ti.getTimestamp()) == (1.25, 1293840000000)
assert (tt.getValue(), tt.getTimestamp()) == ("testing", 1293840000000)
assert (tg.getValue().toWKT(), tg.getTimestamp()) == ("POINT (10 15)", 1293840000000)

assert {tb} == tsetb.getInstants()

assert [tf] == tseqf.getInstants()
assert (tseqf.lower_inc, tseqf.upper_inc) == (False, True)
```

## 2. Serialization
```python
# Let's get some temporal objects ready, which we can use to show serialization examples
ti1 = TInstantInt(10, epoch(2011, 1, 1))
ti2 = TInstantInt(20, epoch(2019, 1, 1))
tseti = TInstantSetInt({ti1, ti2})

tf1 = TInstantFloat(1.0, epoch(2011, 1, 1))
tf2 = TInstantFloat(2.5, epoch(2011, 1, 2))
tseqf = TSequenceFloat([tf1, tf2], True, False)

# Example serialization of these objects
si = SerializerInt()
assert (si.write(ti1) == "10@2011-01-01T00:00:00+0000")

sf = SerializerFloat()
assert (sf.write(tseqf) == "[1@2011-01-01T00:00:00+0000, 2.5@2011-01-02T00:00:00+0000)")

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
assert (ti.getValue(), ti.getTimestamp()) == (10, epoch(2011, 1, 1))

df = DeserializerFloat("{1.0@2011-01-01, 2.5@2011-01-02}")
tset = df.nextTInstantSet()
actual = {(tf.getValue(), tf.getTimestamp()) for tf in tset.getInstants()}
expected = {(1.0, epoch(2011, 1, 1)), (2.5, epoch(2011, 1, 2))}
assert actual == expected

dg = DeserializerGeom("[POINT(0 0)@2012-01-01 08:00:00+00, POINT(2 0)@2012-01-01 08:10:00+00, POINT(2 -1.98)@2012-01-01 08:15:00+00]")
tseq = dg.nextTSequence()
assert (tseq.lower_inc, tseq.upper_inc) == (True, True)
actual = [(tg.getValue().toWKT(), tg.getTimestamp()) for tg in tseq.getInstants()]
expected = [('POINT (0 0)', epoch(2012, 1, 1, 8)), ('POINT (2 0)', epoch(2012, 1, 1, 8, 10)), ('POINT (2 -1.98)', epoch(2012, 1, 1, 8, 15))]
assert actual == expected
```

### 4. Time Types
```python
# Example creation of time objects
period_1 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))
period_2 = Period(unix_dt(2011, 1, 5), unix_dt(2011, 1, 6), False, False)
period_3 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2), True, False)
assert period_1 == period_3
assert period_2 != period_1
assert period_2 != period_3
period_set = PeriodSet({period_1, period_2, period_3})

# Let's verify what we've done
assert period_1.lower == unix_dt(2011, 1, 1)
assert period_1.upper == unix_dt(2011, 1, 2)
assert period_1.lower_inc == True
assert period_1.upper_inc == False

assert period_2.lower == unix_dt(2011, 1, 5)
assert period_2.upper == unix_dt(2011, 1, 6)
assert period_2.lower_inc == False
assert period_2.upper_inc == False

assert len(period_set.periods()) == 2
assert {period_1, period_2} == period_set.periods()

# Test functions
assert period_1.timespan == datetime.timedelta(days=1)
assert period_1.contains_timestamp(unix_dt(2011, 1, 1, 10))
# assert period_set.period == Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 6), True, False)

# Serialization
assert str(period_1) == "[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)"
assert SerializerInt().write(period_1) == "[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)"

# Deserialization
di = DeserializerInt("[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)")
period = di.nextPeriod()
assert period == period_1

di = DeserializerInt("[2019-09-02 01:00:00+01, 2019-09-03 05:43:21+01)")
period = di.nextPeriod()
assert period.lower == unix_dt(2019, 9, 2)
assert period.upper == unix_dt(2019, 9, 3, 4, 43, 21)
assert period.lower_inc == True
assert period.upper_inc == False
```

# Development
MEOS uses the [CMake](https://cmake.org/) build system, [Catch2](https://github.com/catchorg/Catch2) and [pytest](https://docs.pytest.org/en/latest/) for tests.

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
$ pip -m venv ENV
$ source ./ENV/bin/activate
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
