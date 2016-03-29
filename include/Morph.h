/*
 * Morph.h
 *
 *  Created on: Mar 28, 2016
 *      Author: derek
 */

#ifndef INCLUDE_MORPH_H_
#define INCLUDE_MORPH_H_

#include "Types.h"
#include <string>

class Morph
{
public:
	explicit Morph(std::string letters, size_t frequency);
	std::string letters() const;
	size_t frequency() const;
	size_t size() const;
private:
	std::string m_letters;
	size_t m_frequency;
};

#endif /* INCLUDE_MORPH_H_ */
