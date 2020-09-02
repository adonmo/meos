import pytest
from pymeos import GeomPoint
from pymeos.range import RangeInt, RangeBool, RangeFloat, RangeText, RangeGeom


@pytest.mark.parametrize("actual", [
    RangeInt(10, 20),
    RangeInt("[10, 20)"),
])
def test_different_constructors(actual):
    assert actual.lower == 10
    assert actual.upper == 20
    assert actual.lower_inc == True
    assert actual.upper_inc == False


def test_constructor_different_base_types():
    rb = RangeBool(False, True)
    ri = RangeInt(10, 20)
    rf = RangeFloat(1.25, 6.25)
    rt = RangeText("example", "testing")
    rg = RangeGeom(GeomPoint(10.0, 15.0), GeomPoint(20, 24))

    assert (rb.lower, rb.upper) == (False, True)
    assert (ri.lower, ri.upper) == (10, 20)
    assert (rf.lower, rf.upper) == (1.25, 6.25)
    assert (rt.lower, rt.upper) == ("example", "testing")
    assert (rg.lower.toWKT(), rg.upper.toWKT()) == ("POINT (10 15)", "POINT (20 24)")


def test_str():
    tg = RangeGeom(GeomPoint(10.0, 15.0), GeomPoint(20, 24))
    assert str(tg) == "[POINT (10 15), POINT (20 24))"
