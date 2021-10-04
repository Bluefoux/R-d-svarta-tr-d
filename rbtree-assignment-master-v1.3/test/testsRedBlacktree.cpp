#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>

#include "RBReader.hpp"
#include "RBTree.hpp"
#include "catch.hpp"

#define GZ_OUT                                                      \
  UNSCOPED_INFO(std::to_string("GraphViz representation:\n---\n") + \
                reader.graphViz());

template <typename T>
void STANDARD_TEST_CASES(RBTree<T>& rb, RBReader<T> reader, int count) {
  GZ_OUT
  THEN(fmt::format(
      "The tree should contain the correct amount of nodes, (excluding "
      "NIL): {}",
      count)) {
    REQUIRE(reader.cntNodes() == count);
  }

  THEN("The tree should not have any leaves pointing to nullptr") {
    REQUIRE(reader.noLeavesAreNull());
  }

  THEN("The nil node should be black") { REQUIRE(reader.nilIsBlack()); }

  THEN("All Red nodes should have black children") {
    REQUIRE(reader.redNodesHaveBlackChildren());
  }

  THEN("All leaves should have the same amount of black ancestors") {
    REQUIRE(reader.allLeavesHaveSameNumberOfBlackAncestors());
  }

  THEN("There should be no duplicate entries in the tree") {
    REQUIRE(reader.noDuplicateElements());
  }

  THEN("The paths produced by pathFromRoot() should be correct") {
    REQUIRE(reader.pathToAncestorsCorrect());
  }

  THEN(fmt::format("inorder() should return a vector of the right length: {}",
                   count)) {
    auto inOrder = rb.inOrder();
    REQUIRE((int)inOrder.size() == count);
    AND_THEN("The vector should also be sorted") {
      for (int i = 0; i < count - 1; ++i) {
        REQUIRE(inOrder[i] < inOrder[i + 1]);
      }
    }
  }
}

/** Returns an empty string to be able to trigger simply with the INFO macro */
std::string textToTestCaseTxt(std::string text) {
  std::ofstream myfile;
  myfile.open(Catch::getResultCapture().getCurrentTestName() + "-out.txt");
  myfile << text;
  myfile.close();
  return "";
}

SCENARIO("Some simple cases") {
  GIVEN("An empty tree") {
    const int ITERATIONS = 11;
    RBTree<int> rb;
    RBReader<int> reader(&rb);

    STANDARD_TEST_CASES<int>(rb, reader, 0);
    THEN("min() should throw on an empty tree") { CHECK_THROWS(rb.min()); }
    THEN("max() should throw on an empty tree") { CHECK_THROWS(rb.max()); }
    THEN("find(42) should return false") { REQUIRE(!rb.find(42)); }
    THEN("height should be 1") { REQUIRE(rb.height() == -1); }
    THEN("pathFromRoot(0) should return an empty list") {
      REQUIRE(rb.pathFromRoot(0).size() == 0);
    }

    WHEN("Inserting 0 - 10 as integers sequentially") {
      for (int i = 0; i < ITERATIONS; ++i) {
        rb.addNode(i);
      }
      STANDARD_TEST_CASES<int>(rb, reader, ITERATIONS);
      THEN("min() should return 0") { REQUIRE(rb.min() == 0); }
      THEN("max() should return 10") { REQUIRE(rb.max() == 10); }
      THEN("find(42) should return false") { REQUIRE(!rb.find(42)); }
      THEN("find(-1) should return false") { REQUIRE(!rb.find(-1)); }
      THEN("find(11) should return false") { REQUIRE(!rb.find(11)); }
      THEN("find(0) should return true") { REQUIRE(rb.find(0)); }
      THEN("find(10) should return true") { REQUIRE(rb.find(10)); }
      THEN("height should be 6") { REQUIRE(rb.height() == 4); }
      THEN("pathFromRoot(42) should return an empty list") {
        REQUIRE(rb.pathFromRoot(42).size() == 0);
      }
    }

    WHEN("Inserting 10 - 0 as integers sequentially") {
      for (int i = 0; i < ITERATIONS; ++i) {
        rb.addNode(ITERATIONS - 1 - i);
      }
      STANDARD_TEST_CASES<int>(rb, reader, ITERATIONS);
      THEN("min() should return 0") { REQUIRE(rb.min() == 0); }
      THEN("max() should return 10") { REQUIRE(rb.max() == 10); }
      THEN("find(42) should return false") { REQUIRE(!rb.find(42)); }
      THEN("find(-1) should return false") { REQUIRE(!rb.find(-1)); }
      THEN("find(11) should return false") { REQUIRE(!rb.find(11)); }
      THEN("find(0) should return true") { REQUIRE(rb.find(0)); }
      THEN("find(10) should return true") { REQUIRE(rb.find(10)); }
      THEN("height should be 6") { REQUIRE(rb.height() == 4); }
      THEN("pathFromRoot(42) should return an empty list") {
        REQUIRE(rb.pathFromRoot(42).size() == 0);
      }
    }

    WHEN("Inserting 42 ten times, only one node should be created") {
      for (int i = 0; i < ITERATIONS; ++i) {
        rb.addNode(42);
      }
      STANDARD_TEST_CASES<int>(rb, reader, 1);
      THEN("min() should return 42") { REQUIRE(rb.min() == 42); }
      THEN("max() should return 42") { REQUIRE(rb.max() == 42); }
      THEN("find(42) should return true") { REQUIRE(rb.find(42)); }
      THEN("height should be 2") { REQUIRE(rb.height() == 0); }
      THEN("pathFromRoot(42) should return a list with 42") {
        REQUIRE(rb.pathFromRoot(42).size() == 1);
      }
      THEN("pathFromRoot(42) should return a list with 42") {
        REQUIRE(rb.pathFromRoot(42) == std::vector<int>{42});
      }

      AND_WHEN("Deleting 43 ten times") {
        for (int i = 0; i < ITERATIONS; ++i) {
          rb.deleteNode(43);
        }
        THEN(
            "No faults should be triggered and the node 42 should still be in "
            "the tree") {
          STANDARD_TEST_CASES<int>(rb, reader, 1);
          THEN("min() should return 42") { REQUIRE(rb.min() == 42); }
          THEN("max() should return 42") { REQUIRE(rb.max() == 42); }
          THEN("find(42) should return true") { REQUIRE(rb.find(42)); }
          THEN("pathFromRoot(42) should return a list with 42") {
            REQUIRE(rb.pathFromRoot(42).size() == 1);
          }
          THEN("pathFromRoot(42) should return a list with 42") {
            REQUIRE(rb.pathFromRoot(42) == std::vector<int>{42});
          }
        }
      }

      AND_WHEN("Deleting 42 ten times") {
        for (int i = 0; i < ITERATIONS; ++i) {
          rb.deleteNode(42);
        }
        THEN(
            "No faults should be triggered and the node 42 should be removed "
            "from the tree. The tree should be empty") {
          STANDARD_TEST_CASES<int>(rb, reader, 0);
          THEN("min() should throw on an empty tree") {
            CHECK_THROWS(rb.min());
          }
          THEN("max() should throw on an empty tree") {
            CHECK_THROWS(rb.max());
          }
          THEN("find(42) should return false") { REQUIRE(!rb.find(42)); }
          THEN("pathFromRoot(42) should return an empty list") {
            REQUIRE(rb.pathFromRoot(42).size() == 0);
          }
        }
      }
    }
  }
}

SCENARIO("Random inserts") {
  GIVEN("An empty tree") {
    auto shuffler = std::default_random_engine(42);
    const int ITERATIONS = 100;
    int cnt = 0;
    RBTree<int> rb;
    RBReader<int> reader(&rb);
    std::vector<int> v(ITERATIONS);
    std::iota(std::begin(v), std::end(v), -ITERATIONS / 2);
    std::shuffle(v.begin(), v.end(), shuffler);
    WHEN("Inserting values") {
      for (int i : v) {
        // reader.sabotage();
        // INFO(textToTestCaseTxt(reader.printExtra()));
        STANDARD_TEST_CASES<int>(rb, reader, cnt);
        rb.addNode(i);
        ++cnt;
        STANDARD_TEST_CASES<int>(rb, reader, cnt);
      }
    }
  }
}

SCENARIO("Random deletes") {
  GIVEN("A filled tree") {
    auto shuffler = std::default_random_engine(42);
    const int ITERATIONS = 100;
    RBTree<int> rb;
    RBReader<int> reader(&rb);
    std::vector<int> v(ITERATIONS);
    std::iota(std::begin(v), std::end(v), -ITERATIONS / 2);
    std::shuffle(v.begin(), v.end(), shuffler);
    WHEN("Inserting values") {
      for (int i : v) {
        rb.addNode(i);
      }
      AND_THEN("Shuffling the same numbers and then removing them one by one") {
        std::shuffle(v.begin(), v.end(), shuffler);
        int cnt = ITERATIONS;
        for (int i : v) {
          // reader.sabotage();
          // INFO(textToTestCaseTxt(reader.printExtra()));
          INFO(fmt::format("Deleting: '{}'", i));
          STANDARD_TEST_CASES<int>(rb, reader, cnt);
          rb.deleteNode(i);
          --cnt;
          STANDARD_TEST_CASES<int>(rb, reader, cnt);
        }
      }
    }
  }
}

SCENARIO("Random inserts and deletes sequentially") {
  GIVEN("An empty tree") {
    auto shuffler = std::default_random_engine(42);
    const int ITERATIONS = 10;
    int cnt = 0;
    RBTree<int> rb;
    RBReader<int> reader(&rb);
    std::vector<int> v(ITERATIONS);
    std::iota(std::begin(v), std::end(v), -ITERATIONS / 2);
    std::shuffle(v.begin(), v.end(), shuffler);
    WHEN("Inserting values and removing them straight away") {
      for (int i : v) {
        // reader.sabotage();
        // INFO(textToTestCaseTxt(reader.printExtra()));
        STANDARD_TEST_CASES<int>(rb, reader, cnt);
        rb.addNode(i);
        ++cnt;
        rb.deleteNode(i);
        --cnt;
      }
    }
  }
}

SCENARIO("Filling the tree with something other than numbers") {
  GIVEN("An empty tree") {
    auto shuffler = std::default_random_engine(42);
    int cnt = 0;
    RBTree<char> rb;
    RBReader<char> reader(&rb);
    std::vector<char> v{'A', 'a', 'b', 'B', 'c', 'C', 'd',
                        'D', '0', 'O', '1', '2', '3'};
    std::shuffle(v.begin(), v.end(), shuffler);
    WHEN("Inserting values") {
      for (int i : v) {
        // reader.sabotage();
        // INFO(textToTestCaseTxt(reader.printExtra()));
        STANDARD_TEST_CASES<char>(rb, reader, cnt);
        rb.addNode(i);
        ++cnt;
        STANDARD_TEST_CASES<char>(rb, reader, cnt);
      }
      AND_THEN("Shuffling the same numbers and then removing them one by one") {
        std::shuffle(v.begin(), v.end(), shuffler);
        for (int i : v) {
          // reader.sabotage();
          // INFO(textToTestCaseTxt(reader.printExtra()));
          INFO(fmt::format("Deleting: '{}'", i));
          STANDARD_TEST_CASES<char>(rb, reader, cnt);
          rb.deleteNode(i);
          --cnt;
          STANDARD_TEST_CASES<char>(rb, reader, cnt);
        }
      }
    }
  }
}