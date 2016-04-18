// The MIT License (MIT)
//
// Copyright (c) 2016 Derek Felson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef TESTS_CORPUS_LOADER_H_
#define TESTS_CORPUS_LOADER_H_

#include "corpus.h"

namespace morfessor {

namespace tests {

struct CorpusLoader {
 public:
  CorpusLoader()
     : corpus1{"../testdata/test1.txt"},
       corpus2{"../testdata/test2.txt"},
       corpus3{"../testdata/test3.txt"},
       corpus4{"../testdata/test4.txt"} {}

  const Corpus corpus1;
  const Corpus corpus2;
  const Corpus corpus3;
  const Corpus corpus4;
};

CorpusLoader& corpus_loader();

}  // namespace tests

}  // namespace morfessor

#endif /* TESTS_CORPUS_LOADER_H_ */
