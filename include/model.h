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
  /// @param hapax The prior belief for the proportion of morphs that only
  ///   occur once in the corpus. Typically this value is between 0.4 and
  ///   0.6 for English. Must be > 0 and < 1.
  /// @param convergence Must be > 0 and < 1.
  /// @param most_common_morph_length Must be > 0 and < 24*beta.
  /// @param beta Must be > 0.
  Model(const Corpus& corpus, AlgorithmModes mode, double hapax,
      double most_common_morph_len, double beta);

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

  /// Returns the convergence threshold. A change in overall cost less than
  /// this means the algorithm can stop.
  Cost convergence_threshold() const noexcept;

  /// Returns the map of individual letter costs.
  std::unordered_map<char, Cost> letter_costs() const noexcept;

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

 private:
  /// Recalculates the probabilities of each letter in the corpus, and the
  /// end-of-morph marker.
  /// @param include_end_of_string Whether to consider "end of string" a
  ///   letter when computing probabilities. Should be true when using
  ///   the implicit length variants of the algorithm.
  /// @return A map of letters to code lengths
  void UpdateLetterProbabilities(const Corpus& corpus);

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
  /// The "end of morph" marker is ' '.
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

inline std::unordered_map<char, Cost> Model::letter_costs() const noexcept {
  return letter_probabilities_;
}

inline Cost Model::convergence_threshold() const noexcept {
  return convergence_threshold_ * unique_morph_types_;
}

// Morph token count

inline size_t Model::total_morph_tokens() const noexcept {
  return total_morph_tokens_;
}

inline void Model::adjust_morph_token_count(int delta) {
  assert(delta >= 0 || -delta <= total_morph_tokens_);
  total_morph_tokens_ += delta;
}

// Unique morph count

inline size_t Model::unique_morph_types() const noexcept {
  return unique_morph_types_;
}

inline void Model::adjust_unique_morph_count(int delta) {
  assert(delta >= 0 || -delta <= unique_morph_types_);
  unique_morph_types_ += delta;
}

// Frequency cost

inline Cost Model::frequency_cost() const {
  if (explicit_frequency()) {
    return cost_from_frequencies_;
  } else {
    // Formula with logarithmic approximation to binomial coefficients
    // based on Stirling's approximation.
    return (total_morph_tokens_ - 1) * std::log2(total_morph_tokens_ - 2)
          - (unique_morph_types_ - 1) * std::log2(unique_morph_types_ - 2)
          - (total_morph_tokens_ - unique_morph_types_)
            * std::log2(total_morph_tokens_ - unique_morph_types_ - 1);
  }
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

inline Cost Model::explicit_frequency_cost(size_t frequency) const {
  return -std::log2(std::pow(frequency, log2_hapax_)
                    - std::pow(frequency + 1, log2_hapax_));
}

inline bool Model::explicit_frequency() const noexcept {
  return algorithm_mode_ == AlgorithmModes::kBaselineFreq
      || algorithm_mode_ == AlgorithmModes::kBaselineFreqLength;
}

// Corpus cost

inline Cost Model::corpus_cost() const {
  return (total_morph_tokens_ * log(total_morph_tokens_)
      - cost_from_corpus_log_token_sum_) / std::log(2);
}

inline void Model::adjust_corpus_cost(int delta_morph_frequency) {
    cost_from_corpus_log_token_sum_ +=
        delta_morph_frequency * std::log(std::abs(delta_morph_frequency));
}

// Length cost

inline Cost Model::length_cost() const {
  if (explicit_length()) {
    return cost_from_lengths_;
  } else {
    return letter_probabilities_.at(' ') * unique_morph_types_;
  }
}

inline void Model::adjust_length_cost(int delta_morph_length) {
  if (explicit_length()) {
    cost_from_lengths_ += delta_morph_length >= 0
        ? explicit_length_cost(delta_morph_length)
        : -explicit_length_cost(-delta_morph_length);
  } else {
    delta_morph_length >= 0
        ? cost_from_lengths_ += letter_probabilities_.at(' ')
        : cost_from_lengths_ -= letter_probabilities_.at(' ');
  }
}

inline Cost Model::explicit_length_cost(size_t length) const {
  return -std::log2(boost::math::pdf(gamma_, length));
}

inline bool Model::explicit_length() const noexcept {
  return algorithm_mode_ == AlgorithmModes::kBaselineLength
      || algorithm_mode_ == AlgorithmModes::kBaselineFreqLength;
}

// Morph string cost

inline Cost Model::morph_string_cost() const {
  return cost_from_strings_;
}

inline void Model::adjust_string_cost(const std::string& str, bool add) {
  Cost sum = 0;
  for (auto c : str) {
    sum += letter_probabilities_.at(c);
  }
  cost_from_strings_ += (add ? 1 : -1) * sum;
}

// Lexicon order cost

inline Cost Model::lexicon_order_cost() const {
  // Use the first term of Sterling's approximation
  // log n! ~ n * log(n - 1)
  return (unique_morph_types_ * (1 - std::log(unique_morph_types_)))
      / std::log(2);
}

// Lexicon cost

inline Cost Model::lexicon_cost() const {
  return lexicon_order_cost() + frequency_cost()
      + length_cost() + morph_string_cost();
}

// Overall cost

inline Cost Model::overall_cost() const {
  return lexicon_cost() + corpus_cost();
}

}  // namespace morfessor

#endif /* INCLUDE_MODEL_H_ */
