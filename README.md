# 🗡️ Forsaken Dungeon  
**Unreal Engine 5 | 액션 RPG 시스템 구축 개인 프로젝트**

Forsaken Dungeon은 전투·AI·스탯·장비·UI 등 액션 RPG의 핵심 시스템을  
처음부터 끝까지 직접 설계하고 구현한 개인 프로젝트입니다.  

게임플레이 프로그래머로서 필요한 **전투 시스템, AI 패턴, 데이터 기반 시스템 설계, UI/UX 흐름** 등을 모두 경험하기 위해 제작되었습니다.

---

## 🖥️ **프로젝트 주요 특징**

### ⚔️ 1. 전투 시스템 (Combat System)
- Root Motion 기반 이동 & 공격
- 무기 Overlap 판정 기반 타격 시스템
- 공격 모션 타이밍은 Anim Notify 기반으로 처리
- 회피(Dodge) 시스템 + 몽타주 기반 무적 프레임
- Bleed, Crit 등 특수 효과 적용 로직 구현

---

### 🧠 2. AI & 보스 패턴 (AI Behavior System)
- Behavior Tree 기반의 의사결정 구조 구성
- 탐색 → 추격 → 공격 패턴 실행
- 보스는 **근접/원거리 2종 공격 패턴** 보유
- AI가 공격 중 멈추는 문제를 상태 플래그/Notify 기반으로 수정
- 공격 쿨타임 및 상태 기반 제어 로직 작성

---

### 📦 3. 인벤토리 & 장비 시스템 (Inventory & Equipment)
- Backpack / LootChest / Stash 시스템 완비
- 아이템 드래그·드롭, 이동, 수납, 교환 기능 제작
- 장비 착용/해제 시 스탯 자동 업데이트
- 영웅/전설 등급 및 접두어 기반 추가 능력치 시스템
- Tooltip에서 아이템 스탯 정보 자동 표시

---

### 🧮 4. 스탯 & 능력치 구조 (Attributes System)
- Strength, Vitality, Intelligence 등 기본 스탯
- 무기/장비/특전(Prefix) 적용 시 **자동 합산**
- AttackPower, MaxHP 등 파생 능력치는 자동 재계산
- 레벨업, 경험치, 초기화(Reset) 로직 구현

---

### 🗝️ 5. 상호작용 시스템 (Interaction)
- 포탈 상호작용 → 집중 몽타주 → 이동
- 문(Door) 여닫기 + 사운드 효과
- 상자(Chest) 열기 + 사운드 효과
- UI 열려 있을 때 상호작용 차단

---

### 🎨 6. UI/UX 시스템 (UMG)
- 인벤토리, 툴팁, 장비창, 스탯창
- 마우스 커서 제어, 입력모드 전환(Game/UI)
- 텍스트 숨김 처리, 동적 내용 업데이트
- 메인 메뉴 UI + 게임 시작 버튼

---

## 🛠️ **기술 스택 & 사용 기술**

| 분류 | 내용 |
|------|------|
| Engine | Unreal Engine 5.3 / 5.5 |
| Language | C++ / Blueprint |
| AI | Behavior Tree, Blackboard, EQS 일부 구조 |
| Animation | Montage, Anim Notify, Root Motion |
| UI | UMG, Slate Widget |
| ETC | Git, Data Asset 기반 제작 |

---

## 📁 **프로젝트 구조**

Source/
├─ Character/
│ ├─ EchoPlayer.cpp/h
│ └─ EnemyBase.cpp/h
│
├─ AI/
│ ├─ BTService_FindTarget
│ ├─ BTTask_PerformAttack
│ ├─ BTTask_BossMeleeAttack
│ ├─ BTTask_BossRangeAttack
│ └─ BTDecorator_CanAttack
│
├─ Items/
│ ├─ ItemDefinition
│ ├─ ItemInstance
│ ├─ InventoryRootWidget
│ ├─ ItemGridWidget
│ ├─ EquipmentComponent
│ └─ Tooltip
│
├─ World/
│ ├─ PortalActor
│ ├─ DoorActor
│ └─ LootChestActor
│
└─ UI/
├─ MainMenuWidget
└─ HUD, Stat Widgets
