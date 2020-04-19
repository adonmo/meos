"""
When testing, run this with python3.6 - this is because right now pybind11 seems to compile for python3.6: https://github.com/pybind/pybind11/issues/1090
"""

import sys
from pathlib import Path
sys.path.append(str(Path(__file__).absolute().parent.parent / "build" / "pybind"))

from datetime import datetime
from libmeos import Parser, TInstant, TInstantSet, TSequence


# Temporal data types
t = TInstant(10, int(datetime(2011, 1, 1).timestamp() * 1000))
print(t.getValue(), t.getT())

tset = TInstantSet({t})
t = tset.getInstants().pop()
print(t.getValue(), t.getT())

tseq = TSequence([t], False, True)
print(tseq.left_open, tseq.right_open)
t = tseq.getInstants()[0]
print(t.getValue(), t.getT())


# Deserialization / parsing of temporal data types
p = Parser("10@2011-01-01")
t = p.parseNextTInstant()
print(t.getValue(), t.getT())

p = Parser("{10@2011-01-01, 20@2011-01-02}")
tset = p.parseNextTInstantSet()
for t in tset.getInstants():
    print(t.getValue(), t.getT())

p = Parser("[10@2011-01-01, 20@2011-01-02)")
tseq = p.parseNextTSequence()
print(tseq.left_open, tseq.right_open)
for t in tseq.getInstants():
    print(t.getValue(), t.getT())
