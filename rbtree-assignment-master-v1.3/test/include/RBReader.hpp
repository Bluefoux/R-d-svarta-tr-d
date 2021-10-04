#ifndef RBREADER_HPP
#define RBREADER_HPP

//#define FMT_HEADER_ONLY
//#define FMT_UNICODE 0

#include <fmt/color.h>

#include <cmath>
#include <functional>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <optional>

#include "RBTree.hpp"

template <typename U>
class RBReader {
 public:
  using NodeU = typename RBTree<U>::Node;
  struct RBNode : public NodeU {};

  RBTree<U>* tree;

  RBReader(RBTree<U>* tree) : tree(tree){};

  RBNode* root() { return static_cast<RBNode*>(tree->root); }

  static std::string dotNode(const NodeU* n) {
    if (n == nullptr) {
      return "";
    }
    std::string result;
    if (n->parent != nullptr) {
      result += std::to_string(n->parent->value);
      result += " -> ";
      result += std::to_string(n->value);
      result += '\n';
    }
    result += dotNode(n->left);
    result += dotNode(n->right);
    return result;
  }

  static std::string dotIt(const NodeU* n) {
    std::string result = "digraph treetime {\n";
    result += dotNode(n);
    result += "}";
    return result;
  }

  static int maxDepth(const NodeU* n) {
    if (n == nullptr) {
      return 0;
    }
    return 1 + std::max(maxDepth(n->leftChild), maxDepth(n->rightChild));
  }

  struct PrintNode {
    PrintNode() {}
    PrintNode(const NodeU* node, int level, std::string chars)
        : node(node), level(level), chars(chars) {}
    const NodeU* node;
    int level;
    std::string chars;
  };

  std::string graphViz() { return tree->ToGraphviz(); }

  std::string printExtra() {
    return std::to_string("The tree looks like:\n") + print() +
           std::to_string("\n where * indicates nil\n\n") +
           std::to_string("Dot/GraphViz representation below:\n---\n") +
           tree->ToGraphviz() + std::to_string("\n---\n");
  }

  std::string print() const {
    std::string result;
    auto perNode = [&](PrintNode prev, PrintNode curr) {
      int indent = (int)std::pow(2, curr.level) / 2;
      if (prev.level != curr.level) {
        result += '\n';
      }
      int withdraw = curr.chars.length() / 2;
      int withdraw2 = curr.chars.length() - withdraw;
      result += std::string(std::max(0, indent - withdraw), ' ');
      if (curr.node->colour == Colour::RED) {
        result += fmt::format(fg(fmt::color::red), curr.chars);
      } else {
        result += fmt::format(fg(fmt::color::blue), curr.chars);
      }
      result += std::string(std::max(0, indent - withdraw2), ' ');
    };

    auto getLeft = [](PrintNode curr) -> NodeU* {
      return curr.node->leftChild;
    };
    auto getRight = [](PrintNode curr) -> NodeU* {
      return curr.node->rightChild;
    };

    auto insert = [&](std::queue<PrintNode>& q, std::optional<PrintNode> curr,
                      NodeU* child) {
      int level = curr.has_value() ? curr->level - 1 : maxDepth(child);
      static NodeU fake;
      if (child == tree->nilNode) {
        q.push({&fake, level, "*"});
      } else if (child != nullptr) {
        q.push({child, level, std::to_string(child->element)});
      } else if (level > 0) {
        q.push({&fake, level, " "});
      }
    };

    customVisitBreadthFirst<PrintNode>(tree->root, perNode, getLeft, getRight,
                                       insert);
    return result;
  }

  int cntNodes() {
    int cnt = 0;
    visitBreadthFirst(tree->root, [&](NodeU*, NodeU*) { ++cnt; });
    return cnt;
  }

  bool noDuplicateElements() {
    int cnt = 0;
    std::set<U> elements;
    auto perNode = [&](NodeU* prev, NodeU* curr) {
      ++cnt;
      elements.insert(curr->element);
    };
    visitBreadthFirst(tree->root, perNode);
    return cnt == (int)elements.size();
  }

  bool noLeavesAreNull() {
    bool noNullFound = true;
    auto perNode = [&](NodeU* prev, NodeU* curr) {
      if (curr->leftChild == nullptr || curr->rightChild == nullptr) {
        noNullFound = false;
      }
    };
    visitBreadthFirst(tree->root, perNode);
    return noNullFound;
  }

  bool nilIsBlack() {
    if (tree->nilNode != nullptr) {
      return tree->nilNode->colour == Colour::BLACK;
    }
    return false;
  }

  bool redNodesHaveBlackChildren() {
    return redNodesHaveBlackChildren(tree->root);
  }

  bool redNodesHaveBlackChildren(NodeU* curr) {
    if (curr == tree->nilNode) {
      return true;
    }

    if (curr->colour == Colour::RED) {
      if (curr->leftChild->colour == Colour::RED ||
          curr->rightChild->colour == Colour::RED) {
        return false;
      }
    }
    return redNodesHaveBlackChildren(curr->leftChild) &&
           redNodesHaveBlackChildren(curr->rightChild);
  }

  bool allLeavesHaveSameNumberOfBlackAncestors() {
    std::set<int> blackAncestors;
    int cnt = 0;
    ancestorQuery(blackAncestors, cnt, tree->root);
    return blackAncestors.size() == 1;
  }

  void ancestorQuery(std::set<int>& blackAncestors, int cnt, NodeU* curr) {
    if (curr == tree->nilNode) {
      blackAncestors.insert(cnt);
      return;
    }
    if (curr->colour == Colour::BLACK) {
      cnt++;
    }
    ancestorQuery(blackAncestors, cnt, curr->leftChild);
    ancestorQuery(blackAncestors, cnt, curr->rightChild);
  }

  bool pathToAncestorsCorrect() {
    std::vector<U> elements = tree->inOrder();
    for (auto e : elements) {
      std::vector<U> path = tree->pathFromRoot(e);
      int currIdx = 0;
      NodeU* currNode = tree->root;
      while (currNode != tree->nilNode && currIdx < (int)path.size()) {
        if (currNode->element != path[currIdx]) {
          return false;
        }
        if (e >= currNode->element) {
          currNode = currNode->rightChild;
        } else {
          currNode = currNode->leftChild;
        }
        ++currIdx;
      }
      if (currIdx != (int)path.size()) {
        return false;
      }
    }
    return true;
  }

  void visitBreadthFirst(NodeU* n,
                         std::function<void(NodeU*, NodeU*)> perNode) const {
    auto getLeft = [](NodeU* curr) -> NodeU* { return curr->leftChild; };
    auto getRight = [](NodeU* curr) -> NodeU* { return curr->rightChild; };
    auto insert = [&](std::queue<NodeU*>& q, std::optional<NodeU*>,
                      NodeU* child) {
      if (child != tree->nilNode && child != nullptr) {
        q.push(child);
      }
    };
    customVisitBreadthFirst<NodeU*>(tree->root, perNode, getLeft, getRight,
                                    insert);
  }

  void sabotage() { tree->root->leftChild->colour = Colour::RED; }

  template <typename T>
  void customVisitBreadthFirst(
      NodeU* n, std::function<void(T, T)> perNode,
      std::function<NodeU*(T)> fetchLeft, std::function<NodeU*(T)> fetchRight,
      std::function<void(std::queue<T>&, std::optional<T>, NodeU*)> insertFn)
      const {
    std::queue<T> q;
    insertFn(q, {}, n);
    T prev = {};
    T curr = {};
    while (!q.empty()) {
      prev = curr;
      curr = q.front();
      q.pop();
      insertFn(q, curr, fetchLeft(curr));
      insertFn(q, curr, fetchRight(curr));
      perNode(prev, curr);
    }
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const RBReader<T>& reader) {
  os << reader.printExtra();
  return os;
}

#endif