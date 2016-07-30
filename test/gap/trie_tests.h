//
// Created by dc on 6/8/16.
//

#ifndef GAR_TRIE_TESTS_H
#define GAR_TRIE_TESTS_H

#include <gtest/gtest.h>
#include "trie.h"

using namespace gap;

class TrieTest : public ::testing::Test {
public:
    typedef std::shared_ptr<int> Content;
    Content testData_;
    Trie<int>*   trie_;

    TrieTest()
        : trie_(nullptr),
          testData_(Content(new int))
    {}

    static void SetUpTestCase(){
    }

    static void TearDownTestCase(){}

    void SetUp() {
        // create an empty trie
        trie_ = new Trie<int>();
    }

    void TearDown() {
        // free the trie if it exists
        if (trie_) {
            delete trie_;
            trie_ = nullptr;
        }
    }

    ~TrieTest() {
        TearDown();
    }
};


TEST_F(TrieTest, InitialStateTest) {
    ASSERT_EQ(trie_->search("a"), nullptr);
}

TEST_F(TrieTest, AddTest) {
    ASSERT_EQ(trie_->search("a"), nullptr);
    // Adding non-existent key
    ASSERT_TRUE(trie_->add("a", testData_));
    // Adding existing key
    ASSERT_FALSE(trie_->add("a", testData_));
    // Adding successive keys
    ASSERT_TRUE(trie_->add("aa", testData_));
    ASSERT_TRUE(trie_->add("aaa", testData_));
    // Adding key starting with different characters
    ASSERT_TRUE(trie_->add("b", testData_));
    ASSERT_TRUE(trie_->add("bb", testData_));
    ASSERT_TRUE(trie_->add("ab", testData_));
    ASSERT_TRUE(trie_->add("aba", testData_));
    ASSERT_TRUE(trie_->add("bba", testData_));
    // Adding random data
    ASSERT_TRUE(trie_->add("charter", testData_));
    ASSERT_TRUE(trie_->add("charm", testData_));
    ASSERT_TRUE(trie_->add("chest", testData_));
    ASSERT_TRUE(trie_->add("bchatder", testData_));
    // Adding long keys
    ASSERT_TRUE(trie_->add("aaaaaaaaaaaaaa", testData_));
    ASSERT_TRUE(trie_->add("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa", testData_));
    // Add in between of existing key
    ASSERT_TRUE(trie_->add("aaaaaa", testData_));
    ASSERT_TRUE(trie_->add("aaaaaaaa", testData_));
    //trie_->dump();
}

TEST_F(TrieTest, SearchTest) {
    ASSERT_EQ(trie_->search("a"), nullptr);
    // Add data and find
    ASSERT_TRUE(trie_->add("one", testData_));
    ASSERT_EQ(trie_->search("one"), testData_);

    ASSERT_TRUE(trie_->add("on", testData_));
    ASSERT_EQ(trie_->search("on"), testData_);

    ASSERT_TRUE(trie_->add("a", testData_));
    ASSERT_EQ(trie_->search("a"), testData_);

    // search long keys
    ASSERT_TRUE(trie_->add("aaaaaaaaaaaaaaaaaaaaaaaaaaaa", testData_));
    ASSERT_EQ(trie_->search("aaaaaaaaaaaaaaaaaaaaaaaaaaaa"), testData_);

    ASSERT_TRUE(trie_->add("aaaababababababababababababaa", testData_));
    ASSERT_EQ(trie_->search("aaaababababababababababababaa"), testData_);

    // Search after fail to insert because key exists
    ASSERT_FALSE(trie_->add("one", testData_));
    ASSERT_EQ(trie_->search("one"), testData_);

    // Search non-existent (also midkey)
    ASSERT_EQ(trie_->search("o"), nullptr);
    ASSERT_EQ(trie_->search("one1"), nullptr);
    ASSERT_EQ(trie_->search("aa"), nullptr);
    ASSERT_EQ(trie_->search("aaaaaaaaaaaa"), nullptr);
    ASSERT_EQ(trie_->search("aaaabababababa"), nullptr);
    ASSERT_EQ(trie_->search("zzzzzzzzzzzzzz"), nullptr);
}


TEST_F(TrieTest, RemoveTest) {
    ASSERT_EQ(trie_->search("a"), nullptr);
    // Test regular remove, add item, remove it, search it and ensure it's null
    ASSERT_TRUE(trie_->add("a", testData_));
    ASSERT_EQ(trie_->search("a"), testData_);
    trie_->erase("a");
    ASSERT_EQ(trie_->search("a"), nullptr);

    // add 2 keys, a & aa, delete a and ensure aa still exists
    ASSERT_TRUE(trie_->add("a", testData_));
    ASSERT_TRUE(trie_->add("aa", testData_));
    trie_->erase("a");
    ASSERT_EQ(trie_->search("a"), nullptr);
    ASSERT_EQ(trie_->search("aa"), testData_);
    // add back a and delete aa
    ASSERT_TRUE(trie_->add("a", testData_));
    trie_->erase("aa");
    ASSERT_EQ(trie_->search("aa"), nullptr);
    ASSERT_EQ(trie_->search("a"), testData_);
    // erase all
    trie_->erase("a");
    ASSERT_EQ(trie_->search("a"), nullptr);


    // erasing mid, start,
    ASSERT_TRUE(trie_->add("a", testData_));
    ASSERT_TRUE(trie_->add("ab", testData_));
    ASSERT_TRUE(trie_->add("abc", testData_));
    ASSERT_TRUE(trie_->add("abcd", testData_));
    ASSERT_TRUE(trie_->add("abcde", testData_));
    ASSERT_TRUE(trie_->add("abcdef", testData_));

    trie_->erase("abcd");
    ASSERT_EQ(trie_->search("abcd"), nullptr);
    ASSERT_EQ(trie_->search("abc"), testData_);
    ASSERT_EQ(trie_->search("abcde"), testData_);

    trie_->erase("a");
    ASSERT_EQ(trie_->search("a"), nullptr);
    ASSERT_EQ(trie_->search("ab"), testData_);

    trie_->erase("abcdef");
    ASSERT_EQ(trie_->search("abcdef"), nullptr);
    ASSERT_EQ(trie_->search("abcde"), testData_);

    // erasing no existent entry (or already erased) is ok
    trie_->erase("a");
    trie_->erase("c");
    ASSERT_EQ(trie_->search("abc"), testData_);
}

#endif //GAR_TRIE_TESTS_H
