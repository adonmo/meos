import pytest
from pymeos import Geometry
from pymeos.temporal import (TemporalDuration, TInstantBool, TInstantGeom,
                             TInstantInt, TInstantSetBool, TInstantSetGeom,
                             TInstantSetInt)

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
    assert actual.duration == TemporalDuration.InstantSet
    assert actual.duration.name == 'InstantSet'
    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantInt(20, unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantInt(10, unix_dt(2020, 9, 10))


@pytest.mark.parametrize("expected_srid, actual", [
    (0,    TInstantSetGeom({TInstantGeom(Geometry(20, 30), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32), unix_dt(2019, 9, 10))})),
    (4326, TInstantSetGeom({TInstantGeom(Geometry(20, 30), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32), unix_dt(2019, 9, 10))}, 4326)),
    (4326, TInstantSetGeom({TInstantGeom(Geometry(20, 30, 4326), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 4326), unix_dt(2019, 9, 10))})),
    (4326, TInstantSetGeom({TInstantGeom(Geometry(20, 30, 4326), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 4326), unix_dt(2019, 9, 10))}, 0)),
    (4326, TInstantSetGeom({TInstantGeom(Geometry(20, 30, 4326), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 4326), unix_dt(2019, 9, 10))}, 4326)),

    (0,    TInstantSetGeom({"POINT (20 30)@2020-09-10 01:00:00+01", "POINT (24 32)@2019-09-10 01:00:00+01"})),
    (4326, TInstantSetGeom({"POINT (20 30)@2020-09-10 01:00:00+01", "POINT (24 32)@2019-09-10 01:00:00+01"}, 4326)),
    (4326, TInstantSetGeom({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"})),
    (4326, TInstantSetGeom({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, 0)),
    (4326, TInstantSetGeom({"SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01"}, 4326)),

    (0,    TInstantSetGeom("{POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01}")),
    (4326, TInstantSetGeom("{POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01}", 4326)),
    (4326, TInstantSetGeom("{SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01}")),
    (4326, TInstantSetGeom("{SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01}", 0)),
    (4326, TInstantSetGeom("{SRID=4326;POINT (20 30)@2020-09-10 01:00:00+01, SRID=4326;POINT (24 32)@2019-09-10 01:00:00+01}", 4326)),
    (4326, TInstantSetGeom("SRID=4326;{POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01}")),
    (4326, TInstantSetGeom("SRID=4326;{POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01}", 0)),
    (4326, TInstantSetGeom("SRID=4326;{POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01}", 4326)),
])
def test_different_geom_constructors(expected_srid, actual):
    assert actual.duration == TemporalDuration.InstantSet
    assert actual.duration.name == 'InstantSet'
    assert len(actual.instants) == 2
    assert actual.startInstant == TInstantGeom(Geometry(24, 32, expected_srid), unix_dt(2019, 9, 10))
    assert actual.endInstant == TInstantGeom(Geometry(20, 30, expected_srid), unix_dt(2020, 9, 10))
    assert actual.srid == expected_srid
    assert actual.startValue.srid == expected_srid


@pytest.mark.parametrize("args", [
    ({TInstantGeom(Geometry(20, 30, 5676), unix_dt(2020, 9, 10)), TInstantGeom(Geometry(24, 32, 5676), unix_dt(2019, 9, 10))}, 4326),
    ({"SRID=5676;POINT (20 30)@2020-09-10 01:00:00+01", "SRID=5676;POINT (24 32)@2019-09-10 01:00:00+01"}, 4326),
    ("{SRID=5676;POINT (20 30)@2020-09-10 01:00:00+01, SRID=5676;POINT (24 32)@2019-09-10 01:00:00+01}", 4326),
    ("SRID=5676;{POINT (20 30)@2020-09-10 01:00:00+01, POINT (24 32)@2019-09-10 01:00:00+01}", 4326),
])
def test_constructors_with_conflicting_srids(args):
    with pytest.raises(ValueError, match="Conflicting SRIDs provided. Given: 4326, while Geometry contains: 5676"):
        TInstantSetGeom(*args)


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
    assert repr(tsetb) == '{t@2011-01-01T00:00:00+0000, t@2011-01-02T00:00:00+0000}'
