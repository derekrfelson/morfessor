/*
 * Corpus.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#include "corpus.h"

#include <cassert>
#include <fstream>
#include <sstream>

#include "morph.h"

namespace morfessor
{

Corpus::Corpus(std::string word_file)
: words_{}
{
	std::ifstream file{word_file};
	assert(file.is_open());
	std::string line;
	while (getline(file, line))
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
