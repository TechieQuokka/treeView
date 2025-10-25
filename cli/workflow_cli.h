#ifndef _WORKFLOW_CLI_H_
#define _WORKFLOW_CLI_H_

#include "../lib/n_tree.h"

#define MAX_PATH_DEPTH 1024
#define MAX_COMMAND_LEN 256
#define JSON_FILE_PATH "workflow_tree.json"
#define MAX_PARSE_DEPTH 100
#define MAX_PARSE_ITERATIONS 1000

// CLI 상태를 관리하는 구조체
typedef struct {
  struct N_TreeType* tree;
  struct TreeNodeType* current;  // 현재 위치 노드
  char path[MAX_PATH_DEPTH][SIZE];  // 현재 경로
  int pathDepth;  // 경로 깊이
  char jsonFilePath[256];  // JSON 파일 경로
} WorkflowCLI;

// CLI 초기화 및 종료
WorkflowCLI* InitWorkflowCLI(int childSize);
void CleanupWorkflowCLI(WorkflowCLI* cli);

// CLI 메인 루프
void RunWorkflowCLI(WorkflowCLI* cli);

// 명령어 처리 함수들
void ShowHelp(void);
void ClearScreen(void);
void ShowCurrentPath(WorkflowCLI* cli);
void ShowCurrentNode(WorkflowCLI* cli);
void ShowFullTree(WorkflowCLI* cli);
void ChangeDirectory(WorkflowCLI* cli, const char* target);
void InsertNode(WorkflowCLI* cli, const char* data, int position);
void InsertParent(WorkflowCLI* cli, const char* data);
void RemoveNode(WorkflowCLI* cli, bool clearAll);
void ListChildren(WorkflowCLI* cli);
void EditCurrentNode(WorkflowCLI* cli, const char* newData);

// 유틸리티 함수
void UpdatePath(WorkflowCLI* cli);
int GetChildIndex(struct TreeNodeType* parent, struct TreeNodeType* child, int childSize);

// JSON 저장/불러오기 함수
void SaveTreeToJSON(WorkflowCLI* cli);
bool LoadTreeFromJSON(WorkflowCLI* cli);
void AutoSave(WorkflowCLI* cli);

#endif
