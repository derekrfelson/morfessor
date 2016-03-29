/*
 * Morph.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Derek Felson
 */

#include "morph.h"

namespace morfessor
{

Morph::Morph(std::string letters, size_t frequency)
: letters_{letters},
  frequency_{frequency}
{
}

} // namespace morfessor
