"""
When testing, run this with python3.6 - this is because right now pybind11 seems to compile for python3.6: https://github.com/pybind/pybind11/issues/1090
"""

import sys
import pytz
from datetime import datetime
from pathlib import Path
sys.path.append(str(Path(__file__).absolute().parent.parent.parent / "build" / "pybind"))
from pymeos import TInstantBool, TInstantInt, TInstantFloat, TInstantText, TInstantSetBool, TInstantSetInt, TSequenceFloat, SerializerInt, SerializerFloat, DeserializerInt, DeserializerFloat


def epoch(year, month, day):
    return int(datetime(year, month, day).replace(tzinfo=pytz.UTC).timestamp() * 1000)


def test_data_types():
    # Example creation of the temporal instant objects (bool, int, float, text) are supported
    tb = TInstantBool(True, epoch(2011, 1, 1))
    ti = TInstantInt(10, epoch(2011, 1, 1))
    tf = TInstantFloat(1.25, epoch(2011, 1, 1))
    tt = TInstantText("testing", epoch(2011, 1, 1))

    # Example creation of temporal instant set
    tsetb = TInstantSetBool({tb})

    # Example creation of temporal sequences
    tseqf = TSequenceFloat([tf], False, True)

    # Let's verify what we've done
    assert (ti.getValue(), ti.getT()) == (10, 1293840000000)
    assert (tt.getValue(), tt.getT()) == ("testing", 1293840000000)

    tb = tsetb.getInstants().pop()
    assert (tb.getValue(), tb.getT()) == (True, 1293840000000)

    tf = tseqf.getInstants()[0]
    assert (tseqf.left_open, tseqf.right_open) == (False, True)
    assert (tf.getValue(), tf.getT()) == (1.25, 1293840000000)


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

    di = DeserializerInt("[10@2011-01-01, 20@2011-01-02)")
    tseq = di.nextTSequence()
    assert (tseq.left_open, tseq.right_open) == (False, True)
    actual = [(ti.getValue(), ti.getT()) for ti in tseq.getInstants()]
    expected = [(10, epoch(2011, 1, 1)), (20, epoch(2011, 1, 2))]
    assert actual == expected

if __name__ == "__main__":
    test_data_types()
    test_serialization()
    test_deserialization()
    print("All tests passed")
