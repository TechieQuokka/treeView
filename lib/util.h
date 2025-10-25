#ifndef _UTIL_H_
#define _UTIL_H_

#include "n_tree.h"

#define MAX_WIDTH 396
#define MAX_NODES 256

// 노드 위치 정보 구조체
typedef struct {
  struct TreeNodeType* node;
  int x;  // 가로 위치
  int level;  // 레벨 (세로 위치)
  int childIndex;  // 부모의 몇 번째 자식인지 (-1이면 루트)
} NodePosition;

void MoveChildNode (struct TreeNodeType* parent, int from, int to);
void SwapChildNode (struct TreeNodeType* parent, int from, int to);
void Display (struct N_TreeType* tree);
void Show (struct N_TreeType* tree);
void TreeView (struct N_TreeType* tree, int nodeWidth);
void Edit (struct TreeNodeType* current, char* data);
struct TreeNodeType* GetParentNode (struct TreeNodeType* current);

#endif