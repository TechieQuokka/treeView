# N-Tree Workflow CLI

C언어로 구현된 N진 트리(N-ary Tree) 기반의 워크플로우 관리 CLI 도구입니다. 트리 구조를 사용하여 계층적인 작업 흐름을 생성, 수정, 관리할 수 있습니다.

## 주요 기능

### 트리 구조 관리
- **N진 트리**: 각 노드가 최대 N개의 자식 노드를 가질 수 있는 유연한 트리 구조
- **노드 삽입**: 자식 노드 또는 부모 노드를 특정 위치에 삽입
- **노드 삭제**: 개별 노드 삭제 또는 하위 트리 전체 삭제
- **노드 이동/교환**: 자식 노드들의 순서 변경

### 트리 시각화
- **계층적 뷰**: 들여쓰기를 사용한 트리 구조 표시
- **박스 트리 뷰**: 유니코드 문자를 사용한 시각적 트리 표현
- **가로 트리 뷰**: 수평으로 펼쳐진 트리 구조 (인덱스 표시 포함)

### 인터랙티브 CLI
- **디렉토리 네비게이션**: cd 명령어로 트리 노드 간 이동
- **경로 표시**: 현재 위치의 전체 경로 표시
- **자동 저장**: JSON 형식으로 트리 상태 자동 저장/불러오기

## 프로젝트 구조

```
treeView/
├── main.c                  # 프로그램 진입점
├── lib/
│   ├── n_tree.h           # N진 트리 핵심 자료구조 헤더
│   ├── n_tree.c           # N진 트리 구현 (생성, 삽입, 삭제 등)
│   ├── util.h             # 유틸리티 함수 헤더
│   └── util.c             # 트리 시각화 및 편의 기능
├── cli/
│   ├── workflow_cli.h     # CLI 인터페이스 헤더
│   └── workflow_cli.c     # CLI 구현 (명령어 처리, JSON I/O)
└── workflow_tree.json     # 트리 상태 저장 파일 (자동 생성)
```

## 핵심 자료구조

### TreeNodeType
```c
struct TreeNodeType {
    char Data[SIZE];                  // 노드 데이터 (최대 16자)
    struct TreeNodeType* Parent;      // 부모 노드 포인터
    struct TreeNodeType** Children;   // 자식 노드 배열
};
```

### N_TreeType
```c
struct N_TreeType {
    struct TreeNodeType* Head;   // 루트 노드
    int Count;                   // 전체 노드 개수
    int ChildSize;              // 각 노드의 최대 자식 개수
};
```

## 빌드 및 실행

### 요구사항
- C 컴파일러 (GCC, Clang, MSVC 등)
- C 표준 라이브러리

### 컴파일
```bash
# GCC 사용 예시
gcc -o treeview main.c lib/n_tree.c lib/util.c cli/workflow_cli.c

# 또는 개별 컴파일
gcc -c lib/n_tree.c -o lib/n_tree.o
gcc -c lib/util.c -o lib/util.o
gcc -c cli/workflow_cli.c -o cli/workflow_cli.o
gcc -c main.c -o main.o
gcc main.o lib/n_tree.o lib/util.o cli/workflow_cli.o -o treeview
```

### 실행
```bash
./treeview
```

## CLI 명령어

### 네비게이션
- `pwd` - 현재 경로 표시
- `cd <index>` - 자식 노드로 이동 (인덱스 지정)
- `cd ..` - 부모 노드로 이동
- `cd /` - 루트 노드로 이동
- `ls` - 현재 노드의 자식 노드 목록 표시

### 노드 조작
- `insert <data> <position>` - 지정된 위치에 자식 노드 삽입
- `parent <data>` - 현재 노드 위에 부모 노드 삽입
- `edit <data>` - 현재 노드의 데이터 수정
- `rm` - 현재 노드 삭제 (자식이 1개 이하일 때)
- `rm -r` - 현재 노드와 하위 트리 전체 삭제

### 트리 보기
- `tree` - 전체 트리 구조 표시
- `show` - 박스 형태의 트리 뷰 표시
- `view [width]` - 가로 트리 뷰 표시 (선택적으로 노드 너비 지정)
- `stat` - 트리 통계 정보 표시

### 기타
- `help` - 도움말 표시
- `clear` - 화면 지우기
- `save` - 수동으로 트리 저장
- `exit` - 프로그램 종료

## 주요 함수

### N진 트리 핵심 함수 ([n_tree.c](lib/n_tree.c))

- `Create_N_Tree(int number_of_children)` - N진 트리 생성
- `Insert_ChildNode_N_Tree(...)` - 자식 노드 삽입
- `Insert_ParentNode_N_Tree(...)` - 부모 노드 삽입
- `Remove_N_Tree(...)` - 노드 삭제
- `Clear_N_Tree(...)` - 하위 트리 삭제
- `Delete_N_Tree(...)` - 전체 트리 삭제

### 시각화 함수 ([util.c](lib/util.c))

- `Display(struct N_TreeType* tree)` - 기본 트리 표시
- `Show(struct N_TreeType* tree)` - 박스 트리 뷰
- `TreeView(struct N_TreeType* tree, int nodeWidth)` - 가로 트리 뷰

### CLI 함수 ([workflow_cli.c](cli/workflow_cli.c))

- `InitWorkflowCLI(int childSize)` - CLI 초기화 및 JSON 로드
- `RunWorkflowCLI(WorkflowCLI* cli)` - 메인 루프
- `SaveTreeToJSON(WorkflowCLI* cli)` - JSON 파일로 저장
- `LoadTreeFromJSON(WorkflowCLI* cli)` - JSON 파일에서 로드

## 사용 예시

```
$ ./treeview

=== Workflow Tree CLI ===
Type 'help' for available commands.

Current Path: /Root
[Root] > insert Task1 0
Node 'Task1' inserted at position 0.

[Root] > insert Task2 1
Node 'Task2' inserted at position 1.

[Root] > cd 0
Changed to: Task1

Current Path: /Root/Task1
[Task1] > insert Subtask1 0

[Task1] > cd ..

[Root] > tree
[H]Root
  Task1
    Subtask1
    NULL
    NULL
    NULL
  Task2
    NULL
    NULL
    NULL
    NULL
  NULL
  NULL

[Root] > show
Root
├── Task1
│   └── Subtask1
└── Task2
```

## 특징

### 메모리 관리
- 동적 메모리 할당 및 해제 처리
- 메모리 누수 방지를 위한 재귀적 정리 함수

### 에러 처리
- 모든 주요 함수에서 입력 검증
- 명확한 에러 메시지 출력

### 데이터 영속성
- JSON 형식으로 트리 구조 저장
- 프로그램 재시작 시 자동 복원

### 유연한 트리 구조
- 초기화 시 자식 노드 개수 설정 가능
- 최대 128개의 자식 노드 지원

## 제한사항

- 노드 데이터 크기: 최대 16자 (SIZE 상수로 조정 가능)
- 최대 자식 노드 개수: 128개 (CHILD_SIZE 상수로 조정 가능)
- 최대 경로 깊이: 64 레벨 (MAX_PATH_DEPTH 상수로 조정 가능)

## 라이선스

이 프로젝트는 교육 및 학습 목적으로 작성되었습니다.

## 기여

버그 리포트 및 개선 제안은 이슈로 등록해주세요.
