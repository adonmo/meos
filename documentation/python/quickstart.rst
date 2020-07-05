Quickstart
==========

For all below examples, the following imports and helpers are assumed, in the interest of brevity.

.. code-block:: python

    import datetime
    from pymeos import Geometry
    from pymeos.io import (DeserializerFloat, DeserializerGeom, DeserializerInt,
                        SerializerFloat, SerializerInt)
    from pymeos.temporal import (TInstantBool, TInstantFloat, TInstantGeom,
                                TInstantInt, TInstantSetBool, TInstantSetInt,
                                TInstantText, TSequenceFloat)
    from pymeos.time import Period, PeriodSet


    def unix_dt(year, month, day, hour=0, minute=0, second=0):
        return datetime.datetime(year, month, day, hour, minute, second, tzinfo=datetime.timezone.utc)


Range Types
-----------

TODO


Box Types
-----------

TODO


Time Types
----------

.. code-block:: python
    :caption: Example usage of time types (Period, PeriodSet and TimestampSet)

    # Example creation of time objects
    period_1 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2))
    period_2 = Period(unix_dt(2011, 1, 5), unix_dt(2011, 1, 6), False, False)
    period_3 = Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 2), True, False)
    assert period_1 == period_3
    assert period_2 != period_1
    assert period_2 != period_3
    period_set = PeriodSet({period_1, period_2, period_3})

    # Let's verify what we've done
    assert period_1.lower == unix_dt(2011, 1, 1)
    assert period_1.upper == unix_dt(2011, 1, 2)
    assert period_1.lower_inc == True
    assert period_1.upper_inc == False

    assert period_2.lower == unix_dt(2011, 1, 5)
    assert period_2.upper == unix_dt(2011, 1, 6)
    assert period_2.lower_inc == False
    assert period_2.upper_inc == False

    assert len(period_set.periods) == 2
    assert {period_1, period_2} == period_set.periods

    # Test functions
    assert period_1.timespan == datetime.timedelta(days=1)
    assert period_1.contains_timestamp(unix_dt(2011, 1, 1, 10))
    # assert period_set.period == Period(unix_dt(2011, 1, 1), unix_dt(2011, 1, 6), True, False)

    # Serialization
    assert str(period_1) == "[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)"
    assert SerializerInt().write(period_1) == "[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)"

    # Deserialization
    di = DeserializerInt("[2011-01-01T00:00:00+0000, 2011-01-02T00:00:00+0000)")
    period = di.nextPeriod()
    assert period == period_1

    di = DeserializerInt("[2019-09-02 01:00:00+01, 2019-09-03 05:43:21+01)")
    period = di.nextPeriod()
    assert period.lower == unix_dt(2019, 9, 2)
    assert period.upper == unix_dt(2019, 9, 3, 4, 43, 21)
    assert period.lower_inc == True
    assert period.upper_inc == False


Temporal Types
--------------

.. code-block:: python
    :caption: Example usage of time types (TInstant, TInstantSet, TSequence and TSequenceSet)

    # Example creation of temporal instant objects
    tb = TInstantBool(True, unix_dt(2011, 1, 1))
    ti = TInstantInt(10, unix_dt(2011, 1, 1))
    tf = TInstantFloat(1.25, unix_dt(2011, 1, 1))
    tt = TInstantText("testing", unix_dt(2011, 1, 1))
    tg = TInstantGeom(Geometry(10.0, 15.0), unix_dt(2011, 1, 1))  # Spatiotemporal!

    # Example creation of temporal instant set
    tsetb = TInstantSetBool({tb})

    # Example creation of temporal sequence
    tseqf = TSequenceFloat({tf}, False, True)


    # Let's verify what we've done
    assert (tb.getValue, ti.getTimestamp) == (True, unix_dt(2011, 1, 1))
    assert (ti.getValue, ti.getTimestamp) == (10, unix_dt(2011, 1, 1))
    assert (tf.getValue, ti.getTimestamp) == (1.25, unix_dt(2011, 1, 1))
    assert (tt.getValue, tt.getTimestamp) == ("testing", unix_dt(2011, 1, 1))
    assert (tg.getValue.toWKT(), tg.getTimestamp) == ("POINT (10 15)", unix_dt(2011, 1, 1))

    assert {tb} == tsetb.instants

    assert {tf} == tseqf.instants
    assert (tseqf.lower_inc, tseqf.upper_inc) == (False, True)


Serialization
-------------

.. code-block:: python

    # Let's get some temporal objects ready, which we can use to show serialization examples
    ti1 = TInstantInt(10, unix_dt(2011, 1, 1))
    ti2 = TInstantInt(20, unix_dt(2019, 1, 1))
    tseti = TInstantSetInt({ti1, ti2})

    tf1 = TInstantFloat(1.0, unix_dt(2011, 1, 1))
    tf2 = TInstantFloat(2.5, unix_dt(2011, 1, 2))
    tseqf = TSequenceFloat({tf1, tf2}, True, False)

    # Example serialization of these objects
    si = SerializerInt()
    assert (si.write(ti1) == "10@2011-01-01T00:00:00+0000")

    sf = SerializerFloat()
    assert (sf.write(tseqf) == "[1@2011-01-01T00:00:00+0000, 2.5@2011-01-02T00:00:00+0000)")

    # For sets, ordering might vary, so we need to check accordingly
    serialized = si.write(tseti)
    assert len(serialized) == 58
    assert serialized[0] == "{"
    assert serialized[-1] == "}"
    assert {"10@2011-01-01T00:00:00+0000", "20@2019-01-01T00:00:00+0000"} == set(serialized[1:-1].split(", "))



Deserialization
---------------

.. code-block:: python

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
    actual = [(tg.getValue.toWKT(), tg.getTimestamp) for tg in tseq.instants]
    expected = [('POINT (0 0)', unix_dt(2012, 1, 1, 8)), ('POINT (2 0)', unix_dt(2012, 1, 1, 8, 10)), ('POINT (2 -1.98)', unix_dt(2012, 1, 1, 8, 15))]
    assert actual == expected
