#include "workflow_cli.h"
#include "../lib/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WorkflowCLI* InitWorkflowCLI(int childSize) {
  WorkflowCLI* cli = (WorkflowCLI*)malloc(sizeof(WorkflowCLI));
  if (cli == NULL) {
    printf("Error! Failed to allocate CLI structure.\n");
    return NULL;
  }

  // JSON 파일 경로 설정
  strncpy(cli->jsonFilePath, JSON_FILE_PATH, sizeof(cli->jsonFilePath) - 1);
  cli->jsonFilePath[sizeof(cli->jsonFilePath) - 1] = '\0';

  // JSON 파일에서 불러오기 시도
  cli->tree = NULL;
  bool loaded = LoadTreeFromJSON(cli);

  if (!loaded) {
    // JSON 파일이 없으면 새 트리 생성
    cli->tree = Create_N_Tree(childSize);
    if (cli->tree == NULL) {
      free(cli);
      return NULL;
    }

    // 루트 노드를 "Root"로 초기화
    strncpy(cli->tree->Head->Data, "Root", SIZE - 1);
    cli->tree->Head->Data[SIZE - 1] = '\0';
    cli->tree->Count = 1;

    cli->current = cli->tree->Head;
    cli->pathDepth = 0;
    UpdatePath(cli);

    // 초기 상태 저장
    SaveTreeToJSON(cli);
    printf("Created new workflow tree.\n");
  }

  return cli;
}

void CleanupWorkflowCLI(WorkflowCLI* cli) {
  if (cli == NULL) return;

  if (cli->tree != NULL) {
    Delete_N_Tree(cli->tree);
  }
  free(cli);
}

void UpdatePath(WorkflowCLI* cli) {
  if (cli == NULL || cli->current == NULL) return;

  // 경로를 거슬러 올라가며 구성
  struct TreeNodeType* node = cli->current;
  int depth = 0;

  // 임시 배열에 역순으로 저장
  char tempPath[MAX_PATH_DEPTH][SIZE];

  // 무한 루프 방지: 이미 방문한 노드를 추적
  struct TreeNodeType* visited[MAX_PATH_DEPTH] = {NULL};

  while (node != NULL && depth < MAX_PATH_DEPTH) {
    // 순환 참조 검사
    for (int i = 0; i < depth; i++) {
      if (visited[i] == node) {
        printf("Warning! Circular reference detected in tree path.\n");
        cli->pathDepth = depth;
        for (int j = 0; j < depth; j++) {
          strncpy(cli->path[j], tempPath[depth - 1 - j], SIZE);
        }
        return;
      }
    }

    visited[depth] = node;
    strncpy(tempPath[depth], node->Data, SIZE - 1);
    tempPath[depth][SIZE - 1] = '\0';
    depth++;
    node = node->Parent;
  }

  // 정순으로 복사
  cli->pathDepth = depth;
  for (int i = 0; i < depth; i++) {
    strncpy(cli->path[i], tempPath[depth - 1 - i], SIZE);
  }
}

void ShowCurrentPath(WorkflowCLI* cli) {
  printf("\nCurrent Path: ");
  if (cli->pathDepth == 0) {
    printf("/\n");
    return;
  }

  for (int i = 0; i < cli->pathDepth; i++) {
    printf("/%s", cli->path[i]);
  }
  printf("\n");
}

void ClearScreen(void) {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void ShowHelp(void) {
  printf("\n========== Workflow Tree CLI ==========\n");
  printf("Commands:\n");
  printf("  help              - Show this help message\n");
  printf("  clear / cls       - Clear the screen\n");
  printf("  pwd               - Show current path\n");
  printf("  ls                - List children of current node\n");
  printf("  tree              - Show full tree structure\n");
  printf("  view              - Show horizontal tree view\n");
  printf("  cd <index>        - Navigate to child at position\n");
  printf("  cd ..             - Navigate to parent node\n");
  printf("  cd /              - Navigate to root node\n");
  printf("  insert <pos> <data> - Insert child node at position\n");
  printf("  insertp <data>    - Insert parent above current node\n");
  printf("  remove            - Remove current node (keep one child)\n");
  printf("  remove all        - Remove current node and all children\n");
  printf("  edit <data>       - Edit current node's data\n");
  printf("  info              - Show current node information\n");
  printf("  exit / quit       - Exit the CLI\n");
  printf("=======================================\n\n");
}

void ShowCurrentNode(WorkflowCLI* cli) {
  printf("\nCurrent Node: %s\n", cli->current->Data);
  printf("Total Nodes in Tree: %d\n", cli->tree->Count);

  int childCount = 0;
  for (int i = 0; i < cli->tree->ChildSize; i++) {
    if (cli->current->Children[i] != NULL) {
      childCount++;
    }
  }
  printf("Children Count: %d\n", childCount);
}

static int FindMaxDataLength(struct TreeNodeType* node, int childSize) {
  if (node == NULL) return 0;

  int maxLen = strlen(node->Data);

  for (int i = 0; i < childSize; i++) {
    if (node->Children[i] != NULL) {
      int childMax = FindMaxDataLength(node->Children[i], childSize);
      if (childMax > maxLen) {
        maxLen = childMax;
      }
    }
  }

  return maxLen;
}

void ShowFullTree(WorkflowCLI* cli) {
  printf("\n");
  Show(cli->tree);
  printf("\n");
}

void ListChildren(WorkflowCLI* cli) {
  printf("\nChildren of '%s':\n", cli->current->Data);

  int hasChildren = 0;
  for (int i = 0; i < cli->tree->ChildSize; i++) {
    if (cli->current->Children[i] != NULL) {
      printf("  [%d] %s\n", i, cli->current->Children[i]->Data);
      hasChildren = 1;
    }
  }

  if (!hasChildren) {
    printf("  (no children)\n");
  }
  printf("\n");
}

int GetChildIndex(struct TreeNodeType* parent, struct TreeNodeType* child, int childSize) {
  if (parent == NULL || child == NULL) return -1;

  for (int i = 0; i < childSize; i++) {
    if (parent->Children[i] == child) {
      return i;
    }
  }
  return -1;
}

void ChangeDirectory(WorkflowCLI* cli, const char* target) {
  if (strcmp(target, "..") == 0) {
    // 부모로 이동
    if (cli->current->Parent != NULL) {
      cli->current = cli->current->Parent;
      UpdatePath(cli);
      printf("Moved to parent: %s\n", cli->current->Data);
    } else {
      printf("Already at root node.\n");
    }
  } else if (strcmp(target, "/") == 0) {
    // 루트로 이동
    cli->current = cli->tree->Head;
    UpdatePath(cli);
    printf("Moved to root.\n");
  } else {
    // 자식 인덱스로 이동
    int index = atoi(target);
    struct TreeNodeType* child = Access_N_Tree(cli->tree, cli->current, index);

    if (child != NULL) {
      cli->current = child;
      UpdatePath(cli);
      printf("Moved to: %s\n", cli->current->Data);
    } else {
      printf("No child at position %d\n", index);
    }
  }
}

void InsertNode(WorkflowCLI* cli, const char* data, int position) {
  if (position < 0 || position >= cli->tree->ChildSize) {
    printf("Error! Position %d exceeds array bounds (0-%d)\n", position, cli->tree->ChildSize - 1);
    return;
  }

  // 해당 위치가 비어있는지 확인하고, 비어있으면 삽입
  if (cli->current->Children[position] != NULL) {
    printf("Warning! Position %d is already occupied. Skipping insertion.\n", position);
    return;
  }

  Insert_ChildNode_N_Tree(cli->tree, cli->current, (char*)data, position);
  printf("Inserted '%s' at position %d under '%s'\n", data, position, cli->current->Data);
  AutoSave(cli);
}

void InsertParent(WorkflowCLI* cli, const char* data) {
  if (cli->current == cli->tree->Head) {
    printf("Error! Cannot insert parent above root node.\n");
    return;
  }

  Insert_ParentNode_N_Tree(cli->tree, cli->current, (char*)data);
  printf("Inserted parent '%s' above '%s'\n", data, cli->current->Data);

  // 현재 위치를 새로 삽입된 부모로 이동
  cli->current = cli->current->Parent;
  UpdatePath(cli);
  AutoSave(cli);
}

void RemoveNode(WorkflowCLI* cli, bool clearAll) {
  if (cli->current == cli->tree->Head) {
    if (clearAll) {
      // Remove all children of the root node
      int removedCount = 0;
      for (int i = 0; i < cli->tree->ChildSize; i++) {
        if (cli->current->Children[i] != NULL) {
          Clear_N_Tree(cli->tree, cli->current->Children[i]);
          cli->current->Children[i] = NULL;
          removedCount++;
        }
      }
      if (removedCount > 0) {
        printf("Removed all %d children of root node.\n", removedCount);
        UpdatePath(cli);
        AutoSave(cli);
      } else {
        printf("Root node has no children to remove.\n");
      }
    } else {
      printf("Error! Cannot remove root node.\n");
      printf("Use 'remove all' to remove all children of root.\n");
    }
    return;
  }

  char nodeName[SIZE];
  strncpy(nodeName, cli->current->Data, SIZE);

  struct TreeNodeType* parent = cli->current->Parent;
  bool result = Remove_N_Tree(cli->tree, cli->current, clearAll);

  if (result) {
    printf("Removed node '%s'%s\n", nodeName, clearAll ? " (with all children)" : "");
    cli->current = parent;
    UpdatePath(cli);
    AutoSave(cli);
  } else {
    if (!clearAll) {
      printf("Error! Cannot remove node with multiple children.\n");
      printf("Use 'remove all' to remove node and all its children.\n");
    } else {
      printf("Error! Failed to remove node.\n");
    }
  }
}

void EditCurrentNode(WorkflowCLI* cli, const char* newData) {
  strncpy(cli->current->Data, newData, SIZE - 1);
  cli->current->Data[SIZE - 1] = '\0';
  UpdatePath(cli);
  printf("Node data updated to: %s\n", newData);
  AutoSave(cli);
}

// JSON 저장 관련 함수들
static void WriteEscapedString(FILE* fp, const char* str) {
  // 큰따옴표를 이스케이프 처리하여 출력
  while (*str) {
    if (*str == '"') {
      fprintf(fp, "\\\"");
    } else if (*str == '\\') {
      fprintf(fp, "\\\\");
    } else {
      fputc(*str, fp);
    }
    str++;
  }
}

static void WriteNodeToJSON(FILE* fp, struct TreeNodeType* node, int childSize, int depth) {
  if (node == NULL) {
    fprintf(fp, "null");
    return;
  }

  // 들여쓰기
  char indent[256] = "";
  for (int i = 0; i < depth; i++) {
    strcat(indent, "  ");
  }

  fprintf(fp, "{\n");
  fprintf(fp, "%s  \"data\": \"", indent);
  WriteEscapedString(fp, node->Data);
  fprintf(fp, "\"");

  // 자식 노드들 확인
  int hasChildren = 0;
  for (int i = 0; i < childSize; i++) {
    if (node->Children[i] != NULL) {
      hasChildren = 1;
      break;
    }
  }

  if (hasChildren) {
    fprintf(fp, ",\n%s  \"children\": [\n", indent);
    int firstChild = 1;
    for (int i = 0; i < childSize; i++) {
      if (!firstChild) {
        fprintf(fp, ",\n");
      }
      fprintf(fp, "%s    ", indent);
      WriteNodeToJSON(fp, node->Children[i], childSize, depth + 2);
      if (node->Children[i] != NULL) {
        firstChild = 0;
      }
    }
    fprintf(fp, "\n%s  ]", indent);
  }

  fprintf(fp, "\n%s}", indent);
}

void SaveTreeToJSON(WorkflowCLI* cli) {
  if (cli == NULL || cli->tree == NULL) {
    printf("Error! Invalid CLI or tree.\n");
    return;
  }

  FILE* fp = fopen(cli->jsonFilePath, "w");
  if (fp == NULL) {
    printf("Error! Failed to open file for writing: %s\n", cli->jsonFilePath);
    return;
  }

  fprintf(fp, "{\n");
  fprintf(fp, "  \"childSize\": %d,\n", cli->tree->ChildSize);
  fprintf(fp, "  \"count\": %d,\n", cli->tree->Count);
  fprintf(fp, "  \"tree\": ");
  WriteNodeToJSON(fp, cli->tree->Head, cli->tree->ChildSize, 1);
  fprintf(fp, "\n}\n");

  fclose(fp);
}

void AutoSave(WorkflowCLI* cli) {
  SaveTreeToJSON(cli);
}

// JSON 불러오기 관련 함수들
static char* ReadFileContent(const char* filepath) {
  FILE* fp = fopen(filepath, "r");
  if (fp == NULL) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char* content = (char*)malloc(size + 1);
  if (content == NULL) {
    fclose(fp);
    return NULL;
  }

  fread(content, 1, size, fp);
  content[size] = '\0';
  fclose(fp);

  return content;
}

static char* SkipWhitespace(char* str) {
  while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')) {
    str++;
  }
  return str;
}

static char* ParseString(char* str, char* output, int maxLen) {
  str = SkipWhitespace(str);
  if (*str != '"') return NULL;
  str++;

  int i = 0;
  while (*str && *str != '"' && i < maxLen - 1) {
    // 이스케이프된 따옴표 처리
    if (*str == '\\' && *(str + 1) == '"') {
      output[i++] = '"';
      str += 2;
    } else {
      output[i++] = *str++;
    }
  }
  output[i] = '\0';

  if (*str == '"') str++;
  return str;
}

static char* ParseNumber(char* str, int* value) {
  str = SkipWhitespace(str);
  *value = 0;
  while (*str >= '0' && *str <= '9') {
    *value = (*value * 10) + (*str - '0');
    str++;
  }
  return str;
}

static char* ParseNode(char* str, struct TreeNodeType** node, int childSize, int depth);

static char* ParseChildren(char* str, struct TreeNodeType* parent, int childSize, int depth) {
  str = SkipWhitespace(str);
  if (*str != '[') return NULL;
  str++;

  // 최대 깊이 제한 (무한 재귀 방지)
  if (depth > MAX_PARSE_DEPTH) {
    printf("Error! Maximum parsing depth exceeded.\n");
    return NULL;
  }

  for (int i = 0; i < childSize; i++) {
    str = SkipWhitespace(str);

    // null 체크
    if (strncmp(str, "null", 4) == 0) {
      parent->Children[i] = NULL;
      str += 4;
    } else {
      str = ParseNode(str, &parent->Children[i], childSize, depth + 1);
      if (parent->Children[i] != NULL) {
        parent->Children[i]->Parent = parent;
      }
    }

    str = SkipWhitespace(str);
    if (*str == ',') str++;
  }

  str = SkipWhitespace(str);
  if (*str == ']') str++;

  return str;
}

static char* ParseNode(char* str, struct TreeNodeType** node, int childSize, int depth) {
  str = SkipWhitespace(str);

  if (strncmp(str, "null", 4) == 0) {
    *node = NULL;
    return str + 4;
  }

  if (*str != '{') return NULL;
  str++;

  // 최대 깊이 제한 (무한 재귀 방지)
  if (depth > MAX_PARSE_DEPTH) {
    printf("Error! Maximum parsing depth exceeded.\n");
    *node = NULL;
    return NULL;
  }

  *node = (struct TreeNodeType*)malloc(sizeof(struct TreeNodeType));
  if (*node == NULL) return NULL;

  (*node)->Parent = NULL;
  (*node)->Children = (struct TreeNodeType**)calloc(childSize, sizeof(struct TreeNodeType*));
  memset((*node)->Data, 0, SIZE);  // 데이터 초기화

  int iterations = 0;
  while (*str && *str != '}') {
    str = SkipWhitespace(str);

    if (*str == '"') {
      char key[64];
      str = ParseString(str, key, sizeof(key));
      if (str == NULL) break;  // 파싱 실패 시 중단

      str = SkipWhitespace(str);

      if (*str == ':') str++;
      str = SkipWhitespace(str);

      if (strcmp(key, "data") == 0) {
        str = ParseString(str, (*node)->Data, SIZE);
        if (str == NULL) break;  // 파싱 실패 시 중단
      } else if (strcmp(key, "children") == 0) {
        str = ParseChildren(str, *node, childSize, depth);
        if (str == NULL) break;  // 파싱 실패 시 중단
      }
    }

    str = SkipWhitespace(str);
    if (*str == ',') str++;
  }

  if (*str == '}') str++;
  return str;
}

static int CountNodes(struct TreeNodeType* node, int childSize) {
  if (node == NULL) return 0;

  int count = 1;
  for (int i = 0; i < childSize; i++) {
    count += CountNodes(node->Children[i], childSize);
  }
  return count;
}

bool LoadTreeFromJSON(WorkflowCLI* cli) {
  if (cli == NULL) return false;

  char* content = ReadFileContent(cli->jsonFilePath);
  if (content == NULL) {
    // JSON 파일이 없으면 빈 트리 생성
    printf("No existing workflow file found. Starting with empty tree.\n");
    return false;
  }

  char* str = content;
  str = SkipWhitespace(str);

  if (*str != '{') {
    free(content);
    return false;
  }
  str++;

  int childSize = 0;
  int count = 0;
  struct TreeNodeType* root = NULL;

  while (*str && *str != '}') {
    str = SkipWhitespace(str);

    if (*str == '"') {
      char key[64];
      str = ParseString(str, key, sizeof(key));
      str = SkipWhitespace(str);

      if (*str == ':') str++;
      str = SkipWhitespace(str);

      if (strcmp(key, "childSize") == 0) {
        str = ParseNumber(str, &childSize);
      } else if (strcmp(key, "count") == 0) {
        str = ParseNumber(str, &count);
      } else if (strcmp(key, "tree") == 0) {
        str = ParseNode(str, &root, childSize, 0);
      }
    }

    str = SkipWhitespace(str);
    if (*str == ',') str++;
  }

  free(content);

  if (root != NULL && childSize > 0) {
    // 기존 트리 삭제
    if (cli->tree != NULL) {
      Delete_N_Tree(cli->tree);
    }

    // 새 트리 설정
    cli->tree = (struct N_TreeType*)malloc(sizeof(struct N_TreeType));
    cli->tree->Head = root;
    cli->tree->ChildSize = childSize;
    cli->tree->Count = CountNodes(root, childSize);

    cli->current = root;
    UpdatePath(cli);

    printf("Loaded workflow tree from %s (%d nodes)\n", cli->jsonFilePath, cli->tree->Count);
    return true;
  }

  return false;
}

// 큰따옴표를 고려한 명령어 파싱 함수
static int ParseCommand(const char* command, char* cmd, char* arg1, char* arg2) {
  const char* p = command;
  int argCount = 0;
  int maxLen = MAX_COMMAND_LEN - 1;  // 버퍼 크기 제한

  // 공백 건너뛰기
  while (*p && (*p == ' ' || *p == '\t')) p++;

  // 1. 명령어 파싱
  int i = 0;
  while (*p && *p != ' ' && *p != '\t' && i < SIZE - 1) {
    cmd[i++] = *p++;
  }
  cmd[i] = '\0';
  if (i > 0) argCount++;

  // 공백 건너뛰기
  while (*p && (*p == ' ' || *p == '\t')) p++;
  if (!*p) return argCount;

  // 2. 첫 번째 인자 파싱
  i = 0;
  if (*p == '"') {
    // 큰따옴표로 시작하는 경우
    p++; // 시작 따옴표 건너뛰기
    while (*p && *p != '"' && i < maxLen) {
      arg1[i++] = *p++;
    }
    if (*p == '"') p++; // 종료 따옴표 건너뛰기
  } else {
    // 일반 토큰
    while (*p && *p != ' ' && *p != '\t' && i < maxLen) {
      arg1[i++] = *p++;
    }
  }
  arg1[i] = '\0';
  if (i > 0) argCount++;

  // 공백 건너뛰기
  while (*p && (*p == ' ' || *p == '\t')) p++;
  if (!*p) return argCount;

  // 3. 두 번째 인자 파싱
  i = 0;
  if (*p == '"') {
    // 큰따옴표로 시작하는 경우
    p++; // 시작 따옴표 건너뛰기
    while (*p && *p != '"' && i < maxLen) {
      arg2[i++] = *p++;
    }
    if (*p == '"') p++; // 종료 따옴표 건너뛰기
  } else {
    // 나머지 전부 (공백 포함)
    while (*p && i < maxLen) {
      arg2[i++] = *p++;
    }
  }
  arg2[i] = '\0';
  if (i > 0) argCount++;

  return argCount;
}

// 문자열 길이 검증 함수
static bool ValidateStringLength(const char* str, const char* fieldName) {
  size_t len = strlen(str);
  if (len >= SIZE) {
    printf("Error! %s is too long (max %d characters, got %zu).\n",
           fieldName, SIZE - 1, len);
    return false;
  }
  return true;
}

void RunWorkflowCLI(WorkflowCLI* cli) {
  if (cli == NULL) {
    printf("Error! CLI not initialized.\n");
    return;
  }

  char command[MAX_COMMAND_LEN];
  char arg1[MAX_COMMAND_LEN];  // 임시 버퍼는 크게
  char arg2[MAX_COMMAND_LEN];  // 임시 버퍼는 크게

  printf("\n========== Workflow Tree CLI ==========\n");
  printf("Type 'help' for available commands.\n");
  printf("=======================================\n");

  while (1) {
    ShowCurrentPath(cli);
    printf("> ");

    if (fgets(command, MAX_COMMAND_LEN, stdin) == NULL) {
      break;
    }

    // 개행 문자 제거
    command[strcspn(command, "\n")] = 0;

    // 빈 입력 무시
    if (strlen(command) == 0) {
      continue;
    }

    // 명령어 파싱
    char cmd[SIZE] = {0};
    arg1[0] = '\0';
    arg2[0] = '\0';

    int parsed = ParseCommand(command, cmd, arg1, arg2);

    // 디버깅 출력
    printf("[DEBUG] parsed=%d, cmd='%s', arg1='%s', arg2='%s'\n",
           parsed, cmd, arg1, arg2);

    // 명령어 처리
    if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
      printf("Exiting...\n");
      break;
    }
    else if (strcmp(cmd, "help") == 0) {
      ShowHelp();
    }
    else if (strcmp(cmd, "clear") == 0 || strcmp(cmd, "cls") == 0) {
      ClearScreen();
    }
    else if (strcmp(cmd, "pwd") == 0) {
      ShowCurrentPath(cli);
    }
    else if (strcmp(cmd, "ls") == 0) {
      ListChildren(cli);
    }
    else if (strcmp(cmd, "tree") == 0) {
      ShowFullTree(cli);
    }
    else if (strcmp(cmd, "view") == 0) {
      int maxLen = FindMaxDataLength(cli->tree->Head, cli->tree->ChildSize);
      int width = maxLen + 1; // 여백을 위해 1 추가
      printf("\n");
      TreeView(cli->tree, width);
      printf("\n");
    }
    else if (strcmp(cmd, "info") == 0) {
      ShowCurrentNode(cli);
    }
    else if (strcmp(cmd, "cd") == 0) {
      if (parsed < 2) {
        printf("Usage: cd <index|..|/>\n");
      } else {
        ChangeDirectory(cli, arg1);
      }
    }
    else if (strcmp(cmd, "insert") == 0) {
      if (parsed < 3) {
        printf("Usage: insert <position> <data>\n");
        printf("  Or: insert <position> \"data with spaces\"\n");
      } else {
        if (!ValidateStringLength(arg2, "Data")) {
          continue;
        }
        int pos = atoi(arg1);
        InsertNode(cli, arg2, pos);
      }
    }
    else if (strcmp(cmd, "insertp") == 0) {
      if (parsed < 2) {
        printf("Usage: insertp <data>\n");
        printf("  Or: insertp \"data with spaces\"\n");
      } else {
        if (!ValidateStringLength(arg1, "Data")) {
          continue;
        }
        InsertParent(cli, arg1);
      }
    }
    else if (strcmp(cmd, "remove") == 0) {
      if (parsed >= 2 && strcmp(arg1, "all") == 0) {
        RemoveNode(cli, true);
      } else {
        RemoveNode(cli, false);
      }
    }
    else if (strcmp(cmd, "edit") == 0) {
      if (parsed < 2) {
        printf("Usage: edit <new_data>\n");
        printf("  Or: edit \"new data with spaces\"\n");
      } else {
        if (!ValidateStringLength(arg1, "Data")) {
          continue;
        }
        EditCurrentNode(cli, arg1);
      }
    }
    else {
      printf("Unknown command: %s\n", cmd);
      printf("Type 'help' for available commands.\n");
    }
  }
}
