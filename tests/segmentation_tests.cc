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

#include "segmentation.h"

#include <memory>
#include <sstream>
#include <iostream>

#include <gtest/gtest.h>

#include "corpus.h"
#include "model.h"
#include "corpus_loader.h"

using Model = morfessor::Model;
using BaselineFrequencyModel = morfessor::BaselineFrequencyModel;
using BaselineModel = morfessor::BaselineModel;
using BaselineLengthModel = morfessor::BaselineLengthModel;
using BaselineFrequencyLengthModel = morfessor::BaselineFrequencyLengthModel;
using Corpus = morfessor::Corpus;
using Segmentation = morfessor::Segmentation;
static auto corpus_loader = &morfessor::tests::corpus_loader;

constexpr double threshold = 0.0001;

template <class T>
static void test_against_reference(
    std::shared_ptr<T> calculated_model,
    const Segmentation& segmentation) {

  std::stringstream results;
  segmentation.print_as_corpus(results);
  std::cout << std::endl << "sstream:" << std::endl;
  segmentation.print_as_corpus(std::cout);
  std::cout << "end of sstream" << std::endl << std::endl;
  Corpus results_corpus{results};
  T results_model(results_corpus);

  EXPECT_NEAR(results_model.overall_cost(),
      calculated_model->overall_cost(), threshold);
  EXPECT_NEAR(results_model.lexicon_cost(),
      calculated_model->lexicon_cost(), threshold);
  EXPECT_NEAR(results_model.corpus_cost(),
      calculated_model->corpus_cost(), threshold);
  EXPECT_NEAR(results_model.frequency_cost(),
      calculated_model->frequency_cost(), threshold);
  EXPECT_NEAR(results_model.length_cost(),
      calculated_model->length_cost(), threshold);
  EXPECT_NEAR(results_model.morph_string_cost(),
      calculated_model->morph_string_cost(), threshold);
  EXPECT_NEAR(results_model.lexicon_order_cost(),
      calculated_model->lexicon_order_cost(), threshold);
}

TEST(SegmentationTests, OptimizeBaselineFreqLength) {
  auto model1 = std::make_shared<BaselineFrequencyLengthModel>(
      corpus_loader().corpus1);
  auto model2 = std::make_shared<BaselineFrequencyLengthModel>(
      corpus_loader().corpus2);
  auto model3 = std::make_shared<BaselineFrequencyLengthModel>(
      corpus_loader().corpus3);
  auto model4 = std::make_shared<BaselineFrequencyLengthModel>(
      corpus_loader().corpus4);

  Segmentation s1(corpus_loader().corpus1, model1);
  Segmentation s2(corpus_loader().corpus2, model2);
  Segmentation s3(corpus_loader().corpus3, model3);
  Segmentation s4(corpus_loader().corpus4, model4);

  s1.Optimize();
  s2.Optimize();
  s3.Optimize();
  s4.Optimize();

  test_against_reference(model1, s1);
  test_against_reference(model2, s2);
  test_against_reference(model3, s3);
  test_against_reference(model4, s4);
}

TEST(SegmentationTests, AdjustMorphCountCanRemoveNodes) {
  auto model1 = std::make_shared<BaselineFrequencyModel>(
      corpus_loader().corpus1);
  Segmentation s1(corpus_loader().corpus1, model1);

  s1.AdjustMorphCount("redoing", -2);

  std::stringstream expected_results;
  expected_results << "4 trying" << std::endl << "1 reopen" << std::endl;
  std::stringstream results;
  s1.print_as_corpus(results);
  EXPECT_EQ(expected_results.str(), results.str());
}

TEST(SegmentationTests, BaselineLengthSaneAfterSplitting) {
  auto model1 = std::make_shared<BaselineLengthModel>(
      corpus_loader().corpus1);
  Segmentation s1(corpus_loader().corpus1, model1);

  s1.AdjustMorphCount("redoing", -2);
  s1.AdjustMorphCount("re", 2);
  s1.AdjustMorphCount("doing", 2);

  test_against_reference(model1, s1);
}

TEST(SegmentationTests, BaselineFreqLengthSaneAfterSplitting) {
  auto model1 = std::make_shared<BaselineFrequencyLengthModel>(
      corpus_loader().corpus1);
  Segmentation s1(corpus_loader().corpus1, model1);

  s1.AdjustMorphCount("redoing", -2);
  s1.AdjustMorphCount("re", 2);
  s1.AdjustMorphCount("doing", 2);

  test_against_reference(model1, s1);
}
