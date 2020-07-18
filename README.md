[![Tests Status](https://github.com/adonmo/meos/workflows/Tests/badge.svg)](https://github.com/adonmo/meos/actions) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/7e51e8a09bdd424f83af953585fd14b9)](https://www.codacy.com/gh/adonmo/meos?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=adonmo/meos&amp;utm_campaign=Badge_Grade) [![MIT License](https://img.shields.io/github/license/adonmo/meos.svg)](https://github.com/adonmo/meos/blob/master/LICENSE.txt)

![MEOS Logo](https://i.imgur.com/ppfYUeT.png)

**MEOS (Mobility Engine, Open Source)** is a C++ library which makes it easy to work with temporal and spatio-temporal data. It is based on [MobilityDB](https://github.com/ULB-CoDE-WIT/MobilityDB)'s data types and functions. MEOS's code is heavily inspired from a similar library called [GEOS](https://github.com/libgeos/geos) - hence the name. This repository also includes **PyMEOS** - python bindings to MEOS using [pybind11](https://github.com/pybind/pybind11).

⚠️ Currently this library is an early, **experimental** stage - breaking changes might occur as it evolves.

## Design goals and tenets
- Extensibility
- Ease of use and getting started
- Interoperability
- Functionality over performance (for now)

## Projects built using MEOS
MEOS aims to the base on which more libraries can be built. Right now work is underway for the following to be rewritten on top of MEOS/PyMEOS:
- [mobilitydb-sqlalchemy](mobilitydb-sqlalchemy) - The SQLAchemy bindings for MobilityDB. (WIP in [meos branch](https://github.com/adonmo/mobilitydb-sqlalchemy/tree/meos))
- [MobilityDB-python](https://github.com/MobilityDB/MobilityDB-python/) - The official python driver for MobilityDB. (WIP [pull request](https://github.com/MobilityDB/MobilityDB-python/pull/4))

# Usage

## Python

### Installation

```sh
pip install pymeos
```

Note: libgeos-dev might need to be installed on your system.

### Code sample

```python
import datetime
from pymeos import Geometry
from pymeos.temporal import TInstantGeom, TSequenceGeom


def datetime_utc(year, month, day, hour=0, minute=0, second=0):
    return datetime.datetime(year, month, day, hour, minute, second, tzinfo=datetime.timezone.utc)


# Example creation of trajectory (temporal sequence of geometries)
trajectory = TSequenceGeom({
    TInstantGeom(Geometry(0, 0), datetime_utc(2012, 1, 1, 8, 0)),
    TInstantGeom(Geometry(2, 0), datetime_utc(2012, 1, 1, 8, 10)),
    TInstantGeom(Geometry(2, 1), datetime_utc(2012, 1, 1, 8, 15)),
})

print(trajectory)
```

```python
[POINT (0 0)@2012-01-01T08:00:00+0000, POINT (2 0)@2012-01-01T08:10:00+0000, POINT (2 1)@2012-01-01T08:15:00+0000)
```

Some more operations over this data:
```python
# Work with individual
>>> trajectory.instants
{POINT (2 0)@2012-01-01T08:10:00+0000, POINT (2 1)@2012-01-01T08:15:00+0000, POINT (0 0)@2012-01-01T08:00:00+0000}

>>> trajectory.startValue
POINT (0 0)

>>> trajectory.endValue
POINT (2 1)

# Extract just the temporal aspect
>>> trajectory.timestamps
{datetime.datetime(2012, 1, 1, 8, 15, tzinfo=datetime.timezone.utc), datetime.datetime(2012, 1, 1, 8, 10, tzinfo=datetime.timezone.utc), datetime.datetime(2012, 1, 1, 8, 0, tzinfo=datetime.timezone.utc)}
```

### Documentation

Docs and API Reference: [https://pymeos.netlify.app](https://pymeos.netlify.app)

More detailed usage guide/quickstart: [https://pymeos.netlify.app/quickstart.html](https://pymeos.netlify.app/quickstart.html)

## C++

```cpp
#include <iostream>
#include <meos/types/temporal/TSequence.hpp>
#include "time_utils.cpp"
using namespace std;


int main() {
    set<TInstant<int>> instants = {
        TInstant<int>(2, unix_time_point(2012, 1, 1)),
        TInstant<int>(1, unix_time_point(2012, 1, 2)),
        TInstant<int>(4, unix_time_point(2012, 1, 3)),
        TInstant<int>(3, unix_time_point(2012, 1, 4)),
    };
    TSequence<int> tseq(instants);
    cout << tseq << endl;
    return 0;
}
```

```python
[POINT (0 0)@2012-01-01T08:00:00+0000, POINT (2 0)@2012-01-01T08:10:00+0000, POINT (2 1)@2012-01-01T08:15:00+0000)
```

### Example

Minimalistic C++ app example: [https://github.com/adonmo/meos-cpp-example](https://github.com/adonmo/meos-cpp-example)

### Documentation

C++ API Reference: [https://meos.netlify.app](https://meos.netlify.app)

## Contributing

Issues and pull requests are welcome.

* For proposing new features/improvements or reporting bugs, [create an issue](https://github.com/adonmo/meos/issues/new/choose).
* Check [open issues](https://github.com/adonmo/meos/issues) for viewing existing ideas, verify if it is already proposed/being worked upon.
* Instruction on how to build, test and generate documentation can be found in [DEVELOPMENT.md](DEVELOPMENT.md)
* When implementing new features make sure to add relevant tests and documentation before sending pull requests.
