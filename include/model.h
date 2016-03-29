/*
 * Model.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include <vector>

#include "types.h"

namespace morfessor
{

class Morph;
class Lexicon;
class Corpus;
class Node;

class Model
{
 private:
  std::vector<Morph> morphs_;
  std::vector<Node> nodes_;
};

Probability pMorph(const Morph& morph);
Probability pCorpusGivenModel(const Corpus& corpus, const Model& model);
Probability pLexicon(const Lexicon& lexicon);
Probability pMorphFrequenciesImplicit(const Lexicon& lexicon);
Probability pMorphFrequenciesExplicit(const Lexicon& lexicon);
Probability pMorphLengthImplicit(const Lexicon& lexicon);
Probability pMorphLengthExplicit(const Lexicon& lexicon);

} // namespace morfessor

#endif /* INCLUDE_MODEL_H_ */
