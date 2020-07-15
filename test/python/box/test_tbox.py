import pytest

from pymeos.box import TBox

from ..utils import unix_dt


@pytest.mark.parametrize("tbox, serialized", [
    (
        TBox(),
        "TBOX()",
    ),
    (
        TBox(10, unix_dt(2011, 1, 1), 20, unix_dt(2011, 1, 2)),
        "TBOX((10, 2011-01-01T00:00:00+0000), (20, 2011-01-02T00:00:00+0000))",
    ),
    (
        TBox(10, 20),
        "TBOX((10,), (20,))",
    ),
    (
        TBox(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2)),
        "TBOX((, 2011-01-01T00:00:00+0000), (, 2011-01-02T00:00:00+0000))",
    ),
    (
        TBox(10, "2011-01-01", 20, "2011-01-02"),
        "TBOX((10, 2011-01-01T00:00:00+0000), (20, 2011-01-02T00:00:00+0000))",
    ),
    (
        TBox("10", "2011-01-01", "20", "2011-01-02"),
        "TBOX((10, 2011-01-01T00:00:00+0000), (20, 2011-01-02T00:00:00+0000))",
    ),
    (
        TBox("TBOX((10, 2011-01-01T00:00), (20, 2011-01-02T00:00))"),
        "TBOX((10, 2011-01-01T00:00:00+0000), (20, 2011-01-02T00:00:00+0000))",
    ),
])
def test_constructor_and_serdes(tbox, serialized):
    assert str(tbox) == serialized
    assert TBox(serialized) == tbox
