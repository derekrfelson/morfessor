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

wordlist="../testdata/morpho-challenge-2005-wordlist"
testlist="../testdata/morpho-challenge-2005-testset"
goldstd="../testdata/morpho-challenge-2005-goldstd"

evaluate() {
    outdir="$1/$3"
    args="$2"
    language="$3"

    mkdir -p "$outdir"
    "$morfessor" $args --data "$wordlist-${language}.txt" > "$outdir/model.txt"
    "$morfessor" $args --load "$outdir/model.txt" --data "$testlist-${language}.txt" > "$outdir/test-segmentation.txt"
    "$evalscript" -trace -desired "$goldstd-${language}.txt" -suggested "$outdir/test-segmentation.txt" > "$outdir/results.txt"
    return 0
}

evaluate_ref() {
    outdir="$1/$3"
    args="$2"
    language="$3"

    mkdir -p "$outdir"
    "$morfessorRef" -data "$wordlist-${language}.txt" > "$outdir/model.txt"
    "$morfessorRef" -load "$outdir/model.txt" -data "$testlist-${language}.txt" | sed -e "s/^1 //" -e "s/ + / /g" -e "/^#.*/d" > "$outdir/test-segmentation.txt"
    "$evalscript" -trace -desired "$goldstd-${language}.txt" -suggested "$outdir/test-segmentation.txt" > "$outdir/results.txt"
    return 0
}

set -o xtrace

evaluate "results/mc2005/baseline" "--mode Baseline" "english"

# Can run many different tests to see what effect different algorithm
# versions or parameters may have. Uncomment as needed, or write your
# own custom invocations.

#evaluate "results/mc2005/baseline" "--mode Baseline" "english"
#evaluate "results/mc2005/baseline-length" "--mode Length --beta 1.0 --most_common_length 7.0" "english"
#evaluate "results/mc2005/baseline-frequency" "--mode Freq --hapax 0.5" "english"
#evaluate "results/mc2005/baseline-frequency-length" "--mode FreqLength --beta 1.0 --most_common_length 7.0 --hapax 0.5" "english"
#evaluate "results/mc2005/baseline-length-1.0-6.0" "--mode Length --beta 1.0 --most_common_length 6.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-7.0" "--mode Length --beta 1.0 --most_common_length 7.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-8.0" "--mode Length --beta 1.0 --most_common_length 8.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-9.0" "--mode Length --beta 1.0 --most_common_length 9.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-10.0" "--mode Length --beta 1.0 --most_common_length 10.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-11.0" "--mode Length --beta 1.0 --most_common_length 11.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-12.0" "--mode Length --beta 1.0 --most_common_length 12.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-13.0" "--mode Length --beta 1.0 --most_common_length 13.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-14.0" "--mode Length --beta 1.0 --most_common_length 14.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-15.0" "--mode Length --beta 1.0 --most_common_length 15.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-16.0" "--mode Length --beta 1.0 --most_common_length 16.0" "english"
#evaluate "results/mc2005/baseline-length-1.0-17.0" "--mode Length --beta 1.0 --most_common_length 17.0" "english"
#evaluate "results/mc2005/baseline-frequency-0.1" "--mode Freq --hapax 0.1" "english"
#evaluate "results/mc2005/baseline-frequency-0.2" "--mode Freq --hapax 0.2" "english"
#evaluate "results/mc2005/baseline-frequency-0.3" "--mode Freq --hapax 0.3" "english"
#evaluate "results/mc2005/baseline-frequency-0.4" "--mode Freq --hapax 0.4" "english"
#evaluate "results/mc2005/baseline-frequency-0.5" "--mode Freq --hapax 0.5" "english"
#evaluate "results/mc2005/baseline-frequency-0.6" "--mode Freq --hapax 0.6" "english"
#evaluate "results/mc2005/baseline-frequency-0.7" "--mode Freq --hapax 0.7" "english"
#evaluate "results/mc2005/baseline-frequency-0.8" "--mode Freq --hapax 0.8" "english"
#evaluate "results/mc2005/baseline-frequency-0.9" "--mode Freq --hapax 0.9" "english"
#evaluate "results/mc2005/baseline-frequency-length-1.0-6.0-0.4" "--mode FreqLength --beta 1.0 --most_common_length 6.0 --hapax 0.4" "english"
#evaluate "results/mc2005/baseline" "--mode Baseline" "finnish"
#evaluate "results/mc2005/baseline-length" "--mode Length --beta 1.0 --most_common_length 7.0" "finnish"
#evaluate "results/mc2005/baseline-frequency" "--mode Freq --hapax 0.5" "finnish"
#evaluate "results/mc2005/baseline-frequency-length" "--mode FreqLength --beta 1.0 --most_common_length 7.0 --hapax 0.5" "finnish"
#evaluate_ref "results/mc2005/baseline/reference" "-trace 0" "english"
#evaluate_ref "results/mc2005/baseline/reference" "-trace 31" "finnish"

set +o xtrace
