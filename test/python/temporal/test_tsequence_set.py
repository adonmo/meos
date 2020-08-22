import pytest

from pymeos import GeomPoint
from pymeos.io import DeserializerFloat
from pymeos.temporal import (Interpolation, TemporalDuration, TFloatInst,
                             TGeomPointInst, TIntInst, TFloatSeq,
                             TGeomPointSeq, TIntSeq, TFloatSeqSet,
                             TGeomPointSeqSet, TIntSeqSet)

from ..utils import unix_dt


def get_sample_int_tseq_1():
    instant_1 = TIntInst(10, unix_dt(2011, 1, 1))
    instant_2 = TIntInst(40, unix_dt(2011, 1, 2))
    return TIntSeq({instant_1, instant_2})


def get_sample_int_tseq_2():
    instant_1 = TIntInst(20, unix_dt(2011, 1, 3))
    instant_2 = TIntInst(30, unix_dt(2011, 1, 4))
    return TIntSeq({instant_1, instant_2})


def get_sample_int_tseq_set():
    return TIntSeqSet({
        get_sample_int_tseq_1(),
        get_sample_int_tseq_2(),
    })


def get_sample_float_tseq_1():
    instant_1 = TFloatInst(10, unix_dt(2011, 1, 1))
    instant_2 = TFloatInst(40, unix_dt(2011, 1, 2))
    return TFloatSeq({instant_1, instant_2})


def get_sample_float_tseq_2():
    instant_1 = TFloatInst(20, unix_dt(2011, 1, 3))
    instant_2 = TFloatInst(30, unix_dt(2011, 1, 4))
    return TFloatSeq({instant_1, instant_2})


def get_sample_float_tseq_set():
    return TFloatSeqSet({
        get_sample_float_tseq_1(),
        get_sample_float_tseq_2(),
    })


def get_sample_geom_tseq_1(srid=None):
    instant_1 = TGeomPointInst(GeomPoint(10, 12), unix_dt(2011, 1, 1))
    instant_2 = TGeomPointInst(GeomPoint(40, 42), unix_dt(2011, 1, 2))
    s = {instant_1, instant_2}
    return TGeomPointSeq(s) if srid is None else TGeomPointSeq(s, True, False, srid)


def get_sample_geom_tseq_2(srid=None):
    instant_1 = TGeomPointInst(GeomPoint(20, 22), unix_dt(2011, 1, 3))
    instant_2 = TGeomPointInst(GeomPoint(30, 32), unix_dt(2011, 1, 4))
    s = {instant_1, instant_2}
    return TGeomPointSeq(s) if srid is None else TGeomPointSeq(s, True, False, srid)


def get_sample_geom_tseq_set(srid=None):
    s = {
        get_sample_geom_tseq_1(srid),
        get_sample_geom_tseq_2(srid),
    }
    return TGeomPointSeqSet(s) if srid is None else TGeomPointSeqSet(s, srid)


@pytest.mark.parametrize("actual", [
    TIntSeqSet({get_sample_int_tseq_1(), get_sample_int_tseq_2()}),
    TIntSeqSet({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}),
    TIntSeqSet("{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
    TIntSeqSet({get_sample_int_tseq_1(), get_sample_int_tseq_2()}, Interpolation.Stepwise),
    TIntSeqSet({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}, Interpolation.Stepwise),
    TIntSeqSet("Interp=Stepwise;{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
])
def test_different_constructors_with_discrete_base_type(actual):
    assert actual.duration == TemporalDuration.SequenceSet
    assert actual.duration.name == 'SequenceSet'

    assert actual.interpolation == Interpolation.Stepwise
    assert actual.interpolation.name == 'Stepwise'

    assert len(actual.sequences) == 2
    assert actual.startSequence == get_sample_int_tseq_1()
    assert actual.endSequence == get_sample_int_tseq_2()

    assert str(actual) == "{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
    assert repr(actual) == "{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"


@pytest.mark.parametrize("actual", [
    TFloatSeqSet({get_sample_float_tseq_1(), get_sample_float_tseq_2()}),
    TFloatSeqSet({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}),
    TFloatSeqSet("{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
    TFloatSeqSet({get_sample_float_tseq_1(), get_sample_float_tseq_2()}, Interpolation.Linear),
    TFloatSeqSet({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}, Interpolation.Linear),
    TFloatSeqSet({"Interp=Linear;[10@2011-01-01, 40@2011-01-02)", "Interp=Linear;[20@2011-01-03, 30@2011-01-04)"}),
    TFloatSeqSet("Interp=Linear;{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
])
def test_different_constructors_with_continuous_base_type(actual):
    assert actual.duration == TemporalDuration.SequenceSet
    assert actual.duration.name == 'SequenceSet'

    assert actual.interpolation == Interpolation.Linear
    assert actual.interpolation.name == 'Linear'

    assert len(actual.sequences) == 2
    assert actual.startSequence == get_sample_float_tseq_1()
    assert actual.endSequence == get_sample_float_tseq_2()

    assert str(actual) == "{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
    assert repr(actual) == "{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"


@pytest.mark.parametrize("expected_srid, actual", [
    (0,    TGeomPointSeqSet({get_sample_geom_tseq_1(), get_sample_geom_tseq_2()})),
    (4326, TGeomPointSeqSet({get_sample_geom_tseq_1(), get_sample_geom_tseq_2()}, 4326)),
    (4326, TGeomPointSeqSet({get_sample_geom_tseq_1(4326), get_sample_geom_tseq_2(4326)})),
    (4326, TGeomPointSeqSet({get_sample_geom_tseq_1(4326), get_sample_geom_tseq_2(4326)}, 0)),
    (4326, TGeomPointSeqSet({get_sample_geom_tseq_1(4326), get_sample_geom_tseq_2(4326)}, 4326)),

    (0,    TGeomPointSeqSet({str(get_sample_geom_tseq_1()), str(get_sample_geom_tseq_2())})),
    (4326, TGeomPointSeqSet({str(get_sample_geom_tseq_1()), str(get_sample_geom_tseq_2())}, 4326)),
    (4326, TGeomPointSeqSet({str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))})),
    (4326, TGeomPointSeqSet({str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))}, 0)),
    (4326, TGeomPointSeqSet({str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))}, 4326)),

    (0,    TGeomPointSeqSet(str(get_sample_geom_tseq_set()))),
    (4326, TGeomPointSeqSet(str(get_sample_geom_tseq_set()), 4326)),
    (4326, TGeomPointSeqSet("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))))),
    (4326, TGeomPointSeqSet("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))), 0)),
    (4326, TGeomPointSeqSet("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))), 4326)),
    (4326, TGeomPointSeqSet(str(get_sample_geom_tseq_set(4326)))),
    (4326, TGeomPointSeqSet(str(get_sample_geom_tseq_set(4326)), 0)),
    (4326, TGeomPointSeqSet(str(get_sample_geom_tseq_set(4326)), 4326)),
])
def test_different_geom_constructors(expected_srid, actual):
    assert actual.duration == TemporalDuration.SequenceSet
    assert actual.duration.name == 'SequenceSet'

    assert actual.srid == expected_srid
    assert actual.startSequence.srid == expected_srid
    assert actual.startInstant.srid == expected_srid
    assert actual.startValue.srid == expected_srid

    assert len(actual.sequences) == 2
    assert actual.startSequence == get_sample_geom_tseq_1(expected_srid)
    assert actual.endSequence == get_sample_geom_tseq_2(expected_srid)

    assert len(actual.instants) == 4
    assert actual.startInstant == TGeomPointInst(GeomPoint(10, 12, expected_srid), unix_dt(2011, 1, 1))
    assert actual.endInstant == TGeomPointInst(GeomPoint(30, 32, expected_srid), unix_dt(2011, 1, 4))


@pytest.mark.parametrize("args", [
    ({get_sample_geom_tseq_1(5676), get_sample_geom_tseq_2(5676)}, 4326),
    ({str(get_sample_geom_tseq_1(5676)), str(get_sample_geom_tseq_2(5676))}, 4326),
    ("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(5676)), str(get_sample_geom_tseq_2(5676))), 4326),
    (str(get_sample_geom_tseq_set(5676)), 4326),
])
def test_constructors_with_conflicting_srids(args):
    with pytest.raises(ValueError, match="Conflicting SRIDs provided. Given: 4326, while (Sequence|Geometry) contains: 5676"):
        TGeomPointSeqSet(*args)


@pytest.mark.parametrize('seqset', [
    TFloatSeqSet('Interp=Stepwise;{[10.0@2019-09-01 00:00:00+01], [20.0@2019-09-02 00:00:00+01, 10.0@2019-09-03 00:00:00+01]}'),
    DeserializerFloat('Interp=Stepwise;{[10.0@2019-09-01 00:00:00+01], [20.0@2019-09-02 00:00:00+01, 10.0@2019-09-03 00:00:00+01]}').nextTemporal(),
    DeserializerFloat('Interp=Stepwise;{[10.0@2019-09-01 00:00:00+01], [20.0@2019-09-02 00:00:00+01, 10.0@2019-09-03 00:00:00+01]}').nextTSequenceSet(),
])
def test_interpolation_is_maintained_in_sequences(seqset):
    assert seqset.sequences == {
        TFloatSeq('Interp=Stepwise;[10.0@2019-09-01 00:00:00+01]'),
        TFloatSeq('Interp=Stepwise;[20.0@2019-09-02 00:00:00+01, 10.0@2019-09-03 00:00:00+01]')
    }
    assert seqset.interpolation == Interpolation.Stepwise
    for seq in seqset.sequences:
        assert seq.interpolation == seqset.interpolation


def test_constructor():
    seq_1 = get_sample_int_tseq_1()
    seq_2 = get_sample_int_tseq_2()
    seq_3 = get_sample_int_tseq_1() # Repeating
    tseqset = TIntSeqSet({seq_1, seq_2, seq_3})

    sequences = tseqset.sequences
    assert len(sequences) == 2
    assert sequences == {seq_1, seq_2}


def test_str():
    tseqset = get_sample_int_tseq_set()
    assert str(tseqset) == '{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}'

    tseqset = TFloatSeqSet("Interp=Stepwise;{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}")
    assert str(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
    assert repr(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"

    tseqset = TFloatSeqSet({"Interp=Stepwise;[10@2011-01-01, 40@2011-01-02)", "Interp=Stepwise;[20@2011-01-03, 30@2011-01-04)"}, Interpolation.Stepwise)
    assert str(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
    assert repr(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
