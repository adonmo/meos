import pytest

from pymeos.time import Period, PeriodSet

from ..utils import unix_dt


def get_sample_period():
    return Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))


@pytest.mark.parametrize("actual", [
    PeriodSet({get_sample_period()}),
    PeriodSet("{[2011-01-01, 2011-01-02T00:00:00+0000)}"),
    PeriodSet({"[2011-01-01, 2011-01-02T00:00:00+0000)"}),
])
def test_different_constructors(actual):
    assert len(actual.periods) == 1
    p = actual.startPeriod
    assert p.lower == unix_dt(2011, 1, 1)
    assert p.upper == unix_dt(2011, 1, 2)
    assert p.lower_inc == True
    assert p.upper_inc == False


def test_constructor():
    period_1 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))
    period_2 = Period(unix_dt(2011, 1, 5), unix_dt(2011, 1, 6), False, True)
    period_3 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2), True, False)
    period_set = PeriodSet({period_1, period_2, period_3})

    assert len(period_set.periods) == 2
    assert period_set.startPeriod == period_1
    assert period_set.endPeriod == period_2
    assert period_set.period == Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 6), True, True)
