from pymeos.temporal import TInstantBool, TInstantSetBool

from ..utils import unix_dt


def get_sample_tinstant_set():
    tb1 = TInstantBool(True, unix_dt(2011, 1, 1))
    tb2 = TInstantBool(True, unix_dt(2011, 1, 2))
    return TInstantSetBool({tb1, tb2})


def test_constructor():
    tb1 = TInstantBool(True, unix_dt(2011, 1, 1))
    tb2 = TInstantBool(True, unix_dt(2011, 1, 2))
    tb3 = TInstantBool(True, unix_dt(2011, 1, 1)) # Repeating
    tsetb = TInstantSetBool({tb1, tb2, tb3})

    instants = tsetb.getInstants()
    assert len(instants) == 2
    assert instants == {tb1, tb2}


def test_str():
    tsetb = get_sample_tinstant_set()
    assert str(tsetb) == '{t@2011-01-01T00:00:00+0000, t@2011-01-02T00:00:00+0000}'
