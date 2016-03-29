/*
 * CorpusTests.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: Derek Felson
 */

#include "Corpus.h"
#include "Morph.h"
#include <gtest/gtest.h>

TEST(CorpusTests, EmptyCorpusSize)
{
	auto corpus = Corpus("../testdata/EmptyCorpus.txt");
	EXPECT_EQ(0, corpus.size());
}

TEST(CorpusTests, OneLineCorpusSize)
{
	auto corpus = Corpus("../testdata/SingleCorpusRow.txt");
	EXPECT_EQ(1, corpus.size());
}

TEST(CorpusTests, CorpusSize)
{
	auto corpus = Corpus("../testdata/CorpusTestData.txt");
	EXPECT_EQ(4, corpus.size());
}

TEST(CorpusTests, EmptyCorpusIterate)
{
	auto corpus = Corpus("../testdata/EmptyCorpus.txt");
	for (const auto& m : corpus)
	{
		EXPECT_FALSE(true);
	}
}

TEST(CorpusTests, OneLineCorpusIterate)
{
	auto corpus = Corpus("../testdata/SingleCorpusRow.txt");
	auto count = 0;
	for (const auto& m : corpus)
	{
		++count;
		EXPECT_EQ(548, m.frequency());
		EXPECT_EQ("abandon", m.letters());
	}
	EXPECT_EQ(1, count);
}

TEST(CorpusTests, CorpusIterate)
{
	auto corpus = Corpus("../testdata/CorpusTestData.txt");
	EXPECT_EQ(4, corpus.size());
	auto count = 0;
	for (const auto& m : corpus)
	{
		++count;
	}
	EXPECT_EQ(4, count);

	auto iter = corpus.cbegin();
	EXPECT_EQ(548, iter->frequency());
	EXPECT_EQ("abandon", iter->letters());
	++iter;
	EXPECT_EQ(779, iter->frequency());
	EXPECT_EQ("deck", iter->letters());
	++iter;
	EXPECT_EQ(8, iter->frequency());
	EXPECT_EQ("decker", iter->letters());
	++iter;
	EXPECT_EQ(195, iter->frequency());
	EXPECT_EQ("declining", iter->letters());
	++iter;
	EXPECT_EQ(corpus.cend(), iter);
}
