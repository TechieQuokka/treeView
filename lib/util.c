#include "util.h"
#include <stdio.h>
#include <string.h>

void DisplayRecursive (struct TreeNodeType* node, int childSize, int depth) {

  for (int loop = 0; loop < depth; loop++) printf ("  ");
  
  if (node == NULL) {

    printf ("NULL\n");
    return;
  }

  printf ("%s\n", node->Data);

  for (int index = 0; index < childSize; index++) {

    DisplayRecursive (node->Children[index], childSize, depth + 1);
  }
  return;
}

void Display (struct N_TreeType* tree) {

  if (tree == NULL || tree->Head == NULL) return;

  printf ("[H]");
  DisplayRecursive (tree->Head, tree->ChildSize, 0);
  return;
}

static void ShowRecursive (struct TreeNodeType* node, int childSize, char* prefix, int isLast) {

  if (node == NULL) return;

  printf ("%s%s%s\n", prefix, isLast ? "└── " : "├── ", node->Data);

  char newPrefix[1024];
  snprintf (newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "│   ");

  for (int index = 0; index < childSize; index++) {
    int isLastChild = (index == childSize - 1);
    ShowRecursive (node->Children[index], childSize, newPrefix, isLastChild);
  }
}

void Show (struct N_TreeType* tree) {

  if (tree == NULL || tree->Head == NULL) return;

  printf ("%s\n", tree->Head->Data);

  for (int index = 0; index < tree->ChildSize; index++) {
    int isLast = (index == tree->ChildSize - 1);
    ShowRecursive (tree->Head->Children[index], tree->ChildSize, "", isLast);
  }
}

// 트리의 깊이를 구하는 함수
static int GetTreeDepth (struct TreeNodeType* node, int childSize) {
  if (node == NULL) return 0;

  int maxDepth = 0;
  for (int i = 0; i < childSize; i++) {
    if (node->Children[i] != NULL) {
      int childDepth = GetTreeDepth (node->Children[i], childSize);
      if (childDepth > maxDepth) maxDepth = childDepth;
    }
  }
  return maxDepth + 1;
}

// 리프 노드 개수를 세는 함수
static int CountLeaves (struct TreeNodeType* node, int childSize) {
  if (node == NULL) return 0;

  int hasChild = 0;
  for (int i = 0; i < childSize; i++) {
    if (node->Children[i] != NULL) hasChild = 1;
  }

  if (!hasChild) return 1;

  int count = 0;
  for (int i = 0; i < childSize; i++) {
    count += CountLeaves (node->Children[i], childSize);
  }
  return count;
}

// 노드의 가로 위치를 계산하고 저장하는 함수
static int AssignPositions (struct TreeNodeType* node, int childSize, int level, int leftPos, NodePosition* positions, int* posCount, int nodeWidth, int childIdx) {
  if (node == NULL) return leftPos;

  int hasChild = 0;
  for (int i = 0; i < childSize; i++) {
    if (node->Children[i] != NULL) hasChild = 1;
  }

  if (!hasChild) {
    // 리프 노드
    positions[*posCount].node = node;
    positions[*posCount].x = leftPos;
    positions[*posCount].level = level;
    positions[*posCount].childIndex = childIdx;
    (*posCount)++;
    return leftPos + nodeWidth + 2;  // 노드 간격 (nodeWidth + 여백)
  }

  // 내부 노드: 자식들의 위치를 먼저 계산
  int startPos = leftPos;
  int endPos = leftPos;

  for (int i = 0; i < childSize; i++) {
    if (node->Children[i] != NULL) {
      endPos = AssignPositions (node->Children[i], childSize, level + 1, endPos, positions, posCount, nodeWidth, i);
    }
  }

  // 부모는 자식들의 중간에 배치
  int parentX = (startPos + endPos - nodeWidth - 2) / 2;

  positions[*posCount].node = node;
  positions[*posCount].x = parentX;
  positions[*posCount].level = level;
  positions[*posCount].childIndex = childIdx;
  (*posCount)++;

  return endPos;
}

void TreeView (struct N_TreeType* tree, int nodeWidth) {
  if (tree == NULL || tree->Head == NULL) return;
  if (nodeWidth < 1) nodeWidth = 5;  // 최소값 설정

  printf ("[Horizontal Tree View]\n\n");

  NodePosition positions[MAX_NODES];
  int posCount = 0;

  // 노드들의 위치 계산
  AssignPositions (tree->Head, tree->ChildSize, 0, 0, positions, &posCount, nodeWidth, -1);

  int depth = GetTreeDepth (tree->Head, tree->ChildSize);

  // 최대 X 좌표 계산
  int maxX = 0;
  for (int i = 0; i < posCount; i++) {
    int x = positions[i].x + nodeWidth;
    if (x > maxX) maxX = x;
  }

  // 레벨별로 출력
  for (int level = 0; level < depth; level++) {
    char nodeLine[MAX_WIDTH * 4];
    char indexLine[MAX_WIDTH * 4];
    char vertLine[MAX_WIDTH * 4];
    char horzLine[MAX_WIDTH * 4];

    memset (nodeLine, ' ', sizeof(nodeLine));
    memset (indexLine, ' ', sizeof(indexLine));
    memset (vertLine, ' ', sizeof(vertLine));
    memset (horzLine, ' ', sizeof(horzLine));

    // 현재 레벨의 노드들을 버퍼에 추가
    for (int i = 0; i < posCount; i++) {
      if (positions[i].level == level) {
        int x = positions[i].x;
        const char* data = positions[i].node->Data;
        int dataLen = strlen (data);

        // 고정 너비로 노드 데이터 출력 (중앙 정렬)
        int leftPadding = (nodeWidth - dataLen) / 2;
        int rightPadding = nodeWidth - dataLen - leftPadding;

        for (int j = 0; j < nodeWidth; j++) {
          if (j < leftPadding) {
            nodeLine[x + j] = ' ';  // 왼쪽 패딩
          } else if (j < leftPadding + dataLen) {
            nodeLine[x + j] = data[j - leftPadding];  // 텍스트
          } else {
            nodeLine[x + j] = ' ';  // 오른쪽 패딩
          }
        }

        // 인덱스 출력 (루트가 아닌 경우)
        if (positions[i].childIndex >= 0) {
          char indexStr[32];
          snprintf (indexStr, sizeof(indexStr), "(%d)", positions[i].childIndex);
          int indexLen = strlen (indexStr);
          // 중앙 정렬: 노드의 정중앙에서 인덱스 길이의 절반만큼 왼쪽으로
          int indexX = x + (nodeWidth / 2) - (indexLen / 2);

          for (int j = 0; j < indexLen && indexX + j < MAX_WIDTH * 4; j++) {
            indexLine[indexX + j] = indexStr[j];
          }
        }
      }
    }

    // 노드 라인 출력
    for (int i = 0; i <= maxX && i < MAX_WIDTH; i++) {
      printf ("%c", nodeLine[i]);
    }
    printf ("\n");

    // 인덱스 라인 출력 (루트 레벨이 아닌 경우)
    if (level > 0) {
      for (int i = 0; i <= maxX && i < MAX_WIDTH; i++) {
        printf ("%c", indexLine[i]);
      }
      printf ("\n");
    }

    // 연결선 출력 (마지막 레벨 제외)
    if (level < depth - 1) {
      // 수직선과 가로선을 버퍼에 그리기
      for (int i = 0; i < posCount; i++) {
        if (positions[i].level == level) {
          struct TreeNodeType* parent = positions[i].node;
          int parentX = positions[i].x + nodeWidth / 2;

          int firstChildX = -1, lastChildX = -1;
          int childPositions[MAX_NODES];
          int childCount = 0;

          // 자식 노드들의 위치 찾기
          for (int c = 0; c < tree->ChildSize; c++) {
            if (parent->Children[c] != NULL) {
              for (int p = 0; p < posCount; p++) {
                if (positions[p].node == parent->Children[c]) {
                  int childX = positions[p].x + nodeWidth / 2;
                  childPositions[childCount++] = childX;
                  if (firstChildX == -1) firstChildX = childX;
                  lastChildX = childX;
                }
              }
            }
          }

          if (firstChildX != -1) {
            // 부모에서 아래로 수직선
            vertLine[parentX] = '|';

            // 가로선 그리기
            if (firstChildX != lastChildX) {
              for (int x = firstChildX; x <= lastChildX; x++) {
                horzLine[x] = '-';
              }
              // 자식 위치에 + 표시
              for (int c = 0; c < childCount; c++) {
                horzLine[childPositions[c]] = '+';
              }
            } else {
              horzLine[firstChildX] = '|';
            }
          }
        }
      }

      // 수직선 출력
      for (int i = 0; i <= maxX && i < MAX_WIDTH; i++) {
        printf ("%c", vertLine[i]);
      }
      printf ("\n");

      // 가로선 출력
      for (int i = 0; i <= maxX && i < MAX_WIDTH; i++) {
        printf ("%c", horzLine[i]);
      }
      printf ("\n");
    }
  }
}

struct TreeNodeType* GetParentNode (struct TreeNodeType* current) {

  if (current == NULL) return NULL;

  return current->Parent;
}

void MoveChildNode (struct TreeNodeType* parent, int from, int to) {

  if (parent == NULL) return;
  if (parent->Children == NULL) return;

  // 범위 체크는 호출하는 쪽에서 해야 함
  struct TreeNodeType* temp = parent->Children[from];

  // from에서 to로 이동 (사이의 노드들을 shift)
  if (from < to) {
    // 왼쪽에서 오른쪽으로 이동
    for (int i = from; i < to; i++) {
      parent->Children[i] = parent->Children[i + 1];
    }
  } else if (from > to) {
    // 오른쪽에서 왼쪽으로 이동
    for (int i = from; i > to; i--) {
      parent->Children[i] = parent->Children[i - 1];
    }
  } else {
    // from == to, 이동 필요 없음
    return;
  }

  parent->Children[to] = temp;
}

void SwapChildNode (struct TreeNodeType* parent, int from, int to) {

  if (parent == NULL) return;
  if (parent->Children == NULL) return;

  // 범위 체크는 호출하는 쪽에서 해야 함
  struct TreeNodeType* temp = parent->Children[from];
  parent->Children[from] = parent->Children[to];
  parent->Children[to] = temp;
}

void Edit (struct TreeNodeType* current, char* data) {

  if (current == NULL) return;
  if (data == NULL) return;

  strncpy (current->Data, data, SIZE - 1);
  current->Data[SIZE - 1] = '\0';
}