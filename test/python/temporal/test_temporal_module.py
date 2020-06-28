import datetime

import pytz

from pymeos import Geometry
from pymeos.temporal import (TInstantBool, TInstantFloat, TInstantGeom,
                             TInstantInt, TInstantSetBool,
                             TInstantText, TSequenceFloat)


def unix_dt(year, month, day, hour=0, minute=0, second=0):
    return pytz.UTC.localize(datetime.datetime(year, month, day, hour, minute, second))


def test_temporal_types():
    # Example creation of temporal instant objects
    tb = TInstantBool(True, unix_dt(2011, 1, 1))
    ti = TInstantInt(10, unix_dt(2011, 1, 1))
    tf = TInstantFloat(1.25, unix_dt(2011, 1, 1))
    tt = TInstantText("testing", unix_dt(2011, 1, 1))
    tg = TInstantGeom(Geometry(10.0, 15.0), unix_dt(2011, 1, 1))  # Spatiotemporal!

    # Example creation of temporal instant set
    tsetb = TInstantSetBool({tb})

    # Example creation of temporal sequence
    tseqf = TSequenceFloat([tf], False, True)

    # Let's verify what we've done
    assert (tb.getValue(), ti.getTimestamp()) == (True, unix_dt(2011, 1, 1))
    assert (ti.getValue(), ti.getTimestamp()) == (10, unix_dt(2011, 1, 1))
    assert (tf.getValue(), ti.getTimestamp()) == (1.25, unix_dt(2011, 1, 1))
    assert (tt.getValue(), tt.getTimestamp()) == ("testing", unix_dt(2011, 1, 1))
    assert (tg.getValue().toWKT(), tg.getTimestamp()) == ("POINT (10 15)", unix_dt(2011, 1, 1))

    assert {tb} == tsetb.getInstants()

    assert [tf] == tseqf.getInstants()
    assert (tseqf.lower_inc, tseqf.upper_inc) == (False, True)
