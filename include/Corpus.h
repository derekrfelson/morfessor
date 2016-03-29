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
private:
	using Collection = std::vector<Morph>;
	using iterator = Collection::iterator;
	using const_iterator = Collection::const_iterator;

public:
	explicit Corpus(std::string wordFile);
	size_t size() const;
	iterator begin() noexcept;
	iterator end() noexcept;
	const_iterator cbegin() const noexcept;
	const_iterator cend() const noexcept;

private:
	Collection m_words;
};

#endif /* INCLUDE_CORPUS_H_ */
