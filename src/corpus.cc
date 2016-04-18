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

#include "corpus.h"

#include <cassert>
#include <fstream>
#include <sstream>

#include "morph.h"

namespace morfessor
{

Corpus::Corpus(std::istream& in) {
  init(in);
}

Corpus::Corpus(std::string word_file)
: words_{}
{
	std::ifstream file{word_file};
	assert(file.is_open());
	init(file);
}

void Corpus::init(std::istream& in) {
  std::string line;
  while (getline(in, line))
  {
    std::stringstream ssline{line};
    size_t freq;
    std::string morph_string;
    ssline >> freq;
    ssline >> morph_string;
    words_.emplace_back(morph_string, freq);
  }
}

} // namespace morfessor
