#include <stdio.h>
#include "cli/workflow_cli.h"

int main (int argc, char* argv[]) {

  // CLI 초기화 (각 노드는 최대 4개의 자식을 가질 수 있음)
  WorkflowCLI* cli = InitWorkflowCLI(4);
  if (cli == NULL) {
    printf("Failed to initialize CLI.\n");
    return 1;
  }

  // CLI 실행
  RunWorkflowCLI(cli);

  // 정리
  CleanupWorkflowCLI(cli);

  return 0;
}