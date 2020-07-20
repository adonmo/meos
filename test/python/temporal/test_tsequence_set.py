import pytest

from pymeos import Geometry
from pymeos.temporal import (Interpolation, TemporalDuration, TInstantFloat,
                             TInstantGeom, TInstantInt, TSequenceFloat,
                             TSequenceGeom, TSequenceInt, TSequenceSetFloat,
                             TSequenceSetGeom, TSequenceSetInt)

from ..utils import unix_dt


def get_sample_int_tseq_1():
    instant_1 = TInstantInt(10, unix_dt(2011, 1, 1))
    instant_2 = TInstantInt(40, unix_dt(2011, 1, 2))
    return TSequenceInt({instant_1, instant_2})


def get_sample_int_tseq_2():
    instant_1 = TInstantInt(20, unix_dt(2011, 1, 3))
    instant_2 = TInstantInt(30, unix_dt(2011, 1, 4))
    return TSequenceInt({instant_1, instant_2})


def get_sample_int_tseq_set():
    return TSequenceSetInt({
        get_sample_int_tseq_1(),
        get_sample_int_tseq_2(),
    })


def get_sample_float_tseq_1():
    instant_1 = TInstantFloat(10, unix_dt(2011, 1, 1))
    instant_2 = TInstantFloat(40, unix_dt(2011, 1, 2))
    return TSequenceFloat({instant_1, instant_2})


def get_sample_float_tseq_2():
    instant_1 = TInstantFloat(20, unix_dt(2011, 1, 3))
    instant_2 = TInstantFloat(30, unix_dt(2011, 1, 4))
    return TSequenceFloat({instant_1, instant_2})


def get_sample_float_tseq_set():
    return TSequenceSetFloat({
        get_sample_float_tseq_1(),
        get_sample_float_tseq_2(),
    })


def get_sample_geom_tseq_1(srid=None):
    instant_1 = TInstantGeom(Geometry(10, 12), unix_dt(2011, 1, 1))
    instant_2 = TInstantGeom(Geometry(40, 42), unix_dt(2011, 1, 2))
    s = {instant_1, instant_2}
    return TSequenceGeom(s) if srid is None else TSequenceGeom(s, True, False, srid)


def get_sample_geom_tseq_2(srid=None):
    instant_1 = TInstantGeom(Geometry(20, 22), unix_dt(2011, 1, 3))
    instant_2 = TInstantGeom(Geometry(30, 32), unix_dt(2011, 1, 4))
    s = {instant_1, instant_2}
    return TSequenceGeom(s) if srid is None else TSequenceGeom(s, True, False, srid)


def get_sample_geom_tseq_set(srid=None):
    s = {
        get_sample_geom_tseq_1(srid),
        get_sample_geom_tseq_2(srid),
    }
    return TSequenceSetGeom(s) if srid is None else TSequenceSetGeom(s, srid)


@pytest.mark.parametrize("actual", [
    TSequenceSetInt({get_sample_int_tseq_1(), get_sample_int_tseq_2()}),
    TSequenceSetInt({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}),
    TSequenceSetInt("{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
    TSequenceSetInt({get_sample_int_tseq_1(), get_sample_int_tseq_2()}, Interpolation.Stepwise),
    TSequenceSetInt({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}, Interpolation.Stepwise),
    TSequenceSetInt("Interp=Stepwise;{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
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
    TSequenceSetFloat({get_sample_float_tseq_1(), get_sample_float_tseq_2()}),
    TSequenceSetFloat({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}),
    TSequenceSetFloat("{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
    TSequenceSetFloat({get_sample_float_tseq_1(), get_sample_float_tseq_2()}, Interpolation.Linear),
    TSequenceSetFloat({"[10@2011-01-01, 40@2011-01-02)", "[20@2011-01-03, 30@2011-01-04)"}, Interpolation.Linear),
    TSequenceSetFloat({"Interp=Linear;[10@2011-01-01, 40@2011-01-02)", "Interp=Linear;[20@2011-01-03, 30@2011-01-04)"}),
    TSequenceSetFloat("Interp=Linear;{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}"),
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
    (0,    TSequenceSetGeom({get_sample_geom_tseq_1(), get_sample_geom_tseq_2()})),
    (4326, TSequenceSetGeom({get_sample_geom_tseq_1(), get_sample_geom_tseq_2()}, 4326)),
    (4326, TSequenceSetGeom({get_sample_geom_tseq_1(4326), get_sample_geom_tseq_2(4326)})),
    (4326, TSequenceSetGeom({get_sample_geom_tseq_1(4326), get_sample_geom_tseq_2(4326)}, 0)),
    (4326, TSequenceSetGeom({get_sample_geom_tseq_1(4326), get_sample_geom_tseq_2(4326)}, 4326)),

    (0,    TSequenceSetGeom({str(get_sample_geom_tseq_1()), str(get_sample_geom_tseq_2())})),
    (4326, TSequenceSetGeom({str(get_sample_geom_tseq_1()), str(get_sample_geom_tseq_2())}, 4326)),
    (4326, TSequenceSetGeom({str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))})),
    (4326, TSequenceSetGeom({str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))}, 0)),
    (4326, TSequenceSetGeom({str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))}, 4326)),

    (0,    TSequenceSetGeom(str(get_sample_geom_tseq_set()))),
    (4326, TSequenceSetGeom(str(get_sample_geom_tseq_set()), 4326)),
    (4326, TSequenceSetGeom("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))))),
    (4326, TSequenceSetGeom("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))), 0)),
    (4326, TSequenceSetGeom("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(4326)), str(get_sample_geom_tseq_2(4326))), 4326)),
    (4326, TSequenceSetGeom(str(get_sample_geom_tseq_set(4326)))),
    (4326, TSequenceSetGeom(str(get_sample_geom_tseq_set(4326)), 0)),
    (4326, TSequenceSetGeom(str(get_sample_geom_tseq_set(4326)), 4326)),
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
    assert actual.startInstant == TInstantGeom(Geometry(10, 12, expected_srid), unix_dt(2011, 1, 1))
    assert actual.endInstant == TInstantGeom(Geometry(30, 32, expected_srid), unix_dt(2011, 1, 4))


@pytest.mark.parametrize("args", [
    ({get_sample_geom_tseq_1(5676), get_sample_geom_tseq_2(5676)}, 4326),
    ({str(get_sample_geom_tseq_1(5676)), str(get_sample_geom_tseq_2(5676))}, 4326),
    ("{{ {}, {} }}".format(str(get_sample_geom_tseq_1(5676)), str(get_sample_geom_tseq_2(5676))), 4326),
    (str(get_sample_geom_tseq_set(5676)), 4326),
])
def test_constructors_with_conflicting_srids(args):
    with pytest.raises(ValueError, match="Conflicting SRIDs provided. Given: 4326, while (Sequence|Geometry) contains: 5676"):
        TSequenceSetGeom(*args)


def test_constructor():
    seq_1 = get_sample_int_tseq_1()
    seq_2 = get_sample_int_tseq_2()
    seq_3 = get_sample_int_tseq_1() # Repeating
    tseqset = TSequenceSetInt({seq_1, seq_2, seq_3})

    sequences = tseqset.sequences
    assert len(sequences) == 2
    assert sequences == {seq_1, seq_2}


def test_str():
    tseqset = get_sample_int_tseq_set()
    assert str(tseqset) == '{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}'

    tseqset = TSequenceSetFloat("Interp=Stepwise;{[10@2011-01-01, 40@2011-01-02), [20@2011-01-03, 30@2011-01-04)}")
    assert str(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
    assert repr(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"

    tseqset = TSequenceSetFloat({"Interp=Stepwise;[10@2011-01-01, 40@2011-01-02)", "Interp=Stepwise;[20@2011-01-03, 30@2011-01-04)"}, Interpolation.Stepwise)
    assert str(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
    assert repr(tseqset) == "Interp=Stepwise;{[10@2011-01-01T00:00:00+0000, 40@2011-01-02T00:00:00+0000), [20@2011-01-03T00:00:00+0000, 30@2011-01-04T00:00:00+0000)}"
