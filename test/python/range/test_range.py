from pymeos import GeomPoint
from pymeos.range import RangeInt, RangeBool, RangeFloat, RangeText, RangeGeom


def test_constructor_different_base_types():
    rb = RangeBool(True, False)
    ri = RangeInt(10, 20)
    rf = RangeFloat(1.25, 6.25)
    rt = RangeText("testing", "example")
    rg = RangeGeom(GeomPoint(10.0, 15.0), GeomPoint(20, 24))

    assert (rb.lower, rb.upper) == (True, False)
    assert (ri.lower, ri.upper) == (10, 20)
    assert (rf.lower, rf.upper) == (1.25, 6.25)
    assert (rt.lower, rt.upper) == ("testing", "example")
    assert (rg.lower.toWKT(), rg.upper.toWKT()) == ("POINT (10 15)", "POINT (20 24)")


def test_str():
    tg = RangeGeom(GeomPoint(10.0, 15.0), GeomPoint(20, 24))
    assert str(tg) == "[POINT (10 15), POINT (20 24))"
