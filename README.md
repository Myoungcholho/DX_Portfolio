# 📘DirectX 11 자체 게임 엔진
<br>
<img align="right" width="420"
     src="https://github.com/user-attachments/assets/6be7f5a3-344d-4635-8365-18ef474563fc"
     alt="DirectX 11 Engine Editor Screenshot" />

- 소개 영상: [포트폴리오 영상](https://www.youtube.com/watch?v=CVj2y5PXsnw)
- 개발 기간: 2025.05 ~ 2025.10 (156일)
- 개발 인원: 1명
- 핵심 요약: 언리얼 아키텍처를 참고해 재구현한 자체 게임 엔진
- 개발 환경:
  - 언어: C++, HLSL
  - 실행 및 디버깅 툴: VS2022, RenderDoc, Deleaker, IBLMaker
  - 라이브러리: DX11, ImGuiDocking, DirectXTex, Assimp, SimpleMath

<br clear="both"/>

# 📘목차
- [개발 계기](#개발-계기)
- [아쉬웠던 점](#아쉬웠던-점)
- [프로젝트에서 얻은 것](#프로젝트에서-얻은-것)
- [구현 내용](#구현-내용)
- [문제 해결 경험(트러블 슈팅)](#문제-해결-경험트러블-슈팅)
- [핵심 주요 코드](#핵심-주요-코드)

# 📘개발 계기
### 1. 상용 엔진에서 제공하는 기능들의 내부 구현에 대한 호기심

상용 엔진에서 버튼·체크박스 한 번으로 끝나는 기능들이  
어떤 자료구조와 업데이트 순서, 수학으로 돌아가는지 궁금해져  
“엔진이 해주는 일을 직접 구현해 보자”는 생각으로  
DX11 기반 자체 엔진 개발을 시작했습니다.

### 2. 렌더링 과정에 대한 이해와 직접 적용

렌더링이 많은 자원을 쓰는 건 느끼면서도 파이프라인을 몰라 병목을 찾기 어려웠고,  
자원 관리는 프로그래머의 핵심 덕목이라 생각해  
렌더링 파이프라인을 직접 구현하며 구조를 파고들고자 했습니다.

### 3. 언리얼 엔진의 설계 철학 이해 및 적용

언리얼의 복잡하지만 개발자 친화적인 구조를 보며  
엔진 레벨 설계가 클라이언트 개발 효율로 이어진다는 걸 체감했고,  
이 철학을 DX11 자체 엔진 구조로 직접 옮겨 체득하고자 했습니다.

# 📘아쉬웠던 점

**요약**
- [1. 성능·확장성을 고려하지 못한 초기 설계와 구조 개편](#weak-arch)
- [2. 감으로 시작한 최적화](#weak-optim)
- [3. 애니메이션 파이프라인 구조 이해 부족](#weak-anim-pipeline)
- [4. 애니메이션 업데이트 주기 설계 미스](#weak-anim-tick)
- [5. Renderer에 과도하게 몰린 책임](#weak-responsibility)

---

### 1. 성능·확장성을 고려하지 못한 초기 설계와 구조 개편 <a id="weak-arch"></a>

처음 DX11 엔진을 설계할 때 인스턴싱을 충분히 고려하지 않아, 단일 렌더 기준 구조로 출발한 점이 아쉬웠습니다.  

프로젝트가 커지면서 대량 렌더링에서 병목이 드러났고, 결국 렌더링 파이프라인을 인스턴싱 중심으로 한 번 전면 개편해야 했습니다.  

이 과정에서 초기 설계부터 확장 요구사항을 가정하고 구조를 잡는 습관의 중요성을 배웠습니다.

---

### 2. 감으로 시작한 최적화 <a id="weak-optim"></a>

대량 렌더링 상황에 프레임 드랍이 발생했을 때 감으로 CPU 최적화부터 적용했다가,  
프레임 드랍이 잡히지 않아 GPU 병목 프로파일링의 필요성을 뒤늦게 깨달은 점이 아쉬웠습니다.

이 일을 계기로 앞으로는 감에 기대기보다,  
프로파일링과 계측으로 원인을 먼저 확인한 뒤 해결 방향을 정하는 태도를 가지려고 하고 있습니다.

---

### 3. 애니메이션 파이프라인 구조 이해 부족으로 인한 불필요한 변환 <a id="weak-anim-pipeline"></a>

애니메이션 파이프라인 설계 초기에 로컬 포즈와 팔레트 변환의 책임을 잘못 나눠,  
블렌딩 과정에서 로컬 포즈 ↔ 팔레트 변환이 중복으로 일어나는 구조를 만든 점이 아쉬웠습니다.

이후 언리얼의 UAnimInstance가 맡는 책임을 공부하며  
로컬 포즈 단계에서 블렌딩하고 최종 단계에서만 팔레트로 변환하는 구조를 이해했고,  
이를 기준으로 파이프라인을 개편했습니다.

이 경험을 통해 구현에 들어가기 전에  
각 클래스의 책임과 역할을 먼저 명확히 정리하는 습관을 갖게 되었습니다.

---

### 4. 애니메이션 업데이트 주기를 잘못 잡은 점 <a id="weak-anim-tick"></a>

애니메이션 연산을 처음에는 물리와 같이 Fixed 구간에서 처리하면  
더 정확하고 최적화에도 유리할 거라 생각했습니다.  

하지만 대량 블렌딩 시 Fixed 구간이 병목이 되어 심한 프레임 드랍이 발생했고,  
애니메이션은 물리처럼 정확한 시뮬레이션이 아니라  
매 프레임 Δt에 맞춰 자연스럽게 보이도록 갱신하면 되는 연산이라는 점을 뒤늦게 깨달았습니다.

언리얼처럼 Tick 기반으로 옮기자 문제가 해결되었고,  
그 이후로는 시스템의 목적과 특성을 먼저 정의한 뒤  
업데이트 전략을 선택해야 한다는 점을 기준으로 삼게 되었습니다.

---

### 5. 역할을 한 클래스에 몰아넣은 책임 분리 실패 <a id="weak-responsibility"></a>

초기에는 Renderer가 렌더 패스 구성, 순서 제어, 프록시 분류, 드로우 호출까지 모두 담당하는 구조로 설계해,  
Depth/Shadow/PostProcess 패스를 추가할 때마다 URenderer 내부 로직과 호출 순서를 자주 수정해야 했고  
코드가 쉽게 비대해지는 문제가 있었습니다.

이후 수집된 프록시 분류와 패스 구성·순서 결정은 RenderManager와 RenderQueue로 분리하고,  
Renderer는 GPU 리소스 수명 관리와 드로우 호출만 담당하도록 개편했습니다.

이 경험을 통해 한 클래스나 모듈에 책임을 과도하게 몰아넣는 것이  
구조를 얼마나 쉽게 망가뜨리는지 체감했고,  

다른 시스템을 설계할 때도  
“역할과 경계를 먼저 나누고 들어가는 습관”이 몸에 배게 되었습니다.

# 📘프로젝트에서 얻은 것

**요약**
- [1. 드로우콜 렌더링 파이프라인 감각](#gain-drawcall)
- [2. 언리얼 아키텍처 이해와 재구현 경험](#gain-ue-arch)
- [3. C++ 자원 관리와 소유권 감각](#gain-cpp-resource)
- [4. 게임 스레드 / 렌더 스레드 분리·멀티스레드 이해](#gain-gt-rt)
- [5. 행렬·계층 구조 기반 씬/애니메이션 처리 경험](#gain-matrix)
- [6. 쉐이딩 모델·라이팅 이해](#gain-shading)
- [7. Transform·회전·짐벌락을 다루며 쌓은 수학적 감각](#gain-rotation)

---

### 1. 드로우콜 렌더링 파이프라인 감각 <a id="gain-drawcall"></a>

도형을 직접 그려 보고, 하나하나 드로우콜을 날려 보면서  
물체를 화면에 그리기 위해 어떤 단계들이 필요하고,  
그 과정에서 어느 정도 자원이 소모되는지 체감할 수 있었습니다.

---

### 2. 언리얼 아키텍처에 대한 이해와 재구현 경험 <a id="gain-ue-arch"></a>

언리얼 아키텍처를 내 엔진으로 옮겨 구현해 보면서  
Actor / Component / SceneComponent / Controller 등의 계층 구조와 책임 분리,  
그리고 언리얼이 어떤 방식으로 업데이트와 최적화를 조직하는지  
더 깊게 이해하게 되었습니다.

---

### 3. C++ 자원 관리와 소유권에 대한 감각 <a id="gain-cpp-resource"></a>

엔진 레벨에서 객체 생명주기와 소유권을 직접 관리하다 보니  
`std::move`, `std::forward`를 사용해야 하는 이유와  
RAII / 스마트 포인터를 활용한 효율적인 자원 관리 방법을 몸으로 익힐 수 있었습니다.  

SpawnActor와 유사한 흐름을 구현하면서  
팩토리 패턴, 가변 인자, perfect forwarding 같은 언어 레벨 기법을  
실제 설계에 적용해 볼 수 있었던 점도 컸습니다.

---

### 4. 게임 스레드 / 렌더 스레드 분리와 멀티스레드 이해 <a id="gain-gt-rt"></a>

렌더 스레드와 게임 스레드를 분리해 동작시키는 과정에서  
직접 레이스 컨디션과 동기화 문제를 마주하고 해결해 보며,  
멀티스레드 환경에서 데이터를 안전하게 다루는 방법과 패턴에 대한  
이해를 넓힐 수 있었습니다.

---

### 5. 행렬·계층 구조를 활용한 씬/애니메이션 처리 경험 <a id="gain-matrix"></a>

컨텐츠 레벨에서는 잘 드러나지 않던 행렬 연산들이  
SceneComponent의 부모–자식 계층, 스켈레탈 본 트리 계산, WVP 변환 같은 부분에서  
어떻게 쓰이는지 직접 구현해 보았습니다.  

이 과정에서 선형(행렬)·비선형(회전) 변환,  
그리고 SIMD가 주는 이점을 실제 코드와 함께 이해하게 되었습니다.

---

### 6. 쉐이딩 모델과 라이팅에 대한 이해 <a id="gain-shading"></a>

Phong 쉐이딩부터 Blinn-Phong, PBR 모델까지 직접 구현해 보면서  
게임 화면의 표면이 어떤 라이팅 연산을 수행하는지,  
하이라이트·반사·러프니스 같은 개념이 수식과 결과 이미지에서  
어떻게 연결되는지 이해할 수 있었습니다.

---

### 7. Transform, 회전, 짐벌락을 다루며 쌓은 수학적 감각 <a id="gain-rotation"></a>

Transform 시스템을 구현하고 에디터에서 즉시 반영되는 구조를 만들면서  
잘못된 회전 적용, 축 고정, 짐벌락 같은 문제들을 직접 마주했습니다.  

Quat ↔ Euler 변환을 수학적으로 다뤄 보며  
회전 표현에 대한 직관과 문제 해결 능력을 키울 수 있었습니다.


# 📘구현 내용
| [🧱 Core Architecture](#core) | [🌍 World / Object](#world) | [🎨 Rendering](#rendering) | [🕺 Animation](#animation) | [📦 Asset](#asset) | [🛠 Editor](#editor) | [📊 profiling](#profiling) |
|----------------------|------------------|---------------|---------------|-----------|------------|----------|

<h3 id="core">1. 코어 아키텍처 & 실행 구조</h3>

- GT/RT 분리 아키텍처 (MailBox 기반 스냅샷 소비 모델)
- `std::function` 기반 델리게이트/이벤트 시스템
- CommandQueue 구조로 에디터–게임 간 동기 호출로 인한 스톨 제거

---

<h3 id="world">2. 월드·객체 구조 & 트랜스폼</h3>

- Transform 시스템 (월드/로컬 변환 및 위치 정보 관리)
- GameInstance / UWorld / SceneComponent / PrimitiveComponent / RenderProxy 구조
- AActor / StaticMeshComponent / SkeletalMeshComponent 구성
- Pawn / Controller / GameMode 계층
- ClassID 기반 Actor 런타임 스폰 팩토리 (리플렉션 없이 타입 등록/생성)
- 전역 입력 시스템 (마우스·키보드 입력을 Controller로 라우팅)

---

<h3 id="rendering">3. 렌더링 파이프라인 & 그래픽스</h3>

- 그래픽 파이프라인 상태 집합 객체(PSO) 설계  
  (Shader·Blend·Rasterizer·Depth 상태 일체 관리)
- RenderManager(패스 구성) / RenderQueue(배치·정렬) / Renderer(드로우 호출) 역할 분리
- HDR RenderTarget 파이프라인, MSAA, ToneMapping
- Fog / Bloom / Shadow / PBR 라이팅

---

<h3 id="animation">4. 애니메이션 시스템</h3>

- 애니메이션 재생 시스템 (AnimationClip 기반 재생·루프·이동 관리)
- AnimInstance (UAnimInstance와 유사한 상태·블렌딩 로직 전담 계층)
- 애니메이션 블렌딩 (키프레임 보간 기반 블렌딩)
- 카메라 거리 기반 Tick 최적화 (Update Rate Optimization)

---

<h3 id="asset">5. 에셋 & 자원 관리</h3>

- .fbx 파일 로드 파이프라인 (메시·본·애니메이션 데이터 파싱)
- CPU / GPU / 애니메이션 자원 공유 관리 클래스

---

<h3 id="editor">6. 에디터 & 툴링</h3>

- ImGui Docking 기반 에디터 UI
- WorldOutliner / ActorOutliner / InspectorWindow를 통한 런타임 액터/컴포넌트 편집

---

<h3 id="profiling">7. 프로파일링</h3>

- 프레임/섹션 단위 측정 프로파일링 시스템
- CPU / GPU 타임스탬프 기반 프로파일링 시스템





# 📘문제 해결 경험(트러블 슈팅)

**요약**
- [1. 더블 버퍼 레이스 컨디션 해결 — 최소 잠금 + 스냅샷 구조](#ts-double-buffer)
- [2. Actor.Transform 도입 — RootComponent 의존 구조 문제 해결](#ts-transform)
- [3. Static/Skeletal 대량 렌더링 프레임 저하 — CPU/GPU 병목 분석 & 인스턴싱 적용](#ts-instancing)
- [4. CPU·GPU 타임 왜곡 — Present() 측정 시점 오류 수정](#ts-present)
- [5. 애니 블렌딩 덜덜거림 — 정수 기반 샘플링 → 시간 보간 방식 개선](#ts-animation)

---

### 1. 더블 버퍼의 락 회피 시도와 최소 잠금 전환 <a id="ts-double-buffer"></a>

- 문제 상황  

게임 스레드와 렌더 스레드를 분리한 뒤, 더블 버퍼 + 인덱스 스왑 방식으로 렌더링 데이터를 넘기도록 구현했으나  
특정 상황에서 프레임 간 렌더링 결과가 일관되지 않거나, 드물게 비정상 포인터 접근으로 예외가 발생하는 문제가 나타났습니다.

- 원인 분석  

프레임 스톨을 우려해 별도의 임계구역 없이 인덱스 스왑만으로 처리했으나,  
인덱스 스왑 시점과 버퍼 재사용 시점이 겹치는 경우가 있어  
더블 버퍼 내부에서도 레이스 컨디션이 발생할 수 있는 구조였습니다.

- 해결  

락을 없애는 것이 목적이 아니라, **락을 최소화하면서 데이터 일관성을 보장하는 것**을 목표로 재정의했습니다.  

더블 버퍼 구조 자체는 유지하되, 프레임 경계에서 인덱스(포인터)를 스왑하는 짧은 구간에만 잠금을 도입해  
완전 무락의 위험한 구조 대신 ‘최소 잠금 + 더블 버퍼 스냅샷’ 구조로 수렴시켰습니다.  

이를 통해 대부분의 구간에서는 락 없이 동작하면서도,  
필요한 순간에만 짧게 동기화를 보장하는 설계로 정리했습니다.

- 결과  
<img width="446" height="284" alt="image" src="https://github.com/user-attachments/assets/d6701af8-2ce2-4c9d-9145-91f55a9ecbd3" />

인덱스 스왑 시점의 극히 짧은 잠금만으로도 데이터 일관성을 유지하면서,  
락으로 인한 스톨을 최소화해 GT/RT 분리 이후에도 안정적인 프레임을 유지하는 구조를 만들 수 있었습니다.  

이후 동일한 패턴의 비정상 접근 예외나 프레임 간 불일치 현상은 재발하지 않았습니다.

- 배운 점  
1. 멀티스레드 문제는 단순히 mutex를 어디에 더/덜 거느냐의 문제가 아니라,  
   데이터의 수명과 성격에 맞게 구조 자체를 바꾸는 것이 근본적인 해결이 될 수 있다는 점을 체감했습니다.  
2. “락을 없애자”가 목표가 아니라, **일관성을 해치지 않는 선에서 잠금을 최소화하는 설계**가 중요하다는 것을 배웠고,  
   이후 에디터–런타임 동기화 문제를 볼 때도 같은 관점으로 접근하게 되었습니다.

---

### 2. Actor 자체 Transform 도입과 Root 이관 <a id="ts-transform"></a>

- 문제 상황  

초기 설계에서는 Actor가 자체 Transform을 갖지 않고,  
항상 RootComponent의 Transform만 참조하는 구조였습니다.  

그런데 Pawn처럼 컴포넌트가 없이도 컨트롤러 입력으로 위치/회전을 갱신해야 하는 Actor가 필요해지면서,  
RootComponent가 없을 때 기준이 될 안정적인 좌표가 없는 문제가 드러났습니다.

- 원인 분석  

Actor는 항상 RootComponent를 가진다는 가정 하에 설계되어 있어  
Actor의 위치/회전을 표현할 수 있는 권위 있는 기준 좌표가 존재하지 않았습니다.  

Pawn이 컴포넌트 없이 이동해야 하는 경우에 처리할 기준이 없었고,  
컴포넌트를 지웠다 다시 붙이는 경우 기준 정보가 끊기거나 초기화되는 문제도 덤으로 발생했습니다.

- 해결  

Root가 붙기 전까지 `Actor.Transform`을 기준으로 사용하되,  
Root가 생긴 이후로는 `RootComponent`의 Transform이 기준이 되도록 해  
Actor와 Root 간 Transform 소유권을 명확히 나눴습니다.

- 결과  

RootComponent가 없어도 Actor의 Transform만으로 임시 위치를 유지하며 배치할 수 있으며,  
컴포넌트를 지우거나 교체하더라도 Actor의 배치 상태가 그대로 유지되어,  
Pawn처럼 컴포넌트 유무와 상관없이 위치/회전을 안정적으로 관리할 수 있는 구조가 되었습니다.

- 배운 점  

언리얼에서 Actor가 Transform을 별도로 가지고 있다는 사실은 알고 있었지만,  
제 엔진에서는 당장은 필요 없다고 판단해 Actor 차원의 Transform은 두지 않았습니다.  

결국 Pawn처럼 컴포넌트 없이도 위치를 가져야 하는 경우가 생기면서  
동일한 구조가 다시 필요해졌습니다.  

이 경험을 통해, 언리얼 엔진이 Actor가 Transform을 소유하도록 설계한 이유를  
충분히 이해하고 있었다면 애초에 더 탄탄한 설계를 할 수 있었을 것이라는 생각을 하게 되었습니다.  

앞으로는 나중에 구현된 구조를 가져다 쓰더라도  
**무엇을 왜 소유하는지를 먼저 명확히 파악한 뒤 적용하는 습관**을 들이고자 합니다.

---

### 3. 인스턴싱 도입 전 Static / Skeletal 프레임 저하 원인 파악 <a id="ts-instancing"></a>

- 문제 상황  
<img width="362" height="121" alt="image" src="https://github.com/user-attachments/assets/588c3f09-b8c8-4022-b4ba-7514054077bf" />

동일한 스켈탈 애니메이션 모델 100개를 렌더링했을 때  
FPS가 약 **40.9**까지 하락하는 문제가 발생했습니다.

- 원인 분석  
<img width="362" height="121" alt="image" src="https://github.com/user-attachments/assets/f5905d9b-ad01-400a-b7eb-f821da3a4b1b" />

프로파일링 결과, 한 캐릭터의 애니메이션/본 팔레트 계산에 약 0.15ms,  
100개 기준 약 15.2ms가 소모되어 **CPU 애니/팔레트 계산이 병목**이었고,  

동시에 개별 `DrawIndexed` 호출이 많아 **GPU 드로우콜 병목**도 존재하는 것을 확인했습니다.

- 해결  
<img width="362" height="121" alt="image" src="https://github.com/user-attachments/assets/63839d1d-9c11-446a-ac33-7b8e4c42f495" />

먼저 애니메이션 계산을 FixedUpdate에서 Tick/URO 방식으로 전환해  
모든 프레임을 정밀 계산하지 않고 보간·보정을 사용함으로써  
CPU 측 본 팔레트 계산 시간을 프레임 예산 안으로 줄였습니다.  

그 후 동일 모델을 묶어 그리기 위해 인스턴싱을 도입하고,  
개별 드로우콜을 통합해 GPU 드로우콜 병목을 줄였습니다.

- 결과  

애니 계산 구조 변경으로 CPU 병목이 해소되었고,  
인스턴싱으로 GPU 드로우콜 병목을 제거해  
기존 40.9 FPS였던 장면을 수직 동기화 기준인 75 FPS까지 끌어올릴 수 있었습니다.

- 배운 점  
1. 프레임 저하는 CPU / GPU 중 어느 한쪽 문제가 아니라,  
   **두 축을 분리해서 병목을 찾고 해결해야 한다**는 점을 인지했습니다.  
2. 이런 병목을 찾기 위해서는 감이 아니라 **프로파일링 수치로 검증하는 과정**이  
   프로그래머에게 매우 중요하다는 것을 느꼈습니다.  
3. 또한 모든 캐릭터를 매 프레임 완전 계산하기보다,  
   품질을 유지하는 범위에서 연산량을 줄이는 최적화 설계가 필수적이라는 것을 배웠습니다.

---

### 4. CPU·GPU 타임 비례 이상 징후 포착 <a id="ts-present"></a>

- 문제 상황  
<img width="461" height="110" alt="image" src="https://github.com/user-attachments/assets/bef2c646-d665-4921-985f-aee132da5e4c" />

CPU / GPU 프레임 타임을 시각화해 거리 기반 Tick에 따른 병목을 분석하던 중,  
CPU(Game)와 GPU 시간이 거의 비례해서 같이 움직이는 이상 징후가 있었습니다.

- 원인 분석  

제출 인스턴스 수와 드로우콜 구조는 동일한 상태에서  
CPU(Game) 부하를 줄였을 뿐인데 GPU 시간도 함께 감소하는 것은,  
실제 GPU 부하가 줄었다기보다 **측정 방식에 문제가 있을 가능성**을 의심하게 만들었습니다.

- 해결
<img width="461" height="110" alt="image" src="https://github.com/user-attachments/assets/55675cba-4be9-4d23-a658-b08ff153b109" />


타임라인을 다시 점검한 결과, GPU 타임스탬프를 `Present()` 이후에 찍고 있었고,  
이 구간에 프레임 페이싱 대기가 포함되면서 GPU 시간이  
실제 드로우 시간 + 대기 시간이 섞여  
CPU(Game) 시간과 비슷하게 보이는 왜곡이 발생하고 있었습니다.  

이에 GPU 실제 렌더링 시간만 측정할 수 있도록  
`Present()` 호출 직전 시점에 GPU 타임스탬프를 찍도록 위치를 조정했습니다.

- 결과
<img width="461" height="110" alt="image" src="https://github.com/user-attachments/assets/13e03291-599e-4167-9052-b5f25e53a587" />

타이밍 측정 위치를 수정한 뒤에는  
CPU(Game) ≒ 50ms, CPU(Render) ≒ 14ms, GPU ≒ 26ms 수준으로  
CPU와 GPU 시간이 비례하던 왜곡이 사라졌고,  
실제 병목이 GPU가 아니라 CPU 쪽(게임 로직/애니 계산)에 있다는 것을  
명확히 확인할 수 있었습니다.  

이후 인스턴싱 / URO 적용 등 최적화 방향을 정확히 잡는 기반이 되었습니다.

- 배운 점  

`Present()`에서 프레임 페이싱으로 인한 대기 시간이 섞이는 구간은  
GPU 부하로 오해하기 쉽습니다.  

이를 구분하려면 렌더링 파이프라인의 전체 흐름을 이해하고 있어야 했고,  
실제로 이번 문제를 해결할 때 그 이해가 큰 도움이 되었습니다.  

이 경험을 통해 단순히 그 순간의 결과를 맞추는 데서 끝나는 것이 아니라,  
항상 원인 단위까지 추적해 해결하는 방식으로 역량을 쌓아야 한다는 점을  
다시 한 번 확인했습니다.

---

### 5. 정수 기반 샘플링의 한계 – 블렌딩 시 덜덜거림 발생 <a id="ts-animation"></a>

- 문제 상황  

애니메이션마다 `ticksPerSec`(초당 키프레임 수)가 달라  
30 TPS ↔ 60 TPS 전환 시, 특히 30 → 60 전환에서  
블렌딩된 포즈가 계단식으로 갱신되며 화면에 미세한 덜덜거림이 보이는 문제가 발생했습니다.

- 원인 분석  

기존 샘플링 방식은 누적 시간을 `float`로 쌓되,  
`int`로 잘라 프레임 인덱스만 선택하는 정수 기반 샘플링이었습니다.  

TPS가 다른 두 클립을 섞을 때 목표 포즈가 키 프레임 단위로만 바뀌다 보니,  
시간 축에서 포즈가 계단처럼 점프하는 형태가 되었고,  
이 불연속성이 블렌딩 결과에 그대로 반영되면서 미세한 떨림으로 나타났습니다.

- 해결  

클립 간 블렌딩에 앞서, 각 클립을 **시간 기반 보간**으로 먼저 평가하도록 변경했습니다.  

누적 시간의 소수 부분을 보간 계수로 사용해  
키프레임 i, i+1 사이의 포즈를 선형 보간으로 계산한 뒤,  
얻은 두 클립의 보간 포즈를 다시 블렌딩하는 형식으로 바꾸었습니다.

- 결과  
![Animation](https://github.com/user-attachments/assets/558f1dd6-fe3e-4737-9404-e6bc26424e2c)
30 → 60 TPS 전환에서도 목표 포즈가 계단식으로 바뀌지 않고  
시간에 따라 연속적으로 변화하게 되어,  
이전처럼 화면에 미세한 떨림이 발생하지 않게 되었습니다.  

TPS가 서로 다른 애니메이션 클립을 섞어도  
자연스러운 블렌딩 품질을 유지할 수 있게 되었습니다.

- 배운 점  

미세한 떨림처럼 숫자로는 잡기 어려운 문제도  
Unity에서 봤던 애니메이션 키프레임/보간 구조 경험을 떠올리며  
원인에 빨리 도달할 수 있었습니다.  

단순 데이터 값만 보는 것이 아니라,  
이미 알고 있는 구조·툴 이해를 함께 활용하는 게  
트러블슈팅에 큰 힘이 된다는 걸 느꼈습니다.  

앞으로도 이런 식으로 구조를 먼저 이해하고,  
그 위에 경험을 쌓아 가며 문제를 해결하는 태도를 유지하고자 합니다.


# 📘핵심 주요 코드
- [전체 코드 (EntryPoint / Framework / Game – Actor 파생 클래스 구현)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio)
- [코어 (Core – 엔진 기반 시스템, 실행 구조)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio/Framework/Core)
- [에디터 (Editor – ImGui 에디터, WorldOutliner, Inspector)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio/Framework/Editor)
- [Gameplay (Controller / Pawn 등 플레이 로직)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio/Framework/Gameplay)
- [컴포넌트 (Scene / Primitive / Render Components)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio/Framework/Components)
- [액터들 (Game/Actors – 플레이어, 몬스터 등)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio/Game/Actors)
- [자원 관리 매니저 (CPU/GPU/애니메이션 리소스 매니저)](https://github.com/Myoungcholho/DX_Portfolio/tree/main/DX11Portfolio/Framework/Manager)
