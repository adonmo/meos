import pytest

from pymeos import GeomPoint
from pymeos.io import DeserializerGeom
from pymeos.temporal import (Interpolation, TemporalDuration, TFloatInst,
                             TGeomPointInst, TIntInst, TFloatSeq,
                             TGeomPointSeq, TIntSeq)

from ..utils import unix_dt


def get_sample_tsequence():
    tf1 = TFloatInst(1.25, unix_dt(2011, 1, 1))
    tf2 = TFloatInst(6.25, unix_dt(2011, 1, 2))
    return TFloatSeq({tf1, tf2}, False, True)


@pytest.mark.parametrize("actual", [
    TIntSeq({TIntInst(10, unix_dt(2020, 9, 10)), TIntInst(20, unix_dt(2019, 9, 10))}),
    TIntSeq({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}),
    TIntSeq({TIntInst(10, unix_dt(2020, 9, 10)), TIntInst(20, unix_dt(2019, 9, 10))}, True, False, Interpolation.Stepwise),
    TIntSeq({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, True, False, Interpolation.Stepwise),
    TIntSeq("[10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01)"),
    TIntSeq("Interp=Stepwise;[10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01)"),
])
def test_different_constructors_with_discrete_base_type(actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'

    assert actual.interpolation == Interpolation.Stepwise
    assert actual.interpolation.name == 'Stepwise'

    assert len(actual.instants) == 2
    assert actual.startInstant == TIntInst(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TIntInst(10, unix_dt(2020, 9, 10))

    assert actual.lower_inc == True
    assert actual.upper_inc == False

    assert str(actual) == "[20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000)"
    assert repr(actual) == "[20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000)"


@pytest.mark.parametrize("actual", [
    TFloatSeq({TFloatInst(10, unix_dt(2020, 9, 10)), TFloatInst(20, unix_dt(2019, 9, 10))}, False, True),
    TFloatSeq({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, False, True),
    TFloatSeq({TFloatInst(10, unix_dt(2020, 9, 10)), TFloatInst(20, unix_dt(2019, 9, 10))}, False, True, Interpolation.Linear),
    TFloatSeq({"10@2020-09-10 01:00:00+01", "20@2019-09-10 01:00:00+01"}, False, True, Interpolation.Linear),
    TFloatSeq("(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]"),
    TFloatSeq("Interp=Linear;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]"),
])
def test_different_constructors_with_continuous_base_type(actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'

    assert actual.interpolation == Interpolation.Linear
    assert actual.interpolation.name == 'Linear'

    assert len(actual.instants) == 2
    assert actual.startInstant == TFloatInst(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TFloatInst(10, unix_dt(2020, 9, 10))

    assert actual.lower_inc == False
    assert actual.upper_inc == True

    assert str(actual) == "(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"
    assert repr(actual) == "(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"


@pytest.mark.parametrize("expected_srid, actual", [
    (0,    TGeomPointSeq({TGeomPointInst(GeomPoint(20, 30), unix_dt(2020, 9, 10)), TGeomPointInst(GeomPoint(24, 32), unix_dt(2019, 9, 10))}, True, False)),
    (4326, TGeomPointSeq({TGeomPointInst(GeomPoint(20, 30), unix_dt(2020, 9, 10)), TGeomPointInst(GeomPoint(24, 32), unix_dt(2019, 9, 10))}, True, False, 4326)),
    (4326, TGeomPointSeq({TGeomPointInst(GeomPoint(20, 30, 4326), unix_dt(2020, 9, 10)), TGeomPointInst(GeomPoint(24, 32, 4326), unix_dt(2019, 9, 10))}, True, False)),
    (4326, TGeomPointSeq({TGeomPointInst(GeomPoint(20, 30, 4326), unix_dt(2020, 9, 10)), TGeomPointInst(GeomPoint(24, 32, 4326), unix_dt(2019, 9, 10))}, True, False, 0)),
    (4326, TGeomPointSeq({TGeomPointInst(GeomPoint(20, 30, 4326), unix_dt(2020, 9, 10)), TGeomPointInst(GeomPoint(24, 32, 4326), unix_dt(2019, 9, 10))}, True, False, 4326)),

    (0,    TGeomPointSeq({"POINT (20 30)@2020-09-10 01:00:00+01", "POINT (24 32)@2019-09-10 01:00:00+01"}, True, False)),
    (4326, TGeomPointSeq({"POINT (20 30)@2020-09-10 01:00:00+01", "POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 4326)),
    (4326, TGeomPointSeq({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False)),
    (4326, TGeomPointSeq({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 0)),
    (4326, TGeomPointSeq({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 4326)),

    (0,    TGeomPointSeq("[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)")),
    (4326, TGeomPointSeq("[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 4326)),
    (4326, TGeomPointSeq("[SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01)")),
    (4326, TGeomPointSeq("[SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01)", 0)),
    (4326, TGeomPointSeq("[SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01)", 4326)),
    (4326, TGeomPointSeq("SRID=4326;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)")),
    (4326, TGeomPointSeq("SRID=4326;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 0)),
    (4326, TGeomPointSeq("SRID=4326;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 4326)),
])
def test_different_geom_constructors(expected_srid, actual):
    assert actual.duration == TemporalDuration.Sequence
    assert actual.duration.name == 'Sequence'
    assert len(actual.instants) == 2
    assert actual.startInstant == TGeomPointInst(GeomPoint(24, 32, expected_srid), unix_dt(2019, 9, 10))
    assert actual.endInstant == TGeomPointInst(GeomPoint(20, 30, expected_srid), unix_dt(2020, 9, 10))
    assert actual.lower_inc == True
    assert actual.upper_inc == False
    assert actual.srid == expected_srid
    assert actual.startInstant.srid == expected_srid
    assert actual.startValue.srid == expected_srid


@pytest.mark.parametrize("args", [
    ({TGeomPointInst(GeomPoint(20, 30, 5676), unix_dt(2020, 9, 10)), TGeomPointInst(GeomPoint(24, 32, 5676), unix_dt(2019, 9, 10))}, True, False, 4326),
    ({"SRID=5676;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=5676;POINT (24 32)@2019-09-10 01:00:00+01"}, True, False, 4326),
    ("[SRID=5676;POINT (20 30)@2020-09-10 01:00:00+01, SRID=5676;POINT (24 32)@2019-09-10 01:00:00+01)", 4326),
    ("SRID=5676;[POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01)", 4326),
])
def test_constructors_with_conflicting_srids(args):
    with pytest.raises(ValueError, match="Conflicting SRIDs provided. Given: 4326, while (Instant|Geometry) contains: 5676"):
        TGeomPointSeq(*args)


def test_bad_constructors():
    with pytest.raises(ValueError, match="Unsupported interpolation specified: Invalid"):
        TIntSeq("Interp=Invalid;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]")

    with pytest.raises(ValueError, match="Cannot assign linear interpolation to a discrete base type"):
        TIntSeq("Interp=Linear;(10@2020-09-10 01:00:00+01, 20@2019-09-10 01:00:00+01]")


def test_constructor():
    tseqf = get_sample_tsequence()
    assert tseqf.instants == {
      TFloatInst(1.25, unix_dt(2011, 1, 1)),
      TFloatInst(6.25, unix_dt(2011, 1, 2)),
    }
    assert (tseqf.lower_inc, tseqf.upper_inc) == (False, True)


@pytest.mark.parametrize('seq', [
    TGeomPointSeq('SRID=5676;[Point (20 20)@2019-09-02 00:00:00+01, Point (10 10)@2019-09-03 00:00:00+01]'),
    DeserializerGeom('SRID=5676;[Point (20 20)@2019-09-02 00:00:00+01, Point (10 10)@2019-09-03 00:00:00+01]').nextTemporal(),
    DeserializerGeom('SRID=5676;[Point (20 20)@2019-09-02 00:00:00+01, Point (10 10)@2019-09-03 00:00:00+01]').nextTSequence(),
])
def test_srid_is_maintained_in_instants(seq):
    assert seq.srid == 5676
    assert seq.interpolation == Interpolation.Linear
    for instant in seq.instants:
        assert instant.srid == seq.srid
    assert seq.instants == {
        TGeomPointInst('SRID=5676;Point (20 20)@2019-09-02 00:00:00+01'),
        TGeomPointInst('SRID=5676;Point (10 10)@2019-09-03 00:00:00+01'),
    }


def test_str():
    tseqf = get_sample_tsequence()
    assert str(tseqf) == '(1.25@2011-01-01T00:00:00+0000, 6.25@2011-01-02T00:00:00+0000]'
    assert repr(tseqf) == '(1.25@2011-01-01T00:00:00+0000, 6.25@2011-01-02T00:00:00+0000]'

    tseqf = TFloatSeq({TFloatInst(10, unix_dt(2020, 9, 10)), TFloatInst(20, unix_dt(2019, 9, 10))}, False, True, Interpolation.Stepwise)
    assert str(tseqf) == "Interp=Stepwise;(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"
    assert repr(tseqf) == "Interp=Stepwise;(20@2019-09-10T00:00:00+0000, 10@2020-09-10T00:00:00+0000]"
