/*
 * CorpusTests.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Derek Felson
 */

#include "Corpus.h"
#include "Morph.h"
#include <gtest/gtest.h>

TEST(Corpus, EmptyCorpusSize)
{
	auto corpus = Corpus("../testdata/EmptyCorpus.txt");
	EXPECT_EQ(0, corpus.size());
}

TEST(DatasetTests, OneLineCorpusSize)
{
	auto corpus = Corpus("../testdata/SingleCorpusRow.txt");
	EXPECT_EQ(1, corpus.size());
}

TEST(DatasetTests, CorpusSize)
{
	auto corpus = Corpus("../testdata/CorpusTestData.txt");
	EXPECT_EQ(4, corpus.size());
}
