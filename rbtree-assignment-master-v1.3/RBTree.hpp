#ifndef RBTREE_HPP
#define RBTREE_HPP

#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

enum class Colour { RED, BLACK };

namespace std {
std::string to_string(const std::string& str) { return str; }
std::string to_string(const Colour& colour) {
  return (colour == Colour::RED) ? "RED" : "BLACK";
}
}  // namespace std

// Remember to always do a make clean / refresh build when using templates
template <typename V>
class RBReader;

template <typename T>
class RBTree {
  friend RBReader<T>;

 private:
  struct Node {
    Node* parent = nullptr;
    Node* leftChild = nullptr;
    Node* rightChild = nullptr;
    Colour colour = Colour::RED;
    T element = T();
  };

  Node* root = nullptr;
  Node* nilNode = nullptr;

  void RB_Insert_Fixup(Node* TheNode);
  void Left_Rotate(Node* GrandfatherNode);
  void Right_Rotate(Node* TheNode2);
  void inOrderRec(Node* CurrNode, std::vector<T>& inOrderVec);
  int heightRec(Node* CurrNode);
  void RB_Transplant(Node* node, Node* nodechild);
  void RB_Delete_Fixup(Node* currentnode);
  bool findrec(Node* currnode, const T& element);

  int GzAddNode(std::string& nodes, std::string& connections, const Node* curr,
                size_t to);
  int GzAddChild(std::string& nodes, std::string& connections,
                 const Node* child, size_t from, size_t to,
                 const std::string& color);
  template <typename V>
  std::string GzNode(size_t to, const V& what, const std::string& style,
                     const std::string& fillColor,
                     const std::string& fontColor);
  std::string GzConnection(size_t from, size_t to, const std::string& color,
                           const std::string& style);

 public:
  RBTree();
  ~RBTree();

  RBTree(const RBTree& other) = delete;
  RBTree& operator=(const RBTree& other) = delete;
  RBTree(RBTree&& other) = delete;
  RBTree& operator=(RBTree&& other) = delete;
  bool addNode(const T& element);
  bool deleteNode(const T& element);
  bool find(const T& element);
  const T& min();
  const T& max();
  std::vector<T> inOrder();
  int height();
  std::vector<T> pathFromRoot(const T& element);
  std::string ToGraphviz();
};

template <typename T>
RBTree<T>::RBTree() {
  Node* x = new Node;
  x->colour = Colour::BLACK;
  nilNode = x;
  root = nilNode;
}
/*
Node* tmpNode = nullptr;
  static T tmp;
  if (root == nilNode){
    throw std::string("The tree is empty");
  }
  
  
  
*/
template <typename T>
RBTree<T>::~RBTree() {
  Node* tmpNode = nullptr;
  bool deleted = false;
  while (root != NULL && root !=nilNode){
    tmpNode= root;
    while (tmpNode->leftChild != nilNode){
      tmpNode = tmpNode->leftChild;
    }
    deleted = RBTree<T>::deleteNode(tmpNode->element);
    if (root != NULL && root !=nilNode){
      tmpNode= root;
      while (tmpNode->leftChild != nilNode){
        tmpNode = tmpNode->leftChild;
      }
      deleted = RBTree<T>::deleteNode(tmpNode->element);
    }
  }
  tmpNode = NULL;
  delete root;
}

template <typename T>
void RBTree<T>::Left_Rotate(Node* GrandfatherNode){
  Node* rotateNode = nullptr;
  rotateNode = GrandfatherNode->rightChild;
  GrandfatherNode->rightChild = rotateNode->leftChild;
  if (rotateNode->leftChild != nilNode){
    rotateNode->leftChild->parent = GrandfatherNode;
  }
  rotateNode->parent = GrandfatherNode->parent;
  if (GrandfatherNode->parent == nilNode){
    root = rotateNode;
  }
  else if (GrandfatherNode == GrandfatherNode->parent->leftChild){
    GrandfatherNode->parent->leftChild = rotateNode;
  }
  else{
    GrandfatherNode->parent->rightChild = rotateNode;
  }
  rotateNode->leftChild = GrandfatherNode;
  GrandfatherNode->parent = rotateNode;
  rotateNode = NULL;
}

template <typename T>
void RBTree<T>::Right_Rotate(Node* TheNode2){
  Node* rotateNode = nullptr;
  rotateNode = TheNode2->leftChild;
  TheNode2->leftChild = rotateNode->rightChild;
  if (rotateNode->rightChild != nilNode){
    rotateNode->rightChild->parent = TheNode2;
  }
  rotateNode->parent = TheNode2->parent;
  if (TheNode2->parent == nilNode){
    root = rotateNode;
  }
  else if (TheNode2 == TheNode2->parent->rightChild){
    TheNode2->parent->rightChild = rotateNode;
  }
  else{
    TheNode2->parent->leftChild = rotateNode;
  }
  rotateNode->rightChild = TheNode2;
  TheNode2->parent = rotateNode;
  rotateNode = NULL;
}

template <typename T>
void RBTree<T>::RB_Insert_Fixup(Node* TheNode){
  Node* fixNode = nullptr;
  while (TheNode->parent->colour == Colour::RED){
    if (TheNode->parent == TheNode->parent->parent->rightChild){
      fixNode = TheNode->parent->parent->leftChild;
      if (fixNode->colour == Colour::RED){
        TheNode->parent->colour = Colour::BLACK;
        fixNode->colour = Colour::BLACK;
        TheNode->parent->parent->colour = Colour::RED;
        TheNode = TheNode->parent->parent;
      }
      else{
        if (TheNode == TheNode->parent->leftChild){
          TheNode = TheNode->parent;
          Right_Rotate(TheNode);
        }
        TheNode->parent->colour = Colour::BLACK;
        TheNode->parent->parent->colour = Colour::RED;
        Left_Rotate(TheNode->parent->parent);
        }
    }
    else{
      fixNode = TheNode->parent->parent->rightChild;
      if (fixNode->colour == Colour::RED){
        TheNode->parent->colour = Colour::BLACK;
        fixNode->colour = Colour::BLACK;
        TheNode->parent->parent->colour = Colour::RED;
        TheNode = TheNode->parent->parent;
      }
      else{
        if (TheNode == TheNode->parent->rightChild){
          TheNode = TheNode->parent;
          Left_Rotate(TheNode);
        }
        TheNode->parent->colour = Colour::BLACK;
        TheNode->parent->parent->colour = Colour::RED;
        Right_Rotate(TheNode->parent->parent);
      }
      if (TheNode == root){
        break;
      }
    }
  }
  root->colour = Colour::BLACK;
  fixNode = NULL;
}

template <typename T>
bool RBTree<T>::addNode(const T& element) {
  if (RBTree<T>::find(element)){
    return false;
  }

  Node* newNode = new Node;
  Node* x = nullptr;
  Node* y = nullptr;
  x = root;
  y = nilNode;
  newNode->element = element;
  newNode->rightChild = nilNode;
  newNode->leftChild = nilNode;

  while (x != nilNode){
    y = x;
    if (newNode->element < x->element){
      x = x->leftChild;
    }
    else{
      x = x->rightChild;
    }
  }
  newNode->parent = y;
  if (y == nilNode){
    newNode->colour = Colour::BLACK;
    root = newNode;
  }
  else if (newNode->element < y->element){
    y->leftChild = newNode;
  }
  else{
    y->rightChild = newNode;
  }

  if (newNode->parent != nilNode){
    if (newNode->parent->parent != nilNode){
      RBTree<T>::RB_Insert_Fixup(newNode);
    }
  }
  else{
    newNode->colour = Colour::BLACK;
  }
  x = NULL;
  y = NULL;
  newNode = NULL;
  return true;
}

template <typename T>
void RBTree<T>::RB_Transplant(Node* node, Node* nodechild){
  if (node->parent == nilNode){
    root = nodechild;
  }
  else if (node == node->parent->leftChild){
    node->parent->leftChild = nodechild;
  }
  else{
    node->parent->rightChild = nodechild;
  }
  nodechild->parent = node->parent;
}

template <typename T>
void RBTree<T>::RB_Delete_Fixup(Node* currentnode){
  Node* tmpNode = nullptr;
  while (currentnode != root && currentnode->colour == Colour::BLACK){
    if (currentnode == currentnode->parent->leftChild){
      tmpNode = currentnode->parent->rightChild;
      if (tmpNode->colour == Colour::RED){
        tmpNode->colour = Colour::BLACK;
        currentnode->parent->colour = Colour::RED;
        Left_Rotate(currentnode->parent);
        tmpNode = currentnode->parent->rightChild;
      }

      if (tmpNode->leftChild->colour == Colour::BLACK && tmpNode->rightChild->colour == Colour::BLACK){
        tmpNode->colour = Colour::RED;
        currentnode = currentnode->parent;
      }
      else{
        if (tmpNode->rightChild->colour == Colour::BLACK){
          tmpNode->leftChild->colour = Colour::BLACK;
          tmpNode->colour = Colour::RED;
          Right_Rotate(tmpNode);
          tmpNode = currentnode->parent->rightChild;
        }

        tmpNode->colour = currentnode->parent->colour;
        currentnode->parent->colour = Colour::BLACK;
        tmpNode->rightChild->colour = Colour::BLACK;
        Left_Rotate(currentnode->parent);
        currentnode = root;
      }
    }
    else{
      tmpNode = currentnode->parent->leftChild;
      if (tmpNode->colour == Colour::RED){
        tmpNode->colour = Colour::BLACK;
        currentnode->parent->colour = Colour::RED;
        Right_Rotate(currentnode->parent);
        tmpNode = currentnode->parent->leftChild;
      }

      if (tmpNode->leftChild->colour == Colour::BLACK && tmpNode->rightChild->colour == Colour::BLACK){
        tmpNode->colour = Colour::RED;
        currentnode = currentnode->parent;
      }
      else{
        if (tmpNode->leftChild->colour == Colour::BLACK){
          tmpNode->rightChild->colour = Colour::BLACK;
          tmpNode->colour = Colour::RED;
          Left_Rotate(tmpNode);
          tmpNode = currentnode->parent->leftChild;
        }

        tmpNode->colour = currentnode->parent->colour;
        currentnode->parent->colour = Colour::BLACK;
        tmpNode->leftChild->colour = Colour::BLACK;
        Right_Rotate(currentnode->parent);
        currentnode = root;
      }
    }
  }
  currentnode->colour = Colour::BLACK;
  tmpNode = NULL;
}

template <typename T>
bool RBTree<T>::deleteNode(const T& element) {
  Node* tmpNode = nullptr;
  tmpNode = root;
  bool findnode = false;
  if (tmpNode == nilNode || tmpNode == NULL){
    return false;
  }
  
  while (((tmpNode->rightChild != nilNode) || (tmpNode->leftChild != nilNode)) && !findnode){
    if (element == tmpNode->element){
      findnode = true;
    }
    else if (element < tmpNode->element){
      tmpNode = tmpNode->leftChild;
    }
    else if (element > tmpNode->element){
      tmpNode = tmpNode->rightChild;
    }
  }
  if (element == tmpNode->element){
    findnode = true;
  }
  if (!findnode){
    return false;
  }
  Node* tmpNode2 = nullptr;
  Node* tmpNode3 = nullptr;
  tmpNode3 = tmpNode;
  Colour tmpNode3_orig_colour = tmpNode3->colour;
  if (tmpNode->leftChild == nilNode){
    tmpNode2 = tmpNode->rightChild;
    RBTree<T>::RB_Transplant(tmpNode, tmpNode->rightChild);
  }
  else if (tmpNode->rightChild == nilNode){
    tmpNode2 = tmpNode->leftChild;
    RBTree<T>::RB_Transplant(tmpNode, tmpNode->leftChild);
  }
  else{
    tmpNode3 = tmpNode->rightChild;
    while (tmpNode3->leftChild != nilNode){
      tmpNode3 = tmpNode3->leftChild;
    }
    tmpNode3_orig_colour = tmpNode3->colour;
    tmpNode2 = tmpNode3->rightChild;
    if (tmpNode3->parent == tmpNode){
      tmpNode2->parent = tmpNode3;
    }
    else{
      RBTree<T>::RB_Transplant(tmpNode3, tmpNode3->rightChild);
      tmpNode3->rightChild = tmpNode->rightChild;
      tmpNode3->rightChild->parent = tmpNode3;
    }
    RBTree<T>::RB_Transplant(tmpNode, tmpNode3);
    tmpNode3->leftChild = tmpNode->leftChild;
    tmpNode3->leftChild->parent = tmpNode3;
    tmpNode3->colour = tmpNode->colour;
  }
  delete tmpNode;
  if (tmpNode3_orig_colour == Colour::BLACK){
    RBTree<T>::RB_Delete_Fixup(tmpNode2);
  }
  tmpNode2 = NULL;
  tmpNode3 = NULL;
  return true;
}

template <typename T>
bool RBTree<T>::findrec(Node* currnode, const T& element){
  bool tmpbool = false;
  if (currnode == nilNode){
    return false;
  }
  if (element == currnode->element){
    return true;
  }
  if (element < currnode->element){
    tmpbool = findrec(currnode->leftChild, element);
    return tmpbool;
  }
  
  tmpbool = findrec(currnode->rightChild, element);
  return tmpbool;
}

template <typename T>
bool RBTree<T>::find(const T& element) {
  bool found = false;
  found = findrec(root, element);
  return found;
}

template <typename T>
const T& RBTree<T>::min() {
  // Replace with proper implementation
  Node* tmpNode = nullptr;
  static T tmp;
  if (root == nilNode){
    throw std::string("The tree is empty");
  }
  
  tmpNode= root;
  while (tmpNode->leftChild != nilNode){
    tmpNode = tmpNode->leftChild;
  }
  tmp = tmpNode->element;
  tmpNode = NULL;
  return tmp;
}

template <typename T>
const T& RBTree<T>::max() {
  // Replace with proper implementation
  Node* tmpNode = nullptr;
  static T tmp;
  if (root == nilNode){
    throw std::string("The tree is empty");
  }
  tmpNode= root;
  while (tmpNode->rightChild != nilNode){
    tmpNode = tmpNode->rightChild;
  }
  tmp = tmpNode->element;
  tmpNode = NULL;
  return tmp;
}

template <typename T>
std::vector<T> RBTree<T>::inOrder() {
  std::vector<T> order = {};
  RBTree<T>::inOrderRec(root, order);
  return order;
}

template <typename T>
void RBTree<T>::inOrderRec(Node* CurrNode, std::vector<T>& inOrderVec){
  if (CurrNode != nilNode){
    inOrderRec(CurrNode->leftChild, inOrderVec);
    inOrderVec.push_back(CurrNode->element);
    inOrderRec(CurrNode->rightChild, inOrderVec);
  }
}

template <typename T>
int RBTree<T>::heightRec(Node* CurrNode) {
  if (CurrNode == nilNode){
    return -1;
  }
  return (1+ std::max(heightRec(CurrNode->leftChild), (heightRec(CurrNode->rightChild))));
}

template <typename T>
int RBTree<T>::height() {
  int heigh = -1;
  if (root == nilNode){
    return heigh;
  }
  
  heigh = heightRec(root);
  return heigh;
}

template <typename T>
std::vector<T> RBTree<T>::pathFromRoot(const T& element) {
  std::vector<T> result = {};
  if (!static_cast<bool>(RBTree<T>::find(element))){
    return result;
  }
  if (root->element == element){
    result.push_back(root->element);
    return result;
  }
  Node* tmpNode = nullptr;
  tmpNode = root;
  while ((tmpNode->rightChild != nilNode) || (tmpNode->leftChild != nilNode)){
    if (element == tmpNode ->element){
      tmpNode = NULL;
      return result;
    }
    if (element < tmpNode->element){
      result.push_back(tmpNode->element);
      tmpNode = tmpNode->leftChild;
    }
    if (element > tmpNode->element){
      result.push_back(tmpNode->element);
      tmpNode = tmpNode->rightChild;
    }
    if (element == tmpNode->element){
      tmpNode = NULL;
      return result;
    }
  }
  tmpNode = NULL;
  return result;
}

template <typename T>
std::string RBTree<T>::ToGraphviz()  // Member function of the AVLTree class
{
  std::string toReturn = std::string("digraph {\n");
  if (root != nullptr &&
      root != nilNode)  // root is a pointer to the root node of the tree
  {
    std::string nodes;
    std::string connections = "\t\"Root\" -> 0;\n";
    GzAddNode(nodes, connections, root, 0);
    toReturn += nodes;
    toReturn += connections;
  }
  toReturn += "}";
  return toReturn;
}

template <typename T>
int RBTree<T>::GzAddNode(std::string& nodes, std::string& connections,
                         const Node* curr, size_t to) {
  size_t from = to;
  nodes += GzNode(from, curr->element, "filled",
                  curr->colour == Colour::RED ? "tomato" : "black",
                  curr->colour == Colour::RED ? "black" : "white");

  to = GzAddChild(nodes, connections, curr->leftChild, from, ++to, "blue");
  to = GzAddChild(nodes, connections, curr->rightChild, from, ++to, "gold");
  return to;
}

template <typename T>
int RBTree<T>::GzAddChild(std::string& nodes, std::string& connections,
                          const Node* child, size_t from, size_t to,
                          const std::string& color) {
  if (child != nilNode) {
    connections += GzConnection(from, to, color, "");
    to = GzAddNode(nodes, connections, child, to);
  } else if (child == nullptr) {
    nodes += GzNode(to, "null", "filled", "orange", "black");
    connections += GzConnection(from, to, "", "");
  } else {
    nodes += GzNode(to, child->element, "invis", "", "");
    connections += GzConnection(from, to, "", "invis");
  }
  return to;
}

template <typename T>
template <typename V>
std::string RBTree<T>::GzNode(size_t to, const V& what,
                              const std::string& style,
                              const std::string& fillColor,
                              const std::string& fontColor) {
  return fmt::format(
      "\t{} [label=\"{}\", fillcolor=\"{}\", fontcolor=\"{}\", style=\"{}\"]\n",
      to, what, fillColor, fontColor, style);
}

template <typename T>
std::string RBTree<T>::GzConnection(size_t from, size_t to,
                                    const std::string& color,
                                    const std::string& style) {
  return fmt::format("\t{} -> {} [color=\"{}\" style=\"{}\"]\n", from, to,
                     color, style);
}
#endif
