import pytest
from pymeos import Geometry
from pymeos.temporal import (TemporalDuration, TInstantBool, TInstantFloat,
                             TInstantGeom, TInstantInt, TInstantText)

from ..utils import unix_dt


@pytest.mark.parametrize("actual", [
    TInstantInt(10, unix_dt(2011, 1, 1)),
    TInstantInt((10, unix_dt(2011, 1, 1))),
    TInstantInt("10", "2011-01-01"),
    TInstantInt(("10", "2011-01-01")),
    TInstantInt("10@2011-01-01"),
])
def test_different_int_constructors(actual):
    assert actual.duration == TemporalDuration.Instant
    assert actual.duration.name == 'Instant'
    assert actual.getValue == 10
    assert actual.getTimestamp == unix_dt(2011, 1, 1)


@pytest.mark.parametrize("actual, expected_srid", [
    (TInstantGeom(Geometry(20, 30), unix_dt(2011, 1, 1)), 0),
    (TInstantGeom((Geometry(20, 30), unix_dt(2011, 1, 1))), 0),
    (TInstantGeom("POINT (20 30)", "2011-01-01"), 0),
    (TInstantGeom(("POINT (20 30)", "2011-01-01")), 0),
    (TInstantGeom("POINT (20 30)@2011-01-01"), 0),
    (TInstantGeom(Geometry(20, 30), unix_dt(2011, 1, 1), 4326), 4326),
    (TInstantGeom((Geometry(20, 30), unix_dt(2011, 1, 1)), 4326), 4326),
    (TInstantGeom(Geometry(20, 30, 4326), unix_dt(2011, 1, 1)), 4326),
    (TInstantGeom((Geometry(20, 30, 4326), unix_dt(2011, 1, 1))), 4326),
    (TInstantGeom(Geometry(20, 30, 4326), unix_dt(2011, 1, 1), 4326), 4326),
    (TInstantGeom((Geometry(20, 30, 4326), unix_dt(2011, 1, 1)), 4326), 4326),
    (TInstantGeom("POINT (20 30)", "2011-01-01", 4326), 4326),
    (TInstantGeom(("POINT (20 30)", "2011-01-01"), 4326), 4326),
    (TInstantGeom("SRID=4326;POINT (20 30)", "2011-01-01"), 4326),
    (TInstantGeom(("SRID=4326;POINT (20 30)", "2011-01-01")), 4326),
    (TInstantGeom("SRID=4326;POINT (20 30)", "2011-01-01", 4326), 4326),
    (TInstantGeom(("SRID=4326;POINT (20 30)", "2011-01-01"), 4326), 4326),
    (TInstantGeom("SRID=4326;POINT (20 30)@2011-01-01"), 4326),
])
def test_different_geom_constructors(actual, expected_srid):
    assert actual.duration == TemporalDuration.Instant
    assert actual.duration.name == 'Instant'
    assert actual.getValue == Geometry(20, 30, expected_srid)
    assert actual.getTimestamp == unix_dt(2011, 1, 1)
    srid_prefix = 'SRID={};'.format(expected_srid) if expected_srid != 0 else ''
    assert str(actual) == srid_prefix + 'POINT (20 30)@2011-01-01T00:00:00+0000'
    assert actual.srid == expected_srid


def test_constructor_different_base_types():
    tb = TInstantBool(True, unix_dt(2011, 1, 1))
    ti = TInstantInt(10, unix_dt(2011, 1, 1))
    tf = TInstantFloat(1.25, unix_dt(2011, 1, 1))
    tt = TInstantText("testing", unix_dt(2011, 1, 1))
    tg = TInstantGeom(Geometry(10.0, 15.0), unix_dt(2011, 1, 1))

    assert (tb.getValue, ti.getTimestamp) == (True, unix_dt(2011, 1, 1))
    assert (ti.getValue, ti.getTimestamp) == (10, unix_dt(2011, 1, 1))
    assert (tf.getValue, ti.getTimestamp) == (1.25, unix_dt(2011, 1, 1))
    assert (tt.getValue, tt.getTimestamp) == ("testing", unix_dt(2011, 1, 1))
    assert (tg.getValue.toWKT(), tg.getTimestamp) == ("POINT (10 15)", unix_dt(2011, 1, 1))


def test_str():
    tg = TInstantGeom(Geometry(10.0, 15.0), unix_dt(2011, 1, 1))
    assert str(tg) == "POINT (10 15)@2011-01-01T00:00:00+0000"
