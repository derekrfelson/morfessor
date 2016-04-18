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

#ifndef INCLUDE_MODEL_H_
#define INCLUDE_MODEL_H_

#include <cstddef>
#include <cmath>
#include <cassert>
#include <unordered_map>
#include <memory>

#include <boost/math/distributions/gamma.hpp>

#include "corpus.h"
#include "types.h"

namespace morfessor {

class Model {
 public:
  /// Makes a model for analyzing the corpus using the chosen algorithm.
  Model(const Corpus& corpus, AlgorithmModes mode);

  /// D'tor.
  virtual ~Model();

  /// Returns the overall cost, consisting of the cost of the lexicon and
  /// the cost of the corpus given the model.
  Cost overall_cost() const;

  /// Returns the cost of the lexicon.
  Cost lexicon_cost() const;

  /// Returns the cost of the corpus given the model.
  Cost corpus_cost() const;

  /// The cost adjustment based on the n! ways to order the morphs.
  Cost lexicon_order_cost() const;

  /// Returns the cost of the morph frequencies.
  Cost frequency_cost() const;

  /// Returns the cost of the morph lengths.
  Cost length_cost() const;

  /// Returns the cost of all the morph strings.
  Cost morph_string_cost() const;

  /// Returns the number of morph tokens (unique morphs * their frequencies).
  size_t total_morph_tokens() const noexcept;

  /// Returns the number of unique morphs in the data structure.
  size_t unique_morph_types() const noexcept;

  /// Adds or subtracts from the morph token count.
  /// @param delta The number of tokens to add or remove.
  void adjust_morph_token_count(int delta);

  /// Adds or subtracts from the unique morph count.
  /// @param delta The number of unique morphs to add or remove.
  void adjust_unique_morph_count(int delta);

  /// Adjusts the frequency cost based on the number of tokens added or
  /// removed.
  void adjust_frequency_cost(int delta_morph_frequency);

  /// Adjusts the corpus cost based on the number of tokens added or removed.
  void adjust_corpus_cost(int delta_morph_frequency);

  /// Adjust the length cost based on the number of letters added or removed.
  void adjust_length_cost(int delta_morph_length);

  /// Adjust the string cost based on what string was added or removed.
  void adjust_string_cost(const std::string& str, bool add);

  /// A change in overall cost less than this means the algorithm can stop.
  Cost convergence_threshold() const noexcept;

 protected:
  /// Sets the prior belief for the proportion of morphs that only occur once
  /// in the corpus. Typically this value is between 0.4 and 0.6 for English.
  /// @value The expected proportion. Must be in range (0,1)
  void set_hapax_legomena_prior(double value);

  /// Stop optimizing when the cost per morph improves less than this amount.
  /// @param value Must be > 0 and < 1.
  void set_convergence_threshold(double value);

  /// Sets the parameters for the gamma distribution. Used for calculating
  /// the cost of morph lengths explicitly in Morfessor Baseline Length
  /// and Morfessor Baseline Freq Length modes.
  /// @param most_common_morph_length Must be > 0 and < 24*beta.
  /// @param beta Must be > 0.
  /// @see set_algorithm_mode
  void set_gamma_parameters(double most_common_morph_length, double beta);

  /// Recalculates the probabilities of each letter in the corpus, and the
  /// end-of-morph marker.
  /// @param include_end_of_string Whether to consider "end of string" a
  ///   letter when computing probabilities. Should be true when using
  ///   the implicit length variants of the algorithm.
  /// @return A map of letters to code lengths
  void UpdateLetterProbabilities(const Corpus& corpus);

 private:
  /// Whether to use the zipf distribution for morph lengths.
  bool explicit_length() const noexcept;

  /// Whether to use the gamma distribution for morph frequencies.
  bool explicit_frequency() const noexcept;

  /// Returns the code length of a morph given its frequency.
  /// @param count Must be greater than 0.
  /// @see set_hapax_legomena_prior
  Cost explicit_frequency_cost(size_t frequency) const;

  /// Returns the code length of a morph given its length.
  /// @param length Must be greater than 0.
  /// @see set_gamma_parameters
  Cost explicit_length_cost(size_t length) const;

  /// Calculates the cost from morph frequencies, using the implicit method.
  Cost ImplicitFrequencyCost() const;

  /// Returns the code length of a morph given its length.
  /// @param length The actual length of the morph, not including the "end of
  ///   morph" marker.
  Cost implicit_length_cost(size_t length) const;

  /// The cost an individual morph contributed to the corpus, based on the
  /// morph's frequency. Frequency must be > 0.
  /// @param frequency Number of times the morph appears in the corpus.
  Cost morph_in_corpus_cost(size_t frequency) const;

  /// Part of the lexicon cost.
  Cost cost_from_frequencies_ = 0;

  /// Part of the lexicon cost.
  Cost cost_from_lengths_ = 0;

  /// Part of the lexicon cost.
  Cost cost_from_strings_ = 0;

  /// Part of the corpus cost.
  Cost cost_from_corpus_ = 0;

  /// Part of the lexicon cost.
  Cost cost_from_lexicon_order_ = 0;

  /// Part of the corpus cost.
  Cost cost_from_corpus_log_token_sum_ = 0;

  /// Number of morph tokens in the data structure. Whereas unique_morph_types_
  /// equals the number of unique morphs, this number factors in the frequency
  /// of each morph in the corpus.
  size_t total_morph_tokens_ = 0;

  // Number of unique morphs in the data structure. Unlike total_morph_tokens_,
  // this number ignores the frequency, only counting each unique morph once.
  size_t unique_morph_types_ = 0;

  /// Optimization stops when one pass of resplitting the lexicon does not
  /// improve the overall cost by more than this amount per word. The smaller
  /// the value, the longer the program will run and the more accurate it will
  /// theoretically be, but expect greatly diminishing returns.
  /// Valid values are between 0 and 1, not inclusive.
  double convergence_threshold_ = 0.005;

  /// Number computed from a parameter used for explicit frequency costs.
  /// @see set_hapax_legomena_prior
  double log2_hapax_ = -1.0;

  /// The gamma distribution used for explicit length costs.
  /// @see set_gamma_parameters
  boost::math::gamma_distribution<double> gamma_;

  /// Governs which variant of the Morfessor Baseline algorithm to use.
  AlgorithmModes algorithm_mode_ = AlgorithmModes::kBaseline;

  /// Contains the probabilities of each letter in the corpus.
  /// The "end of morph" marker is '#'.
  std::unordered_map<char, Cost> letter_probabilities_;
};

class BaselineModel : public Model {
 public:
  BaselineModel(const Corpus& corpus);
};

class BaselineLengthModel : public Model {
 public:
  BaselineLengthModel(const Corpus& corpus,
      double most_common_morph_length = 7.0,
      double beta = 1.0);
};

class BaselineFrequencyModel : public Model {
 public:
  BaselineFrequencyModel(const Corpus& corpus,
      double hapax_legomena_prior = 0.5);
};

class BaselineFrequencyLengthModel : public Model {
 public:
  BaselineFrequencyLengthModel(const Corpus& corpus,
      double hapax_legomena_prior = 0.5,
      double most_common_morph_length = 7.0,
      double beta = 1.0);
};

inline void Model::set_hapax_legomena_prior(double value) {
  assert(value > 0 && value < 1);
  log2_hapax_ = std::log2(1 - value);
}

inline size_t Model::total_morph_tokens() const noexcept {
  return total_morph_tokens_;
}

inline size_t Model::unique_morph_types() const noexcept {
  return unique_morph_types_;
}

inline void Model::set_convergence_threshold(double value) {
  assert(value > 0 && value < 1);
  convergence_threshold_ = value;
}

inline void Model::set_gamma_parameters(
    double most_common_morph_length, double beta) {
  assert(beta > 0);
  assert(most_common_morph_length > 0);
  assert(most_common_morph_length < 24*beta);
  gamma_ = boost::math::gamma_distribution<double>{
    most_common_morph_length / beta + 1, beta};
}

inline bool Model::explicit_length() const noexcept {
  return algorithm_mode_ == AlgorithmModes::kBaselineLength
      || algorithm_mode_ == AlgorithmModes::kBaselineFreqLength;
}

inline bool Model::explicit_frequency() const noexcept {
  return algorithm_mode_ == AlgorithmModes::kBaselineFreq
      || algorithm_mode_ == AlgorithmModes::kBaselineFreqLength;
}

inline Cost Model::explicit_frequency_cost(size_t frequency) const {
  return -std::log2(std::pow(frequency, log2_hapax_)
                    - std::pow(frequency + 1, log2_hapax_));
}

inline Cost Model::explicit_length_cost(size_t length) const {
  return -std::log2(boost::math::pdf(gamma_, length));
}

inline Cost Model::implicit_length_cost(size_t length) const {
  return letter_probabilities_.at('#');
}

inline Cost Model::morph_in_corpus_cost(size_t frequency) const {
  assert(frequency > 0);
  return std::log(static_cast<double>(frequency) / total_morph_tokens_);
}

inline Cost Model::lexicon_order_cost() const {
  // Use the first term of Sterling's approximation
  // log n! ~ n * log(n - 1)
  return (unique_morph_types_ * (1 - std::log(unique_morph_types_)))
      / std::log(2);
}

inline Cost Model::overall_cost() const {
  return lexicon_cost() + corpus_cost();
}

inline Cost Model::corpus_cost() const {
  return cost_from_corpus_ / std::log(2);
}

inline Cost Model::lexicon_cost() const {
  return cost_from_lexicon_order_ + cost_from_frequencies_
      + cost_from_lengths_ + cost_from_strings_;
}

inline Cost Model::frequency_cost() const {
  if (explicit_frequency()) {
    return cost_from_frequencies_;
  } else {
    return ImplicitFrequencyCost();
  }
}

inline Cost Model::length_cost() const {
  if (explicit_length()) {
    return cost_from_lengths_;
  } else {
    return letter_probabilities_.at('#') * unique_morph_types_;
  }
}

inline Cost Model::morph_string_cost() const {
  return cost_from_strings_;
}

inline Cost Model::convergence_threshold() const noexcept {
  return convergence_threshold_ * unique_morph_types_;
}

inline void Model::adjust_morph_token_count(int delta) {
  assert(total_morph_tokens_ + delta >= 0);
  total_morph_tokens_ += delta;
}

inline void Model::adjust_unique_morph_count(int delta) {
  assert(unique_morph_types_ + delta >= 0);
  unique_morph_types_ += delta;
}

inline void Model::adjust_frequency_cost(int delta_morph_frequency) {
  // Explicit frequency cost needs to be adjusted as morphs are added and
  // removed. Implicit frequency cost is just a simple calculation at the end.
  if (explicit_frequency())
  {
    cost_from_frequencies_ += delta_morph_frequency >= 0
        ? explicit_frequency_cost(delta_morph_frequency)
        : -explicit_frequency_cost(-delta_morph_frequency);
  }
}

inline void Model::adjust_corpus_cost(int delta_morph_frequency) {
  cost_from_corpus_ += delta_morph_frequency >= 0
      ? delta_morph_frequency * morph_in_corpus_cost(delta_morph_frequency)
      : -delta_morph_frequency * morph_in_corpus_cost(-delta_morph_frequency);
}

inline void Model::adjust_length_cost(int delta_morph_length) {
  if (explicit_length()) {
    cost_from_lengths_ += delta_morph_length >= 0
        ? explicit_length_cost(delta_morph_length)
        : -explicit_length_cost(-delta_morph_length);
  } else {
    cost_from_lengths_ += delta_morph_length >= 0
        ? implicit_length_cost(delta_morph_length)
        : -implicit_length_cost(-delta_morph_length);
  }
}

inline void Model::adjust_string_cost(const std::string& str, bool add) {
  Cost sum = 0;
  for (auto c : str) {
    sum += letter_probabilities_.at(c);
  }
  cost_from_strings_ += (add ? 1 : -1) * sum;
}

}  // namespace morfessor

#endif /* INCLUDE_MODEL_H_ */
