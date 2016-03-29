/*
 * Morph.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#include "Morph.h"

Morph::Morph(std::string letters, size_t frequency)
: m_letters{letters},
  m_frequency{frequency}
{
}

std::string Morph::letters() const
{
	return m_letters;
}

size_t Morph::frequency() const
{
	return m_frequency;
}

size_t Morph::length() const
{
	return m_letters.size();
}
