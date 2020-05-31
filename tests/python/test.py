import sys
import pytz
from datetime import datetime
from pathlib import Path
sys.path.append(str(Path(__file__).absolute().parent.parent.parent / "build" / "pybind"))
from pymeos import TInstantBool, TInstantInt, TInstantFloat, TInstantText, TInstantGeom, \
    TInstantSetBool, TInstantSetInt, TSequenceFloat, \
    SerializerInt, SerializerFloat, DeserializerGeom, DeserializerInt, DeserializerFloat, \
    Geometry, Period, PeriodSet


def epoch(year, month, day, hour=0, minute=0):
    return int(datetime(year, month, day, hour, minute).replace(tzinfo=pytz.UTC).timestamp() * 1000)


def test_data_types():
    # Example creation of temporal instant objects
    tb = TInstantBool(True, epoch(2011, 1, 1))
    ti = TInstantInt(10, epoch(2011, 1, 1))
    tf = TInstantFloat(1.25, epoch(2011, 1, 1))
    tt = TInstantText("testing", epoch(2011, 1, 1))
    tg = TInstantGeom(Geometry(10.0, 15.0), epoch(2011, 1, 1))  # Spatiotemporal!

    # Example creation of temporal instant set
    tsetb = TInstantSetBool({tb})

    # Example creation of temporal sequence
    tseqf = TSequenceFloat([tf], False, True)

    # Let's verify what we've done
    assert (tb.getValue(), ti.getT()) == (True, 1293840000000)
    assert (ti.getValue(), ti.getT()) == (10, 1293840000000)
    assert (tf.getValue(), ti.getT()) == (1.25, 1293840000000)
    assert (tt.getValue(), tt.getT()) == ("testing", 1293840000000)
    assert (tg.getValue().toWKT(), tg.getT()) == ("POINT (10 15)", 1293840000000)

    assert {tb} == tsetb.getInstants()

    assert [tf] == tseqf.getInstants()
    assert (tseqf.left_open, tseqf.right_open) == (False, True)


def test_serialization():
    # Let's get some temporal objects ready, which we can use to show serialization examples
    ti1 = TInstantInt(10, epoch(2011, 1, 1))
    ti2 = TInstantInt(20, epoch(2019, 1, 1))
    tseti = TInstantSetInt({ti1, ti2})

    tf1 = TInstantFloat(1.0, epoch(2011, 1, 1))
    tf2 = TInstantFloat(2.5, epoch(2011, 1, 2))
    tseqf = TSequenceFloat([tf1, tf2], False, True)

    # Example serialization of these objects
    si = SerializerInt()
    assert (si.write(ti1) == "10@2011-01-01T00:00:00+0000")

    sf = SerializerFloat()
    assert (sf.write(tseqf) == "[1.000000@2011-01-01T00:00:00+0000, 2.500000@2011-01-02T00:00:00+0000)")

    # For sets, ordering might vary, so we need to check accordingly
    serialized = si.write(tseti)
    assert len(serialized) == 58
    assert serialized[0] == "{"
    assert serialized[-1] == "}"
    assert {"10@2011-01-01T00:00:00+0000", "20@2019-01-01T00:00:00+0000"} == set(serialized[1:-1].split(", "))


def test_deserialization():
    di = DeserializerInt("10@2011-01-01")
    ti = di.nextTInstant()
    assert (ti.getValue(), ti.getT()) == (10, epoch(2011, 1, 1))

    df = DeserializerFloat("{1.0@2011-01-01, 2.5@2011-01-02}")
    tset = df.nextTInstantSet()
    actual = {(tf.getValue(), tf.getT()) for tf in tset.getInstants()}
    expected = {(1.0, epoch(2011, 1, 1)), (2.5, epoch(2011, 1, 2))}
    assert actual == expected

    dg = DeserializerGeom("[POINT(0 0)@2012-01-01 08:00:00+00, POINT(2 0)@2012-01-01 08:10:00+00, POINT(2 -1.98)@2012-01-01 08:15:00+00]")
    tseq = dg.nextTSequence()
    assert (tseq.left_open, tseq.right_open) == (False, False)
    actual = [(tg.getValue().toWKT(), tg.getT()) for tg in tseq.getInstants()]
    expected = [('POINT (0 0)', epoch(2012, 1, 1, 8)), ('POINT (2 0)', epoch(2012, 1, 1, 8, 10)), ('POINT (2 -1.98)', epoch(2012, 1, 1, 8, 15))]
    assert actual == expected


def test_time_types():
    # Example creation of time objects
    period_1 = Period(epoch(2011, 1, 1), epoch(2011, 1, 2), True, False)
    period_2 = Period(epoch(2011, 1, 5), epoch(2011, 1, 6), False, False)
    period_3 = Period(epoch(2011, 1, 1), epoch(2011, 1, 2), True, False)
    assert period_1 == period_3
    assert period_2 != period_1
    assert period_2 != period_3
    period_set = PeriodSet({period_1, period_2, period_3})

    # Let's verify what we've done
    assert period_1.lower() == 1293840000000
    assert period_1.upper() == 1293926400000
    assert period_1.lower_inc() == True
    assert period_1.upper_inc() == False

    assert period_2.lower() == 1294185600000
    assert period_2.upper() == 1294272000000
    assert period_2.lower_inc() == False
    assert period_2.upper_inc() == False

    assert len(period_set.getPeriods()) == 2
    assert {period_1, period_2} == period_set.getPeriods()


if __name__ == "__main__":
    test_data_types()
    test_serialization()
    test_deserialization()
    test_time_types()
    print("All tests passed")
