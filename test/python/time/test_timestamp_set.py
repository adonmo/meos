import pytest

from pymeos.time import Period, TimestampSet

from ..utils import unix_dt


@pytest.mark.parametrize("actual", [
    TimestampSet({unix_dt(2011, 1, 1), unix_dt(2011, 1, 2)}),
    TimestampSet("{2011-01-01, 2011-01-02T00:00:00+0000}"),
    TimestampSet({"2011-01-01", "2011-01-02T00:00:00+0000"}),
])
def test_different_constructors(actual):
    assert len(actual.timestamps) == 2
    assert actual.startTimestamp == unix_dt(2011, 1, 1)
    assert actual.endTimestamp == unix_dt(2011, 1, 2)
    assert actual.timestampN(0) == unix_dt(2011, 1, 1)
    assert actual.timestampN(1) == unix_dt(2011, 1, 2)


def test_constructor():
    t1 = unix_dt(2011, 1, 1)
    t2 = unix_dt(2011, 1, 5)
    t3 = unix_dt(2011, 1, 1)  # Duplicate!
    tset = TimestampSet({t1, t2, t3})

    assert len(tset.timestamps) == 2
    assert tset.startTimestamp == t1
    assert tset.endTimestamp == t2
    assert tset.timestampN(0) == t1
    assert tset.timestampN(1) == t2

    assert len(tset.periods) == 2
    assert tset.startPeriod == Period(t1, t1, True, True)
    assert tset.endPeriod == Period(t2, t2, True, True)
    assert tset.periodN(0) == Period(t1, t1, True, True)
    assert tset.periodN(1) == Period(t2, t2, True, True)
