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

#include "model.h"

#include <cmath>

#include <boost/math/special_functions/binomial.hpp>

#include "morph.h"

namespace morfessor {

BaselineModel::BaselineModel(const Corpus& corpus)
    : Model(corpus, AlgorithmModes::kBaseline) {}

BaselineLengthModel::BaselineLengthModel(const Corpus& corpus,
    double most_common_morph_length, double beta)
    : Model(corpus, AlgorithmModes::kBaselineLength) {
  set_gamma_parameters(most_common_morph_length, beta);
  UpdateLetterProbabilities(corpus);
}

BaselineFrequencyModel::BaselineFrequencyModel(const Corpus& corpus,
    double hapax_legomena_prior)
    : Model(corpus, AlgorithmModes::kBaselineFreq) {
  set_hapax_legomena_prior(hapax_legomena_prior);
}

BaselineFrequencyLengthModel::BaselineFrequencyLengthModel(
    const Corpus& corpus, double hapax_legomena_prior,
    double most_common_morph_length, double beta)
    : Model(corpus, AlgorithmModes::kBaselineFreqLength) {
  set_hapax_legomena_prior(hapax_legomena_prior);
  set_gamma_parameters(most_common_morph_length, beta);
  UpdateLetterProbabilities(corpus);
}

Model::Model(const Corpus& corpus, AlgorithmModes mode)
    : gamma_{8,1}, algorithm_mode_{mode} {
  UpdateLetterProbabilities(corpus);
}

Model::~Model() {}

Cost Model::ImplicitFrequencyCost() const {
  // Formula without approximation
  if (total_morph_tokens_ < 100) {
    return std::log2(boost::math::binomial_coefficient<Probability>(
        total_morph_tokens_ - 1, unique_morph_types_ - 1));
  } else {
    // Formula with logarithmic approximation to binomial coefficients
    // based on Stirling's approximation
    //
    // return (total_morph_tokens_ - 1) * std::log2(total_morph_tokens_ - 2)
    // - (unique_morph_types - 1) * std::log2(unique_morph_types_ - 2)
    // - (total_morph_tokens_ - unique_morph_types_)
    //  * std::log2(total_morph_tokens_ - unique_morph_types_ - 1);
    //
    // The above should be the correct forumula to use here for a fast
    // approximation, but the Morfessor reference implementation uses
    // a slightly different version, which is used below.
    //
    // Formula from reference implementation
    return (total_morph_tokens_ - 1) * std::log2(total_morph_tokens_ - 2)
          - (unique_morph_types_ - 1) * std::log2(unique_morph_types_ - 2)
          - (total_morph_tokens_ - unique_morph_types_)
            * std::log2(total_morph_tokens_ - unique_morph_types_ - 1);
  }
}

void Model::UpdateLetterProbabilities(const Corpus& corpus)
{
  // Calculate the probabilities of each letter in the corpus
  letter_probabilities_.clear();
  size_t total_letters = 0;
  size_t unique_morphs = 0;
  size_t total_morph_tokens = 0;

  // Get the frequency of all the letters first
  for (auto iter = corpus.cbegin(); iter != corpus.cend(); ++iter) {
    ++unique_morphs;
    total_morph_tokens += iter->frequency();
    for (auto c : iter->letters())
    {
      total_letters += iter->frequency();
      // letter_probabiltieis actually contains count at this point
      letter_probabilities_[c] += iter->frequency();
    }
  }

  if (!explicit_length()) {
    // We count the "end of morph" character as a letter
    total_letters += total_morph_tokens;
  }

  // Calculate the actual letter costs using maximum likelihood
  auto log_total_letters = std::log2(total_letters);
  for (auto iter : letter_probabilities_)
  {
    letter_probabilities_.at(iter.first) =
        log_total_letters - std::log2(letter_probabilities_.at(iter.first));
  }

  if (!explicit_length()) {
    // The "end of morph string" character can be understood to appear
    // at the end of every string, i.e. total_morph_tokens number of times.
    letter_probabilities_['#'] =
        log_total_letters - std::log2(total_morph_tokens);
  }
}

}  // namespace morfessor
