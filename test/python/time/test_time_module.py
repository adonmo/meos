import datetime

import pytz

from pymeos.io import SerializerInt, DeserializerInt
from pymeos.time import Period, PeriodSet


def unix_dt(year, month, day, hour=0, minute=0, second=0):
    return pytz.UTC.localize(datetime.datetime(year, month, day, hour, minute, second))


def test_time_types():
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
