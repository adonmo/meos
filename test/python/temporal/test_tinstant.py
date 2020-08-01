import pytest
from pymeos import GeomPoint
from pymeos.temporal import (TemporalDuration, TBoolInst, TFloatInst,
                             TGeomPointInst, TIntInst, TTextInst)

from ..utils import unix_dt


@pytest.mark.parametrize("actual", [
    TIntInst(10, unix_dt(2011, 1, 1)),
    TIntInst((10, unix_dt(2011, 1, 1))),
    TIntInst("10", "2011-01-01"),
    TIntInst(("10", "2011-01-01")),
    TIntInst("10@2011-01-01"),
])
def test_different_int_constructors(actual):
    assert actual.duration == TemporalDuration.Instant
    assert actual.duration.name == 'Instant'
    assert actual.getValue == 10
    assert actual.getTimestamp == unix_dt(2011, 1, 1)


@pytest.mark.parametrize("expected_srid, actual", [
    (0,    TGeomPointInst(GeomPoint(20, 30), unix_dt(2011, 1, 1))),
    (4326, TGeomPointInst(GeomPoint(20, 30), unix_dt(2011, 1, 1), 4326)),
    (4326, TGeomPointInst(GeomPoint(20, 30, 4326), unix_dt(2011, 1, 1))),
    (4326, TGeomPointInst(GeomPoint(20, 30, 4326), unix_dt(2011, 1, 1), 4326)),

    (0,    TGeomPointInst((GeomPoint(20, 30), unix_dt(2011, 1, 1)))),
    (4326, TGeomPointInst((GeomPoint(20, 30), unix_dt(2011, 1, 1)), 4326)),
    (4326, TGeomPointInst((GeomPoint(20, 30, 4326), unix_dt(2011, 1, 1)))),
    (4326, TGeomPointInst((GeomPoint(20, 30, 4326), unix_dt(2011, 1, 1)), 4326)),

    (0,    TGeomPointInst("POINT (20 30)", "2011-01-01")),
    (4326, TGeomPointInst("POINT (20 30)", "2011-01-01", 4326)),
    (4326, TGeomPointInst("SRID=4326;POINT (20 30)", "2011-01-01")),
    (4326, TGeomPointInst("SRID=4326;POINT (20 30)", "2011-01-01", 4326)),

    (0,    TGeomPointInst(("POINT (20 30)", "2011-01-01"))),
    (4326, TGeomPointInst(("POINT (20 30)", "2011-01-01"), 4326)),
    (4326, TGeomPointInst(("SRID=4326;POINT (20 30)", "2011-01-01"))),
    (4326, TGeomPointInst(("SRID=4326;POINT (20 30)", "2011-01-01"), 4326)),

    (0,    TGeomPointInst("POINT (20 30)@2011-01-01")),
    (4326, TGeomPointInst("POINT (20 30)@2011-01-01", 4326)),
    (4326, TGeomPointInst("SRID=4326;POINT (20 30)@2011-01-01")),
    (4326, TGeomPointInst("SRID=4326;POINT (20 30)@2011-01-01", 4326)),
])
def test_different_geom_constructors(expected_srid, actual):
    assert actual.duration == TemporalDuration.Instant
    assert actual.duration.name == 'Instant'
    assert actual.getValue == GeomPoint(20, 30, expected_srid)
    assert actual.getTimestamp == unix_dt(2011, 1, 1)
    srid_prefix = 'SRID={};'.format(expected_srid) if expected_srid != 0 else ''
    assert str(actual) == srid_prefix + 'POINT (20 30)@2011-01-01T00:00:00+0000'
    assert actual.srid == expected_srid


@pytest.mark.parametrize("args", [
    (GeomPoint(20, 30, 5676), unix_dt(2011, 1, 1), 4326),
    ((GeomPoint(20, 30, 5676), unix_dt(2011, 1, 1)), 4326),
    ("SRID=5676;POINT (20 30)", "2011-01-01", 4326),
    (("SRID=5676;POINT (20 30)", "2011-01-01"), 4326),
    ("SRID=5676;POINT (20 30)@2011-01-01", 4326),
])
def test_constructors_with_conflicting_srids(args):
    with pytest.raises(ValueError, match="Conflicting SRIDs provided. Given: 4326, while Geometry contains: 5676"):
        TGeomPointInst(*args)


def test_constructor_different_base_types():
    tb = TBoolInst(True, unix_dt(2011, 1, 1))
    ti = TIntInst(10, unix_dt(2011, 1, 1))
    tf = TFloatInst(1.25, unix_dt(2011, 1, 1))
    tt = TTextInst("testing", unix_dt(2011, 1, 1))
    tg = TGeomPointInst(GeomPoint(10.0, 15.0), unix_dt(2011, 1, 1))

    assert (tb.getValue, ti.getTimestamp) == (True, unix_dt(2011, 1, 1))
    assert (ti.getValue, ti.getTimestamp) == (10, unix_dt(2011, 1, 1))
    assert (tf.getValue, ti.getTimestamp) == (1.25, unix_dt(2011, 1, 1))
    assert (tt.getValue, tt.getTimestamp) == ("testing", unix_dt(2011, 1, 1))
    assert (tg.getValue.toWKT(), tg.getTimestamp) == ("POINT (10 15)", unix_dt(2011, 1, 1))


def test_str():
    tg = TGeomPointInst(GeomPoint(10.0, 15.0), unix_dt(2011, 1, 1))
    assert str(tg) == "POINT (10 15)@2011-01-01T00:00:00+0000"
    assert repr(tg) == "POINT (10 15)@2011-01-01T00:00:00+0000"
