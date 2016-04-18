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

#include "morph.h"

namespace morfessor {

BaselineModel::BaselineModel(const Corpus& corpus)
    : Model(corpus, AlgorithmModes::kBaseline, 0.5, 7.0, 1.0) {}

BaselineLengthModel::BaselineLengthModel(const Corpus& corpus,
    double most_common_morph_length, double beta)
    : Model(corpus, AlgorithmModes::kBaselineLength,
        0.5, most_common_morph_length, beta) {}

BaselineFrequencyModel::BaselineFrequencyModel(const Corpus& corpus,
    double hapax_legomena_prior)
    : Model(corpus, AlgorithmModes::kBaselineFreq, hapax_legomena_prior,
        7.0, 1.0) {}

BaselineFrequencyLengthModel::BaselineFrequencyLengthModel(
    const Corpus& corpus, double hapax_legomena_prior,
    double most_common_morph_length, double beta)
    : Model(corpus, AlgorithmModes::kBaselineFreqLength, hapax_legomena_prior,
        most_common_morph_length, beta) {}

Model::Model(const Corpus& corpus, AlgorithmModes mode, double hapax,
    double most_common_morph_length, double beta)
    : algorithm_mode_{mode},
      gamma_{most_common_morph_length / beta + 1, beta} {
  // Set gamma parameters
  assert(beta > 0);
  assert(most_common_morph_length > 0);
  assert(most_common_morph_length < 24*beta);

  // Set prior for hapax legomena proportion
  assert(hapax > 0 && hapax < 1);
  log2_hapax_ = std::log2(1 - hapax);

  // We have to know this before we can accurately calculate some of the
  // adjustments later on.
  UpdateLetterProbabilities(corpus);

  for (auto iter = corpus.cbegin(); iter != corpus.cend(); ++iter) {
    ++unique_morph_types_;
    total_morph_tokens_ += iter->frequency();
    adjust_frequency_cost(iter->frequency());
    adjust_string_cost(iter->letters(), true);
    adjust_length_cost(iter->letters().length());
    adjust_corpus_cost(iter->frequency());
  }
}

Model::~Model() {}

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
    letter_probabilities_[' '] =
        log_total_letters - std::log2(total_morph_tokens);
  }
}

}  // namespace morfessor
