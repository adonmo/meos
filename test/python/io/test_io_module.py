from pymeos.io import (DeserializerFloat, DeserializerGeom, DeserializerInt,
                       SerializerFloat, SerializerInt)
from pymeos.temporal import (TFloatInst, TIntInst, TIntInstSet, TFloatSeq, TFloatSeqSet)

from ..utils import unix_dt


def test_serialization():
    # Let's get some temporal objects ready, which we can use to show serialization examples
    ti1 = TIntInst(10, unix_dt(2011, 1, 1))
    ti2 = TIntInst(20, unix_dt(2019, 1, 1))
    tseti = TIntInstSet({ti1, ti2})

    tf1 = TFloatInst(1.0, unix_dt(2011, 1, 1))
    tf2 = TFloatInst(2.5, unix_dt(2011, 1, 2))
    tseqf = TFloatSeq({tf1, tf2})
    tseqsetf = TFloatSeqSet({tseqf})

    # Example serialization of these objects
    si = SerializerInt()
    assert (si.write(ti1) == "10@2011-01-01T00:00:00+0000")

    sf = SerializerFloat()
    assert (sf.write(tseqf) == "[1@2011-01-01T00:00:00+0000, 2.5@2011-01-02T00:00:00+0000)")
    assert (sf.write(tseqsetf) == "{[1@2011-01-01T00:00:00+0000, 2.5@2011-01-02T00:00:00+0000)}")

    # For sets, ordering might vary, so we need to check accordingly
    serialized = si.write(tseti)
    assert len(serialized) == 58
    assert serialized[0] == "{"
    assert serialized[-1] == "}"
    assert {"10@2011-01-01T00:00:00+0000", "20@2019-01-01T00:00:00+0000"} == set(serialized[1:-1].split(", "))
    assert {tseqf} == tseqsetf.sequences


def test_deserialization():
    di = DeserializerInt("10@2011-01-01")
    ti = di.nextTInstant()
    assert (ti.getValue, ti.getTimestamp) == (10, unix_dt(2011, 1, 1))

    df = DeserializerFloat("{1.0@2011-01-01, 2.5@2011-01-02}")
    tset = df.nextTInstantSet()
    actual = {(tf.getValue, tf.getTimestamp) for tf in tset.instants}
    expected = {(1.0, unix_dt(2011, 1, 1)), (2.5, unix_dt(2011, 1, 2))}
    assert actual == expected

    dg = DeserializerGeom("[POINT(0 0)@2012-01-01 08:00:00+00, POINT(2 0)@2012-01-01 08:10:00+00, POINT(2 -1.98)@2012-01-01 08:15:00+00]")
    tseq = dg.nextTSequence()
    assert (tseq.lower_inc, tseq.upper_inc) == (True, True)
    actual = {(tg.getValue.toWKT(), tg.getTimestamp) for tg in tseq.instants}
    expected = {('POINT (0 0)', unix_dt(2012, 1, 1, 8)), ('POINT (2 0)', unix_dt(2012, 1, 1, 8, 10)), ('POINT (2 -1.98)', unix_dt(2012, 1, 1, 8, 15))}
    assert actual == expected

    df = DeserializerFloat("{[1.0@2011-01-01, 2.5@2011-01-02)}")
    tseqset = df.nextTSequenceSet()
    tf1 = TFloatInst(1.0, unix_dt(2011, 1, 1))
    tf2 = TFloatInst(2.5, unix_dt(2011, 1, 2))
    expected = TFloatSeqSet({TFloatSeq({tf1, tf2})})
    assert tseqset == expected
