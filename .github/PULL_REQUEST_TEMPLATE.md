## 작업 내용
제목은 [파트] 작업 요약 (Closes #이슈번호)
- 레벨 10 도달 시 서버에서 보스 1회 생성
- GameState에 Phase 반영
- HUD에 보스 등장 신호 송출

## 변경 파일
- MainGameMode.h / .cpp
- MainGameState.h / .cpp

## 테스트 방법
- 레벨 9: 보스 생성 X
- 레벨 10: 보스 1회 생성
- 이후 중복 생성 없음

## 고려 사항
- GameMode만 수정 (UI는 별도 PR 예정)
