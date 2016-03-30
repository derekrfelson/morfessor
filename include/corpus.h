/*
 * Corpus.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#ifndef INCLUDE_CORPUS_H_
#define INCLUDE_CORPUS_H_

#include <string>
#include <vector>

namespace morfessor
{

class Morph;

class Corpus
{
 public:
  explicit Corpus(std::string word_file);
  size_t size() const noexcept { return words_.size(); }
  auto begin() noexcept { return words_.begin(); }
  auto end() noexcept { return words_.end(); }
  auto cbegin() const noexcept { return words_.cbegin(); }
  auto cend() const noexcept { return words_.cend(); }

 private:
  std::vector<Morph> words_;
};

} // namespace morfessor

#endif /* INCLUDE_CORPUS_H_ */
