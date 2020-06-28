import datetime
import pytest

from pymeos.time import Period

from ..utils import unix_dt


@pytest.mark.parametrize("actual", [
    Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2)),
    Period("2011-01-01", "2011-01-02"),
    Period("[2011-01-01, 2011-01-02T00:00:00+0000)"),
])
def test_different_constructors(actual):
    assert actual.lower == unix_dt(2011, 1, 1)
    assert actual.upper == unix_dt(2011, 1, 2)
    assert actual.lower_inc == True
    assert actual.upper_inc == False


def test_constructor():
    period_1 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))
    period_2 = Period(unix_dt(2011, 1, 5), unix_dt(2011, 1, 6), False, False)
    period_3 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2), True, False)
    assert period_1 == period_3
    assert period_2 != period_1
    assert period_2 != period_3

    assert period_1.lower == unix_dt(2011, 1, 1)
    assert period_1.upper == unix_dt(2011, 1, 2)
    assert period_1.lower_inc == True
    assert period_1.upper_inc == False

    assert period_2.lower == unix_dt(2011, 1, 5)
    assert period_2.upper == unix_dt(2011, 1, 6)
    assert period_2.lower_inc == False
    assert period_2.upper_inc == False


def test_functions():
    period_1 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))
    assert period_1.timespan == datetime.timedelta(days=1)
    assert period_1.contains_timestamp(unix_dt(2011, 1, 1, 10))


def test_str():
    p = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))
    assert str(p) == "[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)"
