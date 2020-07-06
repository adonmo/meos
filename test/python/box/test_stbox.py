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
        "GEODSTBOX T((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, "2011-01-01", 21, 22, 23, "2011-01-02", geodetic=True),
        "GEODSTBOX T((11, 12, 13, 2011-01-01T00:00:00+0000), (21, 22, 23, 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2), geodetic=True),
        "GEODSTBOX T(( , , 2011-01-01T00:00:00+0000), ( , , 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox("2011-01-01", "2011-01-02", geodetic=True),
        "GEODSTBOX T(( , , 2011-01-01T00:00:00+0000), ( , , 2011-01-02T00:00:00+0000))",
    ),
    (
        STBox(11, 12, 13, 21, 22, 23, geodetic=True),
        "GEODSTBOX((11, 12, 13), (21, 22, 23))",
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
