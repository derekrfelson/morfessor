/*
 * Morph.h
 *
 *  Created on: Mar 28, 2016
 *      Author: derek
 */

#ifndef INCLUDE_MORPH_H_
#define INCLUDE_MORPH_H_

#include <string>
#include "types.h"

namespace morfessor
{

class Morph
{
 public:
  Morph(std::string letters, size_t frequency);
  std::string letters() const noexcept { return letters_; }
  size_t frequency() const noexcept { return frequency_; }
  size_t length() const noexcept { return letters_.length(); }
 private:
  std::string letters_;
  size_t frequency_;
};

} // namespace morfessor

#endif /* INCLUDE_MORPH_H_ */
