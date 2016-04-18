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

#ifndef INCLUDE_CORPUS_H_
#define INCLUDE_CORPUS_H_

#include <string>
#include <vector>

#include "morph.h"

namespace morfessor
{

class Corpus
{
 public:
  using iterator = std::vector<Morph>::iterator;
  using const_iterator = std::vector<Morph>::const_iterator;
  explicit Corpus(std::string word_file);
  size_t size() const noexcept { return words_.size(); }
  iterator begin() noexcept { return words_.begin(); }
  iterator end() noexcept { return words_.end(); }
  const_iterator cbegin() const noexcept { return words_.cbegin(); }
  const_iterator cend() const noexcept { return words_.cend(); }

 private:
  std::vector<Morph> words_;
};

} // namespace morfessor

#endif /* INCLUDE_CORPUS_H_ */
