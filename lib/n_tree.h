#ifndef _N_TREE_H_
#define _N_TREE_H_

#include <stdbool.h>

#define CHILD_SIZE 128
#define SIZE 32

struct TreeNodeType {

  char Data[SIZE];
  struct TreeNodeType* Parent;
  struct TreeNodeType** Children;
};

struct N_TreeType {

  struct TreeNodeType* Head;
  int Count;
  int ChildSize;
};

struct N_TreeType* Create_N_Tree (int number_of_children);
struct TreeNodeType* Access_N_Tree (struct N_TreeType* tree, struct TreeNodeType* parent, int position);
void Insert_ChildNode_N_Tree (struct N_TreeType* tree, struct TreeNodeType* parent, char* data, int position);
void Insert_ParentNode_N_Tree (struct N_TreeType* tree, struct TreeNodeType* child, char* data);
bool Remove_N_Tree (struct N_TreeType* tree, struct TreeNodeType* target, bool clearAll);
void Clear_N_Tree (struct N_TreeType* tree, struct TreeNodeType* node);
void Delete_N_Tree (struct N_TreeType* tree);

#endif