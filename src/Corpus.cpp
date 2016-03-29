/*
 * Corpus.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#include "Corpus.h"
#include "Morph.h"
#include <fstream>
#include <cassert>
#include <sstream>

Corpus::Corpus(std::string wordFile)
: m_words{}
{
	std::ifstream file{wordFile};
	assert(file.is_open());
	std::string line;
	while (getline(file, line))
	{
		std::stringstream ssLine{line};
		size_t freq;
		std::string morphString;
		ssLine >> freq;
		ssLine >> morphString;
		m_words.emplace_back(morphString, freq);
	}
}

size_t Corpus::size() const
{
	return m_words.size();
}
