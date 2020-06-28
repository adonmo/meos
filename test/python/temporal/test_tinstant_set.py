import pytest

from pymeos.temporal import TInstantBool, TInstantSetBool, TInstantInt, TInstantSetInt

from ..utils import unix_dt


def get_sample_tinstant_set():
    tb1 = TInstantBool(True, unix_dt(2011, 1, 1))
    tb2 = TInstantBool(True, unix_dt(2011, 1, 2))
    return TInstantSetBool({tb1, tb2})


@pytest.mark.parametrize("actual", [
    TInstantSetInt({TInstantInt(10, unix_dt(2020, 9, 10)), TInstantInt(20, unix_dt(2019, 9, 10))}),
    TInstantSetInt({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}),
    TInstantSetInt("{10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01}"),
])
def test_different_constructors(actual):
    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantInt(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantInt(10, unix_dt(2020, 9, 10))


def test_constructor():
    tb1 = TInstantBool(True, unix_dt(2011, 1, 1))
    tb2 = TInstantBool(True, unix_dt(2011, 1, 2))
    tb3 = TInstantBool(True, unix_dt(2011, 1, 1)) # Repeating
    tsetb = TInstantSetBool({tb1, tb2, tb3})

    instants = tsetb.instants
    assert len(instants) == 2
    assert instants == {tb1, tb2}


def test_str():
    tsetb = get_sample_tinstant_set()
    assert str(tsetb) == '{t@2011-01-01T00:00:00+0000, t@2011-01-02T00:00:00+0000}'
