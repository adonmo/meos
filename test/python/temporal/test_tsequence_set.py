import pytest

from pymeos.temporal import TInstantInt, TSequenceInt, TSequenceSetInt, TemporalDuration

from ..utils import unix_dt


def get_sample_tsequence_1():
    instant_1 = TInstantInt(10, unix_dt(2011, 1, 1))
    instant_2 = TInstantInt(40, unix_dt(2011, 1, 2))
    return TSequenceInt({instant_1, instant_2})


def get_sample_tsequence_2():
    instant_1 = TInstantInt(20, unix_dt(2011, 1, 3))
    instant_2 = TInstantInt(30, unix_dt(2011, 1, 4))
    return TSequenceInt({instant_1, instant_2})


def get_sample_tsequence_set():
    return TSequenceSetInt({
        get_sample_tsequence_1(),
        get_sample_tsequence_2(),
    })


@pytest.mark.parametrize("actual", [
    TSequenceSetInt({get_sample_tsequence_1(), get_sample_tsequence_2()}),
    TSequenceSetInt({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}),
    TSequenceSetInt("{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
])
def test_different_constructors(actual):
    assert actual.duration == TemporalDuration.SequenceSet
    assert actual.duration.name == 'SequenceSet'
    assert len(actual.sequences) == 2
    assert actual.startSequence == get_sample_tsequence_1()
    assert actual.endSequence == get_sample_tsequence_2()


def test_constructor():
    seq_1 = get_sample_tsequence_1()
    seq_2 = get_sample_tsequence_2()
    seq_3 = get_sample_tsequence_1() # Repeating
    tseqset = TSequenceSetInt({seq_1, seq_2, seq_3})

    sequences = tseqset.sequences
    assert len(sequences) == 2
    assert sequences == {seq_1, seq_2}


def test_str():
    tseqset = get_sample_tsequence_set()
    assert str(tseqset) == '{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}'
