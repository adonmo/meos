import pytest

from pymeos.temporal import Interpolation, TInstantFloat, TSequenceFloat, TemporalDuration

from ..utils import unix_dt


def get_sample_tsequence():
    tf1 = TInstantFloat(1.25, unix_dt(2011, 1, 1))
    tf2 = TInstantFloat(6.25, unix_dt(2011, 1, 2))
    return TSequenceFloat({tf1, tf2}, False, True)


@pytest.mark.parametrize("actual", [
    TSequenceFloat({TInstantFloat(10, unix_dt(2020, 9, 10)), TInstantFloat(20, unix_dt(2019, 9, 10))}, False, True),
    TSequenceFloat({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, False, True),
    TSequenceFloat("(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]"),
    TSequenceFloat("Interp=Linear;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]"),
])
def test_different_constructors(actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'

    assert actual.interpolation == Interpolation.Linear
    assert actual.interpolation.name == 'Linear'

    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantFloat(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantFloat(10, unix_dt(2020, 9, 10))

    assert actual.lower_inc == False
    assert actual.upper_inc == True


def test_constructor():
    tseqf = get_sample_tsequence()
    assert tseqf.instants == {
      TInstantFloat(1.25, unix_dt(2011, 1, 1)),
      TInstantFloat(6.25, unix_dt(2011, 1, 2)),
    }
    assert (tseqf.lower_inc, tseqf.upper_inc) == (False, True)


def test_str():
    tseqf = get_sample_tsequence()
    assert str(tseqf) == '(1.25@2011-01-01T00:00:00+0000, 6.25@2011-01-02T00:00:00+0000]'
