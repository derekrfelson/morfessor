// The MIT License (MIT)
//
// Copyright (c) 2016 Derek Felson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#include <string>

namespace morfessor
{

/// An actual probability between 0 and 1.
using Probability = double;

/// A cost is a -log2 probability, also called code length.
using Cost = double;

/// Represents the four variants of the Morfessor Baseline algorithm.
enum class AlgorithmModes : unsigned int {
  /// Uses implicit frequency and length formulas
  kBaseline = 0,
  /// Uses explicit frequency formula, implicit length formula
  kBaselineFreq,
  /// Uses implicit frequency formula, explicit length formula
  kBaselineLength,
  /// Uses explicit frequency and length formulas
  kBaselineFreqLength
};

} // namespace morfessor

#endif /* INCLUDE_TYPES_H_ */
