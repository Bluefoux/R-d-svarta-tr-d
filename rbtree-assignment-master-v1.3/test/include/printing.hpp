// #pragma once

// #include <cmath>
// #include <queue>
// #include <string>

// //#include "RedBlackTree.hpp"

// // template <typename T>
// // struct Node {
// //   Node(T value) : value(value) {}
// //   Node* parent = nullptr;
// //   Node* left = nullptr;
// //   Node* right = nullptr;
// //   T value;
// // };

// template <typename T>
// std::string dotNode(const typename RBTree<T>::Node* n) {
//   if (n == nullptr) {
//     return "";
//   }
//   std::string result;
//   if (n->parent != nullptr) {
//     result += std::to_string(n->parent->value);
//     result += " -> ";
//     result += std::to_string(n->value);
//     result += '\n';
//   }
//   result += dotNode(n->left);
//   result += dotNode(n->right);
//   return result;
// }

// template <typename T>
// std::string dotIt(const typename RBTree<T>::Node* n) {
//   std::string result = "digraph treetime {\n";
//   result += dotNode(n);
//   result += "}";
//   return result;
// }

// template <typename T>
// int maxDepth(const typename RBTree<T>::Node* n) {
//   if (n == nullptr) {
//     return 0;
//   }
//   return 1 + std::max(maxDepth<T>(n->leftChild), maxDepth<T>(n->rightChild));
// }

// template <typename T>
// struct PrintNode {
//   PrintNode(const typename RBTree<T>::Node* node, int level, std::string chars)
//       : node(node), level(level), chars(chars) {}
//   const typename RBTree<T>::Node* node;
//   int level;
//   std::string chars;
// };

// template <typename T>
// void addChild(std::queue<PrintNode<T>>& q, typename RBTree<T>::Node* child, int level) {
//   static typename RBTree<T>::Node fake;
//   if (child != nullptr) {
//     q.push({child, level, std::to_string(child->element)});
//   } else if (level > 0) {
//     q.push({&fake, level, " "});
//   }
// }

// template <typename T>
// std::string printNode(const typename RBTree<T>::Node* n) {
//   std::string result;
//   std::queue<PrintNode<T>> q;
//   q.push({n, maxDepth<T>(n), std::to_string(n->element)});
//   int prevDepth = q.front().level;
//   while (!q.empty()) {
//     PrintNode<T> curr = q.front();
//     q.pop();
//     int indent = (int)std::pow(2, curr.level) / 2;
//     if (prevDepth != curr.level) {
//       result += '\n';
//       prevDepth = curr.level;
//     }
//     int withdraw = curr.chars.length() / 2;
//     int withdraw2 = curr.chars.length() - withdraw;
//     result += std::string(std::max(0, indent - withdraw), ' ');
//     result += curr.chars;
//     result += std::string(std::max(0, indent - withdraw2), ' ');

//     addChild<T>(q, curr.node->leftChild, curr.level - 1);
//     addChild<T>(q, curr.node->rightChild, curr.level - 1);
//   }
//   return result;
// }
