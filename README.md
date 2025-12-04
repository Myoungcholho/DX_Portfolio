# 📌프로젝트 개요
<p align="center">
  <img
    width="800"
    src="https://github.com/user-attachments/assets/dffc2176-d743-4aae-90b8-446c3935ae4a"
    alt="DirectX 11 Engine Editor Screenshot" />
</p>

WinMain부터 DirectX11 기반의 자체 게임 엔진을 바닥부터 설계 및 구현했습니다.  
엔진 아키텍처 구축 과정에서 언리얼 엔진의 내부 구조를 분석해 핵심 개념을 이해하고 이를 기반으로 저만의 방식으로 엔진을 재구성했습니다.

또한 다양한 렌더링 기법 적용을 위해 셰이더를 직접 설계하고 구현했으며  
CPU/GPU 병목 구간을 분석하기 위한 프로파일러 개발과 더불어 GameThread/RenderingThread 분리, 인스턴싱 등 엔진 최적화도 성공적으로 수행했습니다.

| 항목 | 내용 |
|------|------|
| 📹 소개 영상 | [📎포트폴리오 영상](https://www.youtube.com/watch?v=CVj2y5PXsnw) |
| 🕒 개발 기간 | 2025.05 ~ 2025.10 (156일) |
| 👤 개발 인원 | 1명 |
| 🧰 개발 환경 | C++ |
| 🛠 실행 및 디버깅 툴 | VS2022, RenderDoc, Deleaker, IBLMaker |
| 📦 라이브러리 | DX11, ImGuiDocking, DirectXTex, Assimp, SimpleMath |

</br>

# 📘목차
- [구현 요약 내용](#구현-요약-내용)
- [문제 해결 경험(트러블 슈팅)](#문제-해결-경험트러블-슈팅)
- [프로젝트에서 얻은 것](#프로젝트에서-얻은-것)
- [개발 계기](#개발-계기)
- [구현 상세 내용](#구현-상세-내용)
- [핵심 주요 코드](#핵심-주요-코드)

</br>

# 📘구현 요약 내용
| 상세 설명 링크 | 구현 요약 |
|----------------------|------------------|
| [🧱 Core Architecture](#core) | 1. GameThread / RenderThread </br> 2. 델리게이트 이벤트 시스템 |
| [🌍 World / Object](#world) | 1. 언리얼 구조 분석하여 유사한 아키텍처 구조로 설계|
| [🎨 Rendering](#rendering) | 1. 렌더링 파이프라인 구현 </br> 2. PBR, MSAA, Lod, Fog, PCF 그림자, ToneMapping 등 다양한 그래픽스 기법 적용 |
| [🕺 Animation](#animation) | 1. 거리 기반 애니메이션 최적화 </br> 2. 애니메이션 보간 및 셰이더 작성 AnimInstance 설계 |
| [📦 Asset](#asset) | 1. fbx 파일 임포트 시스템 </br> 2. 파일 관리 시스템 |
| [🛠 Editor](#editor) | 1. Outliner, Inspector, SceneView 등 에디터 위젯 설계 </br> 2. 명령 큐 시스템을 구축하여 에디터 조작 안정화 |
| [📊 Profiling](#profiling) | 1. 구간별 CPU 사용량 및 레이턴시 프로파일링 시스템 구축 </br> 2. CPU/GPU 타임스탬프 시스템 |

</br>

# 📘핵심 주요 코드

| 코드 파일 | 코드 설명 |
|----------|-----------|
| RenderManager [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Core/URenderManager.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Core/URenderManager.cpp) | GameThread에서 제출된 스냅샷을 수신해 RenderThread에서 렌더 순서를 분류하고 제어하는 렌더링 허브입니다.|
| USceneComponent [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/USceneComponent.cpp) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/USceneComponent.cpp) | Actor의 부모-자식 계층과 Local/World Transform을 관리하는 베이스 컴포넌트 </br> Attach / Detach 로직을 수행하고 있습니다. |
| UAnimInstance [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UAnimInstance.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UAnimInstance.cpp) | 애니메이션 클립의 재생 시간과 상태를 관리하고, 블렌딩을 처리해 본 팔레트에 쓸 로컬 포즈를 만들어주는 실행기입니다. |
| UWorld [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UWorld.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UWorld.cpp) | 액터를 생성·소멸·갱신하며, 액터들의 전체 생명주기(Lifecycle)를 관리하는 클래스입니다. |
| APawnTest [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Game/Actors/APawnTest.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Game/Actors/APawnTest.cpp) | 게임 모듈에서 사용되는 APawn 기반 플레이어 테스트용 액터 클래스입니다. |

# 📘문제 해결 경험(트러블 슈팅)

<table style="border-collapse:collapse;">
  <tr>
    <th width="350" style="border:2px solid #ffb3b3; background:#ffe1e1;">
      📂 Editor-Game 간 RaceCondition 문제 해결
    </th>
    <th width="350" style="border:2px solid #ffd27f; background:#fff1d6;">
      📚 공유 자원으로 인한 불필요한 복사 해결
    </th>
    <th width="350" style="border:2px solid #c3c3ff; background:#e9e9ff;">
      🧾 CPU/GPU 병목 해결
    </th>
  </tr>

  <tr>
    <td width="350" style="border:2px solid #ffb3b3; background:#ffe1e1; vertical-align:top;">
      Editor–GameThread 간 RaceCondition이 발생해 CommandQueue 기반 구조로 전환하여 Lock 최소화 방식으로 해결했습니다.
      <br><br>
      <a href="#t0">[상세설명]</a>
    </td>
    <td width="350" style="border:2px solid #ffd27f; background:#fff1d6; vertical-align:top;">
      인스턴스마다 자원을 중복 로드해 발생하던 지연을 자원 관리 매니저 도입으로 해결했습니다.
      <br><br>
      <a href="#t1">[상세설명]</a>
    </td>
    <td width="350" style="border:2px solid #c3c3ff; background:#e9e9ff; vertical-align:top;">
      대규모 씬 렌더링 환경에서 발생한 프레임 저하를 인스턴싱 및 거리 기반 Tick 최적화로 해결했습니다.
      <br><br>
      <a href="#t2">[상세설명]</a>
    </td>
  </tr>
</table>

<br>

<table style="border-collapse:collapse;">
  <tr>
    <th width="350" style="border:2px solid #a8ddff; background:#e6f6ff;">
      📘 프로파일러 이상 징후 해결
    </th>
    <th width="350" style="border:2px solid #c8ffa8; background:#ebffdf;">
      📂 애니메이션 샘플링 문제 해결
    </th>
  </tr>

  <tr>
    <td width="350" style="border:2px solid #a8ddff; background:#e6f6ff; vertical-align:top;">
      GPU/CPU 타임이 비정상적으로 동일하게 측정되는 문제를 원인을 찾아 문제를 해결했습니다.
      <br><br>
      <a href="#t3">[상세설명]</a>
    </td>
    <td width="350" style="border:2px solid #c8ffa8; background:#ebffdf; vertical-align:top;">
      TPS가 다른 애니메이션을 블렌딩할 때 발생한 미세 떨림 현상을
      시간 기반 보간 방식 적용으로 해결했습니다.
      <br><br>
      <a href="#t4">[상세설명]</a>
    </td>
  </tr>
</table>

---

## 1. Editor–GameThread 간 데이터 충돌 해결 구조 설계 <a id="t0"></a>

<table>
  <tr>
    <td style="border:2px solid #4fa3ff; border-radius:8px; padding:12px 16px; background:#050812;">
      <strong>🧩 문제</strong>
      <ul>
        <li>Editor에서 Component 값 갱신 시 비정상 참조 문제가 발생</li>
      </ul>
      <strong>🔍 원인 분석</strong>
      <ul>
        <li>Component를 여러 스레드가 직접 쓰는 구조로 레이스 컨디션 문제</li>
      </ul>
      <strong>🛠 해결</strong><br>
      <img width="446" height="284" alt="image"
           src="https://github.com/user-attachments/assets/6e4cf646-bf4e-48f0-a4a5-95b4a2215af8" />
      <ul>
        <li>Editor 변경 내용을 Queue에 등록</li>
        <li>GameThread가 매 프레임 시작에 Queue를 비우며 변경 내용을 반영</li>
      </ul>
      <strong>✅ 결과</strong><br>
      <ul>
        <li>쓰기 경로 단일화로 인해 레이스 컨디션 제거</li>
        <li>에디터 수정값은 최대 1프레임 지연으로 안정적 반영</li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li>레이스 컨디션을 피하려면 데이터를 실제로 바꾸는 ‘권위 스레드’를 한 명으로 정해야 한다는 점</li>
        <li>프레임 스톨을 줄이기 위해 락을 최소화하는 구조를 항상 고민해야 한다는 점</li>
      </ul>
    </td>
  </tr>
</table>

---

## 2. Asset 공유 구조 적용 – 중복 로딩 제거로 로딩 병목 해결 <a id="t1"></a>

<table>
  <tr>
    <td style="border:2px solid #ffd27f; border-radius:8px; padding:12px 16px; background:#120d05;">
      <strong>🧩 문제</strong>
      <ul>
        <li>동일 모델을 여러 개 배치할수록 로드 시간이 과도하게 늘어나는 문제가 발생</li>
      </ul>
      <strong>🔍 원인 분석</strong>
      <ul>
        <li>각 컴포넌트가 동일한 모델 데이터를 각각 별도로 소유하는 것이 문제</li>
        <li>인스턴스별로 CPU/GPU 자원을 공유 가능한 자원임에도 중복 생성하는 것이 문제</li>
      </ul>
      <strong>🛠 해결</strong><br>
      <img width="634" height="53"
           alt="image"
           src="https://github.com/user-attachments/assets/4a82bd85-39c7-4a24-8d97-611f045c48d0" />
      <ul>
        <li>한 번 로드한 파일은 다시 읽지 않고 캐시에서 재사용하도록 변경</li>
        <li>로드된 자산은 공유참조로만 쓰게 해 불필요한 중복 소유를 제거</li>
      </ul>
      <strong>✅ 결과</strong><br>
      <img width="531" height="134"
           alt="image"
           src="https://github.com/user-attachments/assets/ee06bffb-a560-4634-ae03-78f46dbfbaa7" /><br>
      <img width="531" height="134"
           alt="image"
           src="https://github.com/user-attachments/assets/ad32d436-2fc2-4a0f-8c0d-9400bafe5ddb" />
      <ul>
        <li>GPU : 54초 → 8초 (약 85.18% 단축)</li>
        <li>CPU : 0.02초 → 0.00006초 (약 99.7% 단축)</li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li>자원이 공유 가능한 대상인지 먼저 고민하고, 불필요한 작업을 줄여 속도를 올리는 것이 중요하다는 점</li>
        <li>캐시해서 들고 있는 것도 중요하지만, 메모리 사용량을 고려해 쓰지 않는 자원은 제거하는 구조도 필요하다는 점</li>
        <li>디자인 패턴 서적처럼, 꾸준한 학습이 있어야 실제 상황에서 개념을 떠올리고 적용까지 시도해 볼 수 있다는 점</li>
      </ul>
    </td>
  </tr>
</table>

---

## 3. 인스턴싱 도입 전 Static / Skeletal 프레임 저하 원인 파악 <a id="t2"></a>

<table>
  <tr>
    <td style="border:2px solid #a8ddff; border-radius:8px; padding:12px 16px; background:#050a12;">
      <strong>🧩 문제</strong><br>
      <img width="362" height="121" alt="image"
           src="https://github.com/user-attachments/assets/588c3f09-b8c8-4022-b4ba-7514054077bf" />
      <ul>
        <li>동일 스켈탈 애니메이션 모델 100개 렌더링 시 FPS가 약 40.9까지 하락함</li>
      </ul>
      <strong>🔍 원인 분석</strong><br>
      <img width="362" height="121" alt="image"
           src="https://github.com/user-attachments/assets/f5905d9b-ad01-400a-b7eb-f821da3a4b1b" />
      <ul>
        <li>캐릭터 1개당 애니메이션·본 팔레트 계산에 약 0.15ms, 100개 기준 약 15.2ms가 소모되어 CPU 병목이었다는 점</li>
        <li>동일 모델 100개를 각각 별도 DrawIndexed로 그려 GPU 드로우콜 수가 과도하게 많았다는 점</li>
      </ul>
      <strong>🛠 해결</strong>
      <ul>
        <li>애니메이션 계산을 FixedUpdate에서 Tick/URO 방식으로 전환해, 모든 프레임을 정밀 계산하지 않고 보간·보정으로 CPU 본 팔레트 계산량을 줄임</li>
        <li>동일 모델을 인스턴싱으로 묶어 그리도록 변경해, 개별 DrawIndexed 호출을 통합하고 GPU 드로우콜 병목을 완화</li>
      </ul>
      <strong>✅ 결과</strong><br>
      <img width="362" height="121" alt="image"
           src="https://github.com/user-attachments/assets/33dad103-e126-43b6-9dae-20134c2f2887" />
      <ul>
        <li>40.9 FPS → 112.1 FPS로, 약 174% 수준의 프레임 향상을 달성</li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li>프레임 저하는 CPU·GPU 중 한쪽만의 문제가 아니라 두 축을 나눠 각각 병목을 찾아야 한다는 점</li>
        <li>감이 아니라 프로파일링 수치로 병목을 검증하는 과정이 프로그래머에게 필수적이라는 점</li>
        <li>모든 캐릭터를 매 프레임 완전 계산하기보다, 품질을 유지하는 선에서 연산량을 줄이는 최적화 설계가 중요하다는 점</li>
      </ul>
    </td>
  </tr>
</table>

---

## 4. CPU·GPU 타임 비례 이상 징후 포착 <a id="t3"></a>

<table>
  <tr>
    <td style="border:2px solid #4fa3ff; border-radius:8px; padding:12px 16px; background:#050812;">
      <strong>🧩 문제</strong><br>
      <img width="461" height="110" alt="image"
           src="https://github.com/user-attachments/assets/bef2c646-d665-4921-985f-aee132da5e4c" />
      <ul>
        <li>CPU(Game) / GPU 프레임 타임을 시각화했을 때, 두 값이 거의 비례해 같이 움직이는 이상 징후가 있었다는 점</li>
      </ul>
      <strong>🔍 원인 분석</strong>
      <ul>
        <li>제출 인스턴스 수와 드로우콜 구조는 동일한 상태에서 CPU 부하를 줄였는데 GPU 시간도 함께 감소했다는 점</li>
        <li>이는 실제 GPU 부하 감소라기보다, GPU 타이밍 측정 구간에 문제가 있을 가능성을 의심</li>
      </ul>
      <img width="461" height="110" alt="image"
           src="https://github.com/user-attachments/assets/55675cba-4be9-4d23-a658-b08ff153b109" />
      <ul>
        <li>렌더링 타임라인을 다시 점검해 보니 GPU 타임스탬프를 Present() 이후에 찍고 있어, 프레임 페이싱 대기 시간이 GPU 시간에 섞여 있었다는 점</li>
      </ul>
      <strong>🛠 해결</strong>
      <ul>
        <li>GPU 실제 렌더링 시간만 측정하도록, Present() 호출 직전에 GPU 타임스탬프를 찍도록 측정 위치를 조정</li>
      </ul>
      <strong>✅ 결과</strong><br>
      <img width="461" height="110" alt="image"
           src="https://github.com/user-attachments/assets/13e03291-599e-4167-9052-b5f25e53a587" />
      <ul>
        <li>수정 후 CPU(Game) ≒ 50ms, CPU(Render) ≒ 14ms, GPU ≒ 26ms로 분리되어 측정되면서, CPU·GPU 시간이 비례하던 왜곡이 사라짐</li>
        <li>실제 병목이 GPU가 아니라 게임 로직·애니메이션 계산이 있는 CPU 쪽이라는 것을 명확히 확인할 수 있었음</li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li>렌더링처럼 눈에 보이는 것이 아닌 수치·시간 같은 지표는, 항상 측정 방식이 합리적인지 먼저 의심해야 한다는 점</li>
        <li>해결책부터 찾기보다 원인 중심으로 분석하는 습관이 동작 원리를 훨씬 깊게 이해하게 만든다는 점</li>
      </ul>
    </td>
  </tr>
</table>

---

## 5. 정수 기반 샘플링의 한계 – 블렌딩 시 덜덜거림 발생 <a id="t4"></a>

<table>
  <tr>
    <td style="border:2px solid #c8ffa8; border-radius:8px; padding:12px 16px; background:#060f06;">
      <strong>🧩 문제</strong>
      <ul>
        <li>애니메이션 30 TPS → 60 TPS 블렌딩 시 포즈가 계단식으로 변하며 화면에 미세한 덜덜거림 발생</li>
      </ul>
      <strong>🔍 원인 분석</strong><br>
      <img width="570" height="315" alt="image"
           src="https://github.com/user-attachments/assets/2c913312-c086-4f49-b874-af657b883661" />
      <ul>
        <li>int 기반 정수 샘플링으로 키프레임만 점프하며 갱신해 TPS가 다른 클립 블렌딩 시 시간 축 불연속성이 그대로 반영됨</li>
      </ul>
      <strong>🛠 해결</strong><br>
      <img width="570" height="177" alt="image"
           src="https://github.com/user-attachments/assets/424df55e-9547-4764-962b-124a2df4e573" />
      <ul>
        <li>두 애니메이션의 시간 축을 맞춰, 키 프레임 사이 값을 보간해 자연스럽게 연결</li>
      </ul>
      <strong>✅ 결과</strong><br>
      <img alt="Animation"
           src="https://github.com/user-attachments/assets/558f1dd6-fe3e-4737-9404-e6bc26424e2c" />
      <ul>
        <li>덜덜거림 제거, TPS가 다른 애니도 자연스러운 전환 유지</li>
      </ul>
      <strong>📚 배운 점</strong>
      <ul>
        <li>애니메이션 블렌딩의 본질이 두 애니메이션의 시간축을 맞추고 그 사이를 보간하는 것이라는 점</li>
        <li>시각화를 통해서만 발견할 수 있는 문제가 있으며, 검증과 테스트는 중요하다는 점</li>
      </ul>
    </td>
  </tr>
</table>

</br>

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

</br>

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

</br>

# 📘구현 상세 내용 
<h3 id="core">1. Core Architecture </h3>

- GameThread/RednerThread 분리 아키텍처 (MailBox 기반 스냅샷 소비 모델)
- `std::function` 기반 델리게이트
- Editor → GameThread 작업 전달용 Command Queue로 RaceCondition 제거 & Lock 스톨 최소화

---

<h3 id="world">2. World / Object </h3>

- Transform 시스템 (월드/로컬 변환 및 위치 정보 관리)
- GameInstance / UWorld / SceneComponent / PrimitiveComponent / RenderProxy 구조
- AActor / StaticMeshComponent / SkeletalMeshComponent 구성
- Pawn / Controller / GameMode 계층
- ClassID 기반 Actor 런타임 스폰 팩토리 (리플렉션 없이 타입 등록/생성)
- 마우스·키보드 전역 입력 시스템

---

<h3 id="rendering">3. Rendering </h3>

- 그래픽 파이프라인 상태 집합 객체(PSO) 설계  
  (Shader·Blend·Rasterizer·Depth 상태 일체 관리)
- RenderManager(패스 구성) / DrawBatch(오브젝트 유형별 배치·정렬) / Renderer(드로우 호출) 구조
- HDR RenderTarget 파이프라인, MSAA, ToneMapping
- Fog / Bloom / Shadow / PBR 라이팅

---

<h3 id="animation">4. Animation </h3>

- 애니메이션 재생 시스템 (AnimationClip 기반 재생·루프·이동 관리)
- AnimInstance (UAnimInstance와 유사한 상태·블렌딩 로직 전담 계층)
- 애니메이션 블렌딩 (키프레임 보간 기반 블렌딩)
- 카메라 거리 기반 Tick 최적화 (Update Rate Optimization)

---

<h3 id="asset">5. Asset </h3>

- .fbx 파일 로드 파이프라인 (메시·본·애니메이션 데이터 파싱)
- CPU / GPU / 애니메이션 자원 공유 관리 클래스

---

<h3 id="editor">6. Editor </h3>

- ImGui Docking 기반 에디터 UI
- WorldOutliner / ActorOutliner / InspectorWindow를 통한 런타임 액터/컴포넌트 편집

---

<h3 id="profiling">7. Profiling </h3>

- 구역 단위 실행 시간 측정 & 파일 저장 프로파일러
- CPU/GPU 프레임 타임 측정 런타임 시스템
