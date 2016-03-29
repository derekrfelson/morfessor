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
class Morph;

class Corpus
{
public:
	explicit Corpus(std::string wordFile);
	size_t size() const;
private:
	std::vector<Morph> m_words;
};

#endif /* INCLUDE_CORPUS_H_ */
