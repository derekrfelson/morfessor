/*
 * Model.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include "Types.h"
class Morph;
class Model;
class Lexicon;
class Corpus;

Probability pMorph(const Morph& morph);
Probability pCorpusGivenModel(const Corpus& corpus, const Model& model);
Probability pLexicon(const Lexicon& lexicon);
Probability pMorphFrequenciesImplicit(const Lexicon& lexicon);
Probability pMorphFrequenciesExplicit(const Lexicon& lexicon);
Probability pMorphLengthImplicit(const Lexicon& lexicon);
Probability pMorphLengthExplicit(const Lexicon& lexicon);

#endif /* INCLUDE_MODEL_H_ */
