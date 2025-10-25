#include "n_tree.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CHILD_SIZE 128

struct ChildPositionsType {

  int positions[CHILD_SIZE];
  int size;
};

int ClearRecursive (struct TreeNodeType* node, int childSize);

struct N_TreeType* Create_N_Tree (int number_of_children) {

  if (number_of_children < 1 || number_of_children > CHILD_SIZE) {

    printf("Error! Invalid number_of_children: %d (must be between 1 and %d). Create_N_Tree()\n",
               number_of_children, CHILD_SIZE);
    return NULL;
  }

  struct N_TreeType* tree = (struct N_TreeType*) malloc (1 * sizeof (struct N_TreeType));
  if (tree == NULL) {

    printf ("Error! The dynamic memory allocation failed. Create_N_Tree().\n");
    return NULL;
  }

  tree->Head = (struct TreeNodeType*) calloc (1, sizeof (struct TreeNodeType));
  if (tree->Head == NULL) {

    printf ("Error! The dynamic memory allocation failed(2). Create_N_Tree().\n");
    free (tree);
    return NULL;
  }

  tree->Head->Children = (struct TreeNodeType**) calloc (number_of_children, sizeof (struct TreeNodeType*));
  if (tree->Head->Children == NULL) {

    printf ("Error! The dynamic memory allocation failed(3). Create_N_Tree().\n");
    free (tree->Head);
    free (tree);
    return NULL;
  }

  tree->ChildSize = number_of_children;
  tree->Count = 0;
  return tree;
}

struct TreeNodeType* Access_N_Tree (struct N_TreeType* tree, struct TreeNodeType* parent, int position) {

  if (tree == NULL || parent == NULL) return NULL;
  if (position < 0 || position >= tree->ChildSize) {

    printf("Error! position %d is out of range (0 ~ %d). Access_N_Tree()\n",
      position, tree->ChildSize - 1);
    return NULL;
  }

  return parent->Children[position];
}

struct TreeNodeType* MakeTreeNode (struct TreeNodeType* parent, char* data, int childSize) {

  struct TreeNodeType* newNode = (struct TreeNodeType*) calloc (1, sizeof (struct TreeNodeType));
  if (newNode == NULL) {

    printf ("Error! The dynamic memory allocation failed. MakeTreeNode()\n");
    return NULL;
  }

  newNode->Children = (struct TreeNodeType**) calloc (childSize, sizeof (struct TreeNodeType*));
  if (newNode->Children == NULL) {
 
    printf ("Error! The dynamic memory allocation failed. MakeTreeNode()\n");
    free (newNode);
    return NULL;
  }

  strncpy(newNode->Data, data, SIZE - 1);
  newNode->Data[SIZE - 1] = '\0';
  newNode->Parent = parent;

  return newNode;
}

void Insert_ChildNode_N_Tree (struct N_TreeType* tree, struct TreeNodeType* parent, char* data, int position) {

  if (tree == NULL || tree->Head == NULL) {

    printf("Error! N_TreeType pointer is NULL. Insert_ChildNode_N_Tree()\n");
    return;
  }
  if (parent == NULL) {

    printf("Error! Parent node is NULL. Insert_ChildNode_N_Tree()\n");
    return;
  }
  if (position < 0 || position >= tree->ChildSize) {

    printf("Error! position %d is out of range (0 ~ %d). Insert_ChildNode_N_Tree()\n",
      position, tree->ChildSize - 1);
    return;
  }

  struct TreeNodeType* newNode = MakeTreeNode(parent, data, tree->ChildSize);
  if (newNode == NULL) return;

  if (parent->Children[position] == NULL) {

    parent->Children[position] = newNode;
    tree->Count++;
    return;
  }

  struct TreeNodeType* childNode = parent->Children[position];
  childNode->Parent = newNode;
  newNode->Children[0] = childNode;
  newNode->Parent = parent;
  parent->Children[position] = newNode;
  tree->Count++;
  return;
}

int GetChildPosition (struct TreeNodeType* child, int childSize) {

  struct TreeNodeType* parent = child->Parent;
  for (int index = 0; index < childSize; index++) {

    if (parent->Children[index] == child) return index;
  }

  return -1;
}

void Insert_ParentNode_N_Tree (struct N_TreeType* tree, struct TreeNodeType* child, char* data) {

  if (tree == NULL || tree->Head == NULL) {

    printf("Error! N_TreeType pointer is NULL. Insert_ParentNode_N_Tree()\n");
    return;
  }
  if (child == NULL) {

    printf("Error! Child node is NULL. Insert_ParentNode_N_Tree()\n");
    return;
  }
  if (tree->Head == child) {

    printf("Error! Cannot insert a parent above the head node. Insert_ParentNode_N_Tree()\n");
    return;
  }

  struct TreeNodeType* newNode = MakeTreeNode (child->Parent, data, tree->ChildSize);
  if (newNode == NULL) return;

  int position = GetChildPosition(child, tree->ChildSize);
  if (position == -1) {

    ClearRecursive(newNode, tree->ChildSize);
    return;
  }

  struct TreeNodeType* parentNode = child->Parent;
  child->Parent = newNode;
  newNode->Children[0] = child;
  newNode->Parent = parentNode;
  parentNode->Children[position] = newNode;

  tree->Count++;
  return;
}

struct ChildPositionsType GetChildIndices (struct TreeNodeType* child, int childSize) {

  struct ChildPositionsType result = { 0, };
  int location = 0;
  for (int index = 0; index < childSize; index++) {
    if (child->Children[index] == NULL) continue;

    result.positions[location++] = index;
  }
  result.size = location;

  return result;
}

bool Remove_N_Tree (struct N_TreeType* tree, struct TreeNodeType* target, bool clearAll) {

  if (tree == NULL || tree->Head == NULL) {

    printf("Error! N_TreeType pointer is NULL. Remove_N_Tree()\n");
    return false;
  }
  if (target == NULL) {

    printf("Error! Target node is NULL. Remove_N_Tree()\n");
    return false;
  }

  struct TreeNodeType* parentNode = target->Parent;
  int childSize = tree->ChildSize;
  int targetPosition = GetChildPosition(target, childSize);

  if (clearAll) {

    parentNode->Children[targetPosition] = NULL;
    Clear_N_Tree (tree, target);
    return true;
  }

  struct ChildPositionsType childPositions = GetChildIndices (target, childSize);

  if (childPositions.size > 1) return false;
  if (childPositions.size == 0) {

    parentNode->Children[targetPosition] = NULL;

    free (target->Children);
    free (target);

    tree->Count--;
    return true;
  }
  // if target child is 1.
  struct TreeNodeType* childNode = target->Children[childPositions.positions[0]];
  childNode->Parent = parentNode;
  parentNode->Children[targetPosition] = childNode;

  free (target->Children);
  free (target);

  tree->Count--;
  return true;
}

int ClearRecursive (struct TreeNodeType* node, int childSize) {

  if (node == NULL) return 0;

  int count = 1;
  for (int index = 0; index < childSize; index++) {

    count += ClearRecursive (node->Children[index], childSize);
  }
  free (node->Children);
  free (node);
  return count;
}

void Clear_N_Tree (struct N_TreeType* tree, struct TreeNodeType* node) {

  if (tree == NULL || node == NULL) return;
  int count = ClearRecursive (node, tree->ChildSize);
  tree->Count -= count;
  return;
}

void Delete_N_Tree (struct N_TreeType* tree) {

  if (tree == NULL) return;

  ClearRecursive (tree->Head, tree->ChildSize);
  free (tree);
  return;
}
