/*
 * Lexicon.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#ifndef INCLUDE_LEXICON_H_
#define INCLUDE_LEXICON_H_

#include <vector>

namespace morfessor
{

class Morph;

class Lexicon
{
 private:
  std::vector<Morph> morphs_;
};

} // namespace morfessor

#endif /* INCLUDE_LEXICON_H_ */
