import pytest

from pymeos import Geometry
from pymeos.temporal import (Interpolation, TemporalDuration, TInstantFloat,
                             TInstantGeom, TInstantInt, TSequenceFloat,
                             TSequenceGeom, TSequenceInt)

from ..utils import unix_dt


def get_sample_tsequence():
    tf1 = TInstantFloat(1.25, unix_dt(2011, 1, 1))
    tf2 = TInstantFloat(6.25, unix_dt(2011, 1, 2))
    return TSequenceFloat({tf1, tf2}, False, True)


@pytest.mark.parametrize("actual", [
    TSequenceInt({TInstantInt(10, unix_dt(2020, 9, 10)), TInstantInt(20, unix_dt(2019, 9, 10))}),
    TSequenceInt({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}),
    TSequenceInt({TInstantInt(10, unix_dt(2020, 9, 10)), TInstantInt(20, unix_dt(2019, 9, 10))}, True, False, Interpolation.Stepwise),
    TSequenceInt({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, True, False, Interpolation.Stepwise),
    TSequenceInt("[10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01)"),
    TSequenceInt("Interp=Stepwise;[10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01)"),
])
def test_different_constructors_with_discrete_base_type(actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'

    assert actual.interpolation == Interpolation.Stepwise
    assert actual.interpolation.name == 'Stepwise'

    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantInt(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantInt(10, unix_dt(2020, 9, 10))

    assert actual.lower_inc == True
    assert actual.upper_inc == False

    assert str(actual) == "[20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000)"
    assert repr(actual) == "[20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000)"


@pytest.mark.parametrize("actual", [
    TSequenceFloat({TInstantFloat(10, unix_dt(2020, 9, 10)), TInstantFloat(20, unix_dt(2019, 9, 10))}, False, True),
    TSequenceFloat({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, False, True),
    TSequenceFloat({TInstantFloat(10, unix_dt(2020, 9, 10)), TInstantFloat(20, unix_dt(2019, 9, 10))}, False, True, Interpolation.Linear),
    TSequenceFloat({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, False, True, Interpolation.Linear),
    TSequenceFloat("(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]"),
    TSequenceFloat("Interp=Linear;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]"),
])
def test_different_constructors_with_continuous_base_type(actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'

    assert actual.interpolation == Interpolation.Linear
    assert actual.interpolation.name == 'Linear'

    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantFloat(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantFloat(10, unix_dt(2020, 9, 10))

    assert actual.lower_inc == False
    assert actual.upper_inc == True

    assert str(actual) == "(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"
    assert repr(actual) == "(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"


@pytest.mark.parametrize("expected_srid, actual", [
    (0,    TSequenceGeom({TInstantGeom(Geometry(20, 30), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32), unix_dt(2019, 9, 10))}, True, False)),
    (4326, TSequenceGeom({TInstantGeom(Geometry(20, 30), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32), unix_dt(2019, 9, 10))}, True, False, 4326)),
    (4326, TSequenceGeom({TInstantGeom(Geometry(20, 30, 4326), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 4326), unix_dt(2019, 9, 10))}, True, False)),
    (4326, TSequenceGeom({TInstantGeom(Geometry(20, 30, 4326), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 4326), unix_dt(2019, 9, 10))}, True, False, 0)),
    (4326, TSequenceGeom({TInstantGeom(Geometry(20, 30, 4326), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 4326), unix_dt(2019, 9, 10))}, True, False, 4326)),

    (0,    TSequenceGeom({"POINT (20 30)@2020-09-10 01:00:00+01", "POINT (24 32)@2019-09-10 01:00:00+01"}, True, False)),
    (4326, TSequenceGeom({"POINT (20 30)@2020-09-10 01:00:00+01", "POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 4326)),
    (4326, TSequenceGeom({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False)),
    (4326, TSequenceGeom({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 0)),
    (4326, TSequenceGeom({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 4326)),

    (0,    TSequenceGeom("[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)")),
    (4326, TSequenceGeom("[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 4326)),
    (4326, TSequenceGeom("[SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01)")),
    (4326, TSequenceGeom("[SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01)", 0)),
    (4326, TSequenceGeom("[SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01)", 4326)),
    (4326, TSequenceGeom("SRID=4326;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)")),
    (4326, TSequenceGeom("SRID=4326;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 0)),
    (4326, TSequenceGeom("SRID=4326;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 4326)),
])
def test_different_geom_constructors(expected_srid, actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'
    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantGeom(Geometry(24, 32, expected_srid), unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantGeom(Geometry(20, 30, expected_srid), unix_dt(2020, 9, 10))
    assert actual.lower_inc == True
    assert actual.upper_inc == False
    assert actual.srid == expected_srid
    assert actual.startValue.srid == expected_srid


@pytest.mark.parametrize("args", [
    ({TInstantGeom(Geometry(20, 30, 5676), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 5676), unix_dt(2019, 9, 10))}, True, False, 4326),
    ({"SRID=5676;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=5676;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 4326),
    ("[SRID=5676;POINT (20 30)@2020-09-10 01:00:00+01, SRID=5676;POINT (24 32)@2019-09-10 01:00:00+01)", 4326),
    ("SRID=5676;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 4326),
])
def test_constructors_with_conflicting_srids(args):
    with pytest.raises(ValueError, match="Conflicting SRIDs provided. Given: 4326, while Geometry contains: 5676"):
        TSequenceGeom(*args)


def test_bad_constructors():
    with pytest.raises(ValueError, match="Unsupported interpolation specified: Invalid"):
        TSequenceInt("Interp=Invalid;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]")

    with pytest.raises(ValueError, match="Cannot assign linear interpolation to a discrete base type"):
        TSequenceInt("Interp=Linear;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]")


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
    assert repr(tseqf) == '(1.25@2011-01-01T00:00:00+0000, 6.25@2011-01-02T00:00:00+0000]'

    tseqf = TSequenceFloat({TInstantFloat(10, unix_dt(2020, 9, 10)), TInstantFloat(20, unix_dt(2019, 9, 10))}, False, True, Interpolation.Stepwise)
    assert str(tseqf) == "Interp=Stepwise;(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"
    assert repr(tseqf) == "Interp=Stepwise;(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"
