#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "lib\n_tree.h"
#include "lib\util.h"

char* appendNumberToString(const char* source, int length, unsigned int number) {
    static char buffer[128];
    int index = 0;

    // source 복사
    for (index = 0; index < length && source[index] != '\0'; index++) {
        buffer[index] = source[index];
    }

    // number를 문자열로 변환 (뒤에서부터 저장)
    char numbuf[32];
    int nidx = 0;
    if (number == 0) {
        numbuf[nidx++] = '0';
    } else {
        while (number > 0) {
            numbuf[nidx++] = '0' + (number % 10);
            number /= 10;
        }
    }

    // 뒤집어서 buffer에 이어붙이기
    for (int i = nidx - 1; i >= 0; i--) {
        buffer[index++] = numbuf[i];
    }

    buffer[index] = '\0';
    return buffer;
}

void InputRecursive (struct N_TreeType* tree, struct TreeNodeType* current, int childSize, int limit, int depth) {

  if (depth == limit) return;

  for (int index = 0; index < childSize; index++) {

    Insert_ChildNode_N_Tree (tree, current, "TEST1", index);
  }

  for (int index = 0; index < childSize; index++) {

    InputRecursive (tree, current->Children[index], childSize, limit, depth + 1);
  }
  return;
}

int main (int argc, char* argv[]) {

  SetConsoleOutputCP(CP_UTF8);

  int childSize = 3;
  struct N_TreeType* tree = Create_N_Tree (childSize);
  struct TreeNodeType* current = tree->Head;

  // Head 노드에 데이터 설정
  strncpy(current->Data, "Root", SIZE - 1);

  InputRecursive (tree, current, childSize, 3, 0);

  printf ("Number Of : %d\n", tree->Count);

  current = current->Children[1]->Children[1]->Children[1];
  Remove_N_Tree(tree, current, false);

  TreeView (tree, 5);

  printf ("Number Of : %d\n", tree->Count);

  current = tree->Head;
  current = current->Children[1]->Children[2];
  Remove_N_Tree(tree, current, true);

  TreeView (tree, 5);

  printf ("Number Of : %d\n", tree->Count);

  current = tree->Head;
  current = current->Children[2];
  Insert_ParentNode_N_Tree (tree, current, "DAT11");

  TreeView (tree, 11);

  printf ("Number Of : %d\n", tree->Count);
  return 0;
}