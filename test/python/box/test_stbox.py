import datetime
import pytest

from pymeos.box import STBox

from ..utils import unix_dt


@pytest.mark.parametrize("stbox, serialized", [
    (
        STBox(),
        "STBOX()",
    ),
    (
        STBox(11, 12, 13, unix_dt(2011, 1, 1), 21, 22, 23, unix_dt(2011, 1, 2)),
        "STBOX ZT((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, "2011-01-01", 21, 22, 23, "2011-01-02"),
        "STBOX ZT((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, 21, 22, 23),
        "STBOX Z((11, 12, 13), (21, 22, 23))",
    ),
    (
        STBox(11, 12, unix_dt(2011, 1, 1), 21, 22, unix_dt(2011, 1, 2)),
        "STBOX T((11, 12, 2011-01-01T00:00:00+0000), (21, 22, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, "2011-01-01", 21, 22, "2011-01-02"),
        "STBOX T((11, 12, 2011-01-01T00:00:00+0000), (21, 22, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2)),
        "STBOX T(( , , 2011-01-01T00:00:00+0000), ( , , 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox("2011-01-01", "2011-01-02"),
        "STBOX T(( , , 2011-01-01T00:00:00+0000), ( , , 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 21, 22),
        "STBOX((11, 12), (21, 22))",
    ),
    (
        STBox(11, 12, 13, unix_dt(2011, 1, 1), 21, 22, 23, unix_dt(2011, 1, 2), geodetic=True),
        "SRID=4326;GEODSTBOX T((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, "2011-01-01", 21, 22, 23, "2011-01-02", geodetic=True),
        "SRID=4326;GEODSTBOX T((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2), geodetic=True),
        "SRID=4326;GEODSTBOX T(( , , 2011-01-01T00:00:00+0000), ( , , 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox("2011-01-01", "2011-01-02", geodetic=True),
        "SRID=4326;GEODSTBOX T(( , , 2011-01-01T00:00:00+0000), ( , , 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, 21, 22, 23, geodetic=True),
        "SRID=4326;GEODSTBOX((11, 12, 13), (21, 22, 23))",
    ),
    (
        STBox("STBOX ZT((11, 12, 13, 2011-01-01T00:00), (21, 22, 23, 2011-01-02T00:00))"),
        "STBOX ZT((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, "2011-01-01", 21, 22, "2011-01-02", srid=5676),
        "SRID=5676;STBOX T((11, 12, 2011-01-01T00:00:00+0000), (21, 22, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, 21, 22, 23, srid=4326, geodetic=True),
        "SRID=4326;GEODSTBOX((11, 12, 13), (21, 22, 23))",
    ),
])
def test_constructor_and_serdes(stbox, serialized):
    assert str(stbox) == serialized
    assert STBox(serialized) == stbox


def test_accessor_functions():
    stbox = STBox(11, 12, 13, "2011-01-01", 21, 22, 23, "2011-01-02", srid=5676)
    assert stbox.xmin == 11
    assert stbox.ymin == 12
    assert stbox.zmin == 13
    assert stbox.tmin == unix_dt(2011, 1, 1)
    assert stbox.xmax == 21
    assert stbox.ymax == 22
    assert stbox.zmax == 23
    assert stbox.tmax == unix_dt(2011, 1, 2)
    assert stbox.srid == 5676
    assert stbox.geodetic == False

    assert stbox == STBox(11, 12, 13, "2011-01-01 00:00", 21, 22, 23, "2011-01-02 00:00", srid=5676)
