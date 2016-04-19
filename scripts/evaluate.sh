#!/bin/bash

# The MIT License (MIT)
#
# Copyright (c) 2016 Derek Felson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#rm -r results

morfessor="../build/morfessor"
morfessorRef="./morfessor-reference.perl"
evalscript="./morpho-challenge-eval.perl"

wordlistEng="../testdata/morpho-challenge-2005-wordlist-english.txt"
testlistEng="../testdata/morpho-challenge-2005-testset-english.txt"
goldstdEng="../testdata/morpho-challenge-2005-goldstd-english.txt"

outdir="results/mc2005/baseline/english"
mkdir -p "$outdir"
time "$morfessor" --data "$wordlistEng"
#"$morfessor" --data "$wordlistEng" > "$outdir/model.txt"
#"$morfessor" --load "$outdir/model.txt" --data "$testlistEng" > "$outdir/test-segmentation.txt"
#"$evalscript" -trace -desired "$goldstdEng" -suggested "$outdir/test-segmentation.txt" > "$outdir/results.txt"

outdir="results/mc2005/baseline/reference/english"
mkdir -p "$outdir"
time "$morfessorRef" -data "$wordlistEng"
#"$morfessorRef" -data "$wordlistEng" > "$outdir/model.txt"
#"$morfessorRef" -load "$outdir/model.txt" -data "$testlistEng" | sed -e "s/^1 //" -e "s/ + / /g" -e "/^#.*/d" > "$outdir/test-segmentation.txt"
#"$evalscript" -trace -desired "$goldstdEng" -suggested "$outdir/test-segmentation.txt" > "$outdir/results.txt"

