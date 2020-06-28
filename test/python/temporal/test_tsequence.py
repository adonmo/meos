from pymeos.temporal import TInstantFloat, TSequenceFloat

from ..utils import unix_dt


def get_sample_tsequence():
    tf1 = TInstantFloat(1.25, unix_dt(2011, 1, 1))
    tf2 = TInstantFloat(6.25, unix_dt(2011, 1, 2))
    return TSequenceFloat([tf1, tf2], False, True)


def test_constructor():
    tseqf = get_sample_tsequence()
    assert tseqf.getInstants == [
      TInstantFloat(1.25, unix_dt(2011, 1, 1)),
      TInstantFloat(6.25, unix_dt(2011, 1, 2)),
    ]
    assert tseqf.instants == {
      TInstantFloat(1.25, unix_dt(2011, 1, 1)),
      TInstantFloat(6.25, unix_dt(2011, 1, 2)),
    }
    assert (tseqf.lower_inc, tseqf.upper_inc) == (False, True)


def test_str():
    tseqf = get_sample_tsequence()
    assert str(tseqf) == '(1.25@2011-01-01T00:00:00+0000, 6.25@2011-01-02T00:00:00+0000]'
