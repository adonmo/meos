"""
When testing, run this with python3.6 - this is because right now pybind11 seems to compile for python3.6: https://github.com/pybind/pybind11/issues/1090
"""

import sys
import pytz
from datetime import datetime
from pathlib import Path
sys.path.append(str(Path(__file__).absolute().parent.parent.parent / "build" / "pybind"))


def epoch(year, month, day):
    return int(datetime(year, month, day).replace(tzinfo=pytz.UTC).timestamp() * 1000)


def test_data_types():
    from pymeos import TInstantInt, TInstantFloat, TInstantSetInt, TSequenceFloat


    t = TInstantInt(10, epoch(2011, 1, 1))
    assert (t.getValue(), t.getT()) == (10, 1293840000000)

    tset = TInstantSetInt({t})
    t = tset.getInstants().pop()
    assert (t.getValue(), t.getT()) == (10, 1293840000000)

    tf = TInstantFloat(1.0, epoch(2011, 1, 1))
    tseq = TSequenceFloat([tf], False, True)
    assert (tseq.left_open, tseq.right_open) == (False, True)
    t = tseq.getInstants()[0]
    assert (t.getValue(), t.getT()) == (1.0, 1293840000000)


def test_deserialization():
    from datetime import datetime
    from pymeos import DeserializerInt, DeserializerFloat


    p = DeserializerInt("10@2011-01-01")
    t = p.nextTInstant()
    assert (t.getValue(), t.getT()) == (10, epoch(2011, 1, 1))

    p = DeserializerFloat("{1.0@2011-01-01, 2.5@2011-01-02}")
    tset = p.nextTInstantSet()
    actual = {(t.getValue(), t.getT()) for t in tset.getInstants()}
    expected = {(1.0, epoch(2011, 1, 1)), (2.5, epoch(2011, 1, 2))}
    assert actual == expected

    p = DeserializerInt("[10@2011-01-01, 20@2011-01-02)")
    tseq = p.nextTSequence()
    assert (tseq.left_open, tseq.right_open) == (False, True)
    actual = [(t.getValue(), t.getT()) for t in tseq.getInstants()]
    expected = [(10, epoch(2011, 1, 1)), (20, epoch(2011, 1, 2))]
    assert actual == expected

if __name__ == "__main__":
    test_data_types()
    test_deserialization()
    print("All tests passed")
