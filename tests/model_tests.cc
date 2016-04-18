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

#include <memory>

#include <gtest/gtest.h>

#include "corpus.h"

using Model = morfessor::Model;
using BaselineFrequencyModel = morfessor::BaselineFrequencyModel;
using BaselineModel = morfessor::BaselineModel;
using BaselineLengthModel = morfessor::BaselineLengthModel;
using BaselineFrequencyLengthModel = morfessor::BaselineFrequencyLengthModel;
using Corpus = morfessor::Corpus;

constexpr double threshold = 0.0001;

Corpus corpus1{"../testdata/test1.txt"};
Corpus corpus2{"../testdata/test2.txt"};
Corpus corpus3{"../testdata/test3.txt"};
Corpus corpus4{"../testdata/test4.txt"};

class ModelTests: public ::testing::Test {
 protected:
  std::unique_ptr<Model> model1;
  std::unique_ptr<Model> model2;
  std::unique_ptr<Model> model3;
  std::unique_ptr<Model> model4;

  void check_explicit_frequency_cost() {
    EXPECT_NEAR(7.90689, model1->frequency_cost(), threshold);
    EXPECT_NEAR(14.07682, model2->frequency_cost(), threshold);
    EXPECT_NEAR(2341.87284, model3->frequency_cost(), threshold);
    EXPECT_NEAR(822962.11904, model4->frequency_cost(), threshold);
  }

  void check_implicit_frequency_cost() {
    EXPECT_NEAR(4165.46499, model3->frequency_cost(), threshold);
    EXPECT_NEAR(1444149.68019, model4->frequency_cost(), threshold);
  }

  void check_explicit_length_cost() {
    EXPECT_NEAR(8.46787, model1->length_cost(), threshold);
    EXPECT_NEAR(11.70718, model2->length_cost(), threshold);
    EXPECT_NEAR(1623.19011, model3->length_cost(), threshold);
    EXPECT_NEAR(564175.94227, model4->length_cost(), threshold);
  }

  void check_implicit_length_cost() {
    EXPECT_NEAR(8.59521, model1->length_cost(), threshold);
    EXPECT_NEAR(11.70400, model2->length_cost(), threshold);
    EXPECT_NEAR(1323.25343, model3->length_cost(), threshold);
    EXPECT_NEAR(410817.33961, model4->length_cost(), threshold * 5);
  }

  void check_string_cost_with_implicit_length() {
    EXPECT_NEAR(66.42218, model1->morph_string_cost(), threshold);
    EXPECT_NEAR(99.38380, model2->morph_string_cost(), threshold);
    EXPECT_NEAR(17957.46139, model3->morph_string_cost(), threshold);
    EXPECT_NEAR(6198711.87999, model4->morph_string_cost(), threshold * 5);
  }

  void check_string_cost_with_explicit_length() {
    EXPECT_NEAR(62.37530, model1->morph_string_cost(), threshold);
    EXPECT_NEAR(94.09193, model2->morph_string_cost(), threshold);
    EXPECT_NEAR(17022.40186, model3->morph_string_cost(), threshold);
    EXPECT_NEAR(5810609.35413, model4->morph_string_cost(), threshold * 5);
  }

  void check_corpus_cost() {
    EXPECT_NEAR(9.65148, model1->corpus_cost(), threshold);
    EXPECT_NEAR(24.60336, model2->corpus_cost(), threshold);
    EXPECT_NEAR(194245.30310, model3->corpus_cost(), threshold);
    EXPECT_NEAR(252489771.98888, model4->corpus_cost(), threshold * 5);
  }

  void check_lexicon_order_cost() {
    EXPECT_NEAR(-0.42680, model1->lexicon_order_cost(), threshold);
    EXPECT_NEAR(-2.22922, model2->lexicon_order_cost(), threshold);
    EXPECT_NEAR(-3689.91118, model3->lexicon_order_cost(), threshold);
    EXPECT_NEAR(-2662975.89120, model4->lexicon_order_cost(), threshold);
  }

  void check_implicit_letter_probabilities() {
    auto lp = model1->letter_costs();

    EXPECT_NEAR(2.86507, lp['#'], threshold);
    EXPECT_NEAR(4.67243, lp['d'], threshold);
    EXPECT_NEAR(3.67243, lp['e'], threshold);
    EXPECT_NEAR(3.08746, lp['g'], threshold);
    EXPECT_NEAR(3.08746, lp['i'], threshold);
    EXPECT_NEAR(2.86507, lp['n'], threshold);
    EXPECT_NEAR(4.08746, lp['o'], threshold);
    EXPECT_NEAR(5.67243, lp['p'], threshold);
    EXPECT_NEAR(2.86507, lp['r'], threshold);
    EXPECT_NEAR(3.67243, lp['t'], threshold);
    EXPECT_NEAR(3.67243, lp['y'], threshold);
  }

  void check_explicit_letter_probabilities() {
    auto lp = model1->letter_costs();

    EXPECT_EQ(lp.end(), lp.find('#'));
    EXPECT_NEAR(4.45943, lp['d'], threshold);
    EXPECT_NEAR(3.45943, lp['e'], threshold);
    EXPECT_NEAR(2.87447, lp['g'], threshold);
    EXPECT_NEAR(2.87447, lp['i'], threshold);
    EXPECT_NEAR(2.65208, lp['n'], threshold);
    EXPECT_NEAR(3.87447, lp['o'], threshold);
    EXPECT_NEAR(5.45943, lp['p'], threshold);
    EXPECT_NEAR(2.65208, lp['r'], threshold);
    EXPECT_NEAR(3.45943, lp['t'], threshold);
    EXPECT_NEAR(3.45943, lp['y'], threshold);
  }
};

class BaselineModelTests: public ModelTests {
 protected:
  virtual void SetUp() {
    model1 = std::make_unique<BaselineModel>(corpus1);
    model2 = std::make_unique<BaselineModel>(corpus2);
    model3 = std::make_unique<BaselineModel>(corpus3);
    model4 = std::make_unique<BaselineModel>(corpus4);
  }

  virtual void TearDown() {
  }
};

class BaselineFrequencyModelTests: public ModelTests {
 protected:
  virtual void SetUp() {
    model1 = std::make_unique<BaselineFrequencyModel>(corpus1);
    model2 = std::make_unique<BaselineFrequencyModel>(corpus2);
    model3 = std::make_unique<BaselineFrequencyModel>(corpus3);
    model4 = std::make_unique<BaselineFrequencyModel>(corpus4);
  }

  virtual void TearDown() {
  }
};

class BaselineLengthModelTests: public ModelTests {
 protected:
  virtual void SetUp() {
    model1 = std::make_unique<BaselineLengthModel>(corpus1);
    model2 = std::make_unique<BaselineLengthModel>(corpus2);
    model3 = std::make_unique<BaselineLengthModel>(corpus3);
    model4 = std::make_unique<BaselineLengthModel>(corpus4);
  }

  virtual void TearDown() {
  }
};

class BaselineFrequencyLengthModelTests: public ModelTests {
 protected:
  virtual void SetUp() {
    model1 = std::make_unique<BaselineFrequencyLengthModel>(corpus1);
    model2 = std::make_unique<BaselineFrequencyLengthModel>(corpus2);
    model3 = std::make_unique<BaselineFrequencyLengthModel>(corpus3);
    model4 = std::make_unique<BaselineFrequencyLengthModel>(corpus4);
  }

  virtual void TearDown() {
  }
};

// Baseline Model tests

TEST_F(BaselineModelTests, OverallCost) {
  EXPECT_NEAR(214001.57173, model3->overall_cost(), threshold);
  EXPECT_NEAR(257880474.99748, model4->overall_cost(), threshold * 5);
}

TEST_F(BaselineModelTests, LexiconCost) {
  EXPECT_NEAR(19756.26863, model3->lexicon_cost(), threshold);
  EXPECT_NEAR(5390703.00858, model4->lexicon_cost(), threshold * 5);
}

TEST_F(BaselineModelTests, FrequencyCost) {
  check_implicit_frequency_cost();
}

TEST_F(BaselineModelTests, MorphStringCost) {
  check_string_cost_with_implicit_length();
}

TEST_F(BaselineModelTests, LengthCost) {
  check_implicit_length_cost();
}

TEST_F(BaselineModelTests, CorpusCost) {
  check_corpus_cost();
}

TEST_F(BaselineModelTests, LexiconOrderCost) {
  check_lexicon_order_cost();
}

TEST_F(BaselineModelTests, IndividualLetterCosts) {
  check_implicit_letter_probabilities();
}

// Baseline Frequency Model tests

TEST_F(BaselineFrequencyModelTests, OverallCost) {
  EXPECT_NEAR(92.14896, model1->overall_cost(), threshold);
  EXPECT_NEAR(147.53875, model2->overall_cost(), threshold);
  EXPECT_NEAR(212177.97957, model3->overall_cost(), threshold);
  EXPECT_NEAR(257259287.43630, model4->overall_cost(), threshold * 5);
}

TEST_F(BaselineFrequencyModelTests, LexiconCost) {
  EXPECT_NEAR(82.49748, model1->lexicon_cost(), threshold);
  EXPECT_NEAR(122.93539, model2->lexicon_cost(), threshold);
  EXPECT_NEAR(17932.67648, model3->lexicon_cost(), threshold);
  EXPECT_NEAR(4769515.44743, model4->lexicon_cost(), threshold);
}

TEST_F(BaselineFrequencyModelTests, FrequencyCost) {
  check_explicit_frequency_cost();
}

TEST_F(BaselineFrequencyModelTests, MorphStringCost) {
  check_string_cost_with_implicit_length();
}

TEST_F(BaselineFrequencyModelTests, LengthCost) {
  check_implicit_length_cost();
}

TEST_F(BaselineFrequencyModelTests, CorpusCost) {
  check_corpus_cost();
}

TEST_F(BaselineFrequencyModelTests, LexiconOrderCost) {
  check_lexicon_order_cost();
}

TEST_F(BaselineFrequencyModelTests, IndividualLetterCosts) {
  check_implicit_letter_probabilities();
}

// Baseline Length Model tests

TEST_F(BaselineLengthModelTests, OverallCost) {
  EXPECT_NEAR(213366.44888, model3->overall_cost(), threshold);
  EXPECT_NEAR(257645731.07427, model4->overall_cost(), threshold * 5);
}

TEST_F(BaselineLengthModelTests, LexiconCost) {
  EXPECT_NEAR(19121.14578, model3->lexicon_cost(), threshold);
  EXPECT_NEAR(5155959.08539, model4->lexicon_cost(), threshold);
}

TEST_F(BaselineLengthModelTests, FrequencyCost) {
  check_implicit_frequency_cost();
}

TEST_F(BaselineLengthModelTests, MorphStringCost) {
  check_string_cost_with_explicit_length();
}

TEST_F(BaselineLengthModelTests, LengthCost) {
  check_explicit_length_cost();
}

TEST_F(BaselineLengthModelTests, CorpusCost) {
  check_corpus_cost();
}

TEST_F(BaselineLengthModelTests, LexiconOrderCost) {
  check_lexicon_order_cost();
}

TEST_F(BaselineLengthModelTests, IndividualLetterCosts) {
  check_explicit_letter_probabilities();
}

// Baseline Frequency Length Model tests

TEST_F(BaselineFrequencyLengthModelTests, OverallCost) {
  EXPECT_NEAR(87.97474, model1->overall_cost(), threshold);
  EXPECT_NEAR(142.25007, model2->overall_cost(), threshold);
  EXPECT_NEAR(211542.85672, model3->overall_cost(), threshold);
  EXPECT_NEAR(257024543.51312, model4->overall_cost(), threshold * 5);
}

TEST_F(BaselineFrequencyLengthModelTests, LexiconCost) {
  EXPECT_NEAR(78.32325, model1->lexicon_cost(), threshold);
  EXPECT_NEAR(117.64671, model2->lexicon_cost(), threshold);
  EXPECT_NEAR(17297.55363, model3->lexicon_cost(), threshold);
  EXPECT_NEAR(4534771.52424, model4->lexicon_cost(), threshold);
}

TEST_F(BaselineFrequencyLengthModelTests, FrequencyCost) {
  check_explicit_frequency_cost();
}

TEST_F(BaselineFrequencyLengthModelTests, MorphStringCost) {
  check_string_cost_with_explicit_length();
}

TEST_F(BaselineFrequencyLengthModelTests, LengthCost) {
  check_explicit_length_cost();
}

TEST_F(BaselineFrequencyLengthModelTests, CorpusCost) {
  check_corpus_cost();
}

TEST_F(BaselineFrequencyLengthModelTests, LexiconOrderCost) {
  check_lexicon_order_cost();
}

TEST_F(BaselineFrequencyLengthModelTests, IndividualLetterCosts) {
  check_explicit_letter_probabilities();
}
