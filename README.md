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
- [구현 요약 내용](#구현-요약-내용-목차-이동)
- [핵심 주요 코드](#핵심-주요-코드-목차-이동)
- [문제 해결 경험(트러블 슈팅)](#문제-해결-경험트러블-슈팅-목차-이동)
- [프로젝트에서 얻은 것](#프로젝트에서-얻은-것-목차-이동)
- [개발 계기](#개발-계기-목차-이동)
- [구현 상세 내용](#구현-상세-내용-목차-이동)

</br>

# 📘구현 요약 내용 [(목차 이동)](#목차)
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

# 📘핵심 주요 코드 [(목차 이동)](#목차)

| 코드 파일 | 코드 설명 |
|----------|-----------|
| RenderManager [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Core/URenderManager.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Core/URenderManager.cpp) | GameThread에서 제출된 스냅샷을 수신해 RenderThread에서 렌더 순서를 분류하고 제어하는 렌더링 허브입니다.|
| USceneComponent [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/USceneComponent.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/USceneComponent.cpp) | Actor의 부모-자식 계층과 Local/World Transform을 관리하는 베이스 컴포넌트 </br> Attach / Detach 로직을 수행하고 있습니다. |
| UAnimInstance [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UAnimInstance.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UAnimInstance.cpp) | 애니메이션 클립의 재생 시간과 상태를 관리하고, 블렌딩을 처리해 본 팔레트에 쓸 로컬 포즈를 만들어주는 실행기입니다. |
| UWorld [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UWorld.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Framework/Components/UWorld.cpp) | 액터를 생성·소멸·갱신하며, 액터들의 전체 생명주기(Lifecycle)를 관리하는 클래스입니다. |
| APawnTest [.h](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Game/Actors/APawnTest.h) / [.cpp](https://github.com/Myoungcholho/DX_Portfolio/blob/main/DX11Portfolio/Game/Actors/APawnTest.cpp) | 게임 모듈에서 사용되는 APawn 기반 플레이어 테스트용 액터 클래스입니다. |

</br>

# 📘문제 해결 경험(트러블 슈팅) [(목차 이동)](#목차)

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

## 1. Editor–GameThread 간 데이터 충돌 해결 구조 설계 <a id="t0"></a> [(표로 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

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

## 2. Asset 공유 구조 적용 – 중복 로딩 제거로 로딩 병목 해결 <a id="t1"></a> [(표로 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

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

## 3. 인스턴싱 도입 전 Static / Skeletal 프레임 저하 원인 파악 <a id="t2"></a> [(표로 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

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

## 4. CPU·GPU 타임 비례 이상 징후 포착 <a id="t3"></a> [(표로 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

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

## 5. 정수 기반 샘플링의 한계 – 블렌딩 시 덜덜거림 발생 <a id="t4"></a> [(표로 이동)](#문제-해결-경험트러블-슈팅-목차-이동)

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

# 📘프로젝트에서 얻은 것 [(목차 이동)](#목차)

| 번호 | 얻은 경험 |
|------|-----------|
| 1 | [드로우콜·렌더링 파이프라인 감각](#gain-drawcall) |
| 2 | [언리얼 아키텍처 이해와 재구현 경험](#gain-ue-arch) |
| 3 | [C++ 자원 관리와 소유권 감각](#gain-cpp-resource) |
| 4 | [게임 스레드 / 렌더 스레드 분리·멀티스레드 이해](#gain-gt-rt) |
| 5 | [행렬·계층 구조 기반 씬/애니메이션 처리 경험](#gain-matrix) |
| 6 | [쉐이딩 모델·라이팅 이해](#gain-shading) |
| 7 | [Transform·회전·짐벌락을 다루며 쌓은 수학적 감각](#gain-rotation) |

---

### 1. 드로우콜 렌더링 파이프라인 감각 <a id="gain-drawcall"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

도형을 직접 그려 보고, 하나하나 드로우콜을 날려 보면서 물체를 화면에 그리기 위해 어떤 단계들이 필요하고,  
그 과정에서 어느 정도 자원이 소모되는지 체감할 수 있었습니다.

---

### 2. 언리얼 아키텍처에 대한 이해와 재구현 경험 <a id="gain-ue-arch"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

언리얼 아키텍처를 내 엔진으로 옮겨 구현해 보면서 Actor / Component / SceneComponent / Controller 등의 계층 구조와 책임 분리,  
그리고 언리얼이 어떤 방식으로 업데이트와 최적화를 조직하는지 더 깊게 이해하게 되었습니다.

---

### 3. C++ 자원 관리와 소유권에 대한 감각 <a id="gain-cpp-resource"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

엔진 레벨에서 객체 생명주기와 소유권을 직접 관리하다 보니 `std::move`, `std::forward`를 사용해야 하는 이유와 RAII / 스마트 포인터를 활용한 효율적인 자원 관리 방법을 몸으로 익힐 수 있었습니다.  

SpawnActor와 유사한 흐름을 구현하면서 팩토리 패턴, 가변 인자, perfect forwarding 같은 언어 레벨 기법을 실제 설계에 적용해 볼 수 있었던 점도 컸습니다.

---

### 4. 게임 스레드 / 렌더 스레드 분리와 멀티스레드 이해 <a id="gain-gt-rt"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

렌더 스레드와 게임 스레드를 분리해 동작시키는 과정에서 직접 레이스 컨디션과 동기화 문제를 마주하고 해결해 보며,  
멀티스레드 환경에서 데이터를 안전하게 다루는 방법과 패턴에 대한 이해를 넓힐 수 있었습니다.

---

### 5. 행렬·계층 구조를 활용한 씬/애니메이션 처리 경험 <a id="gain-matrix"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

컨텐츠 레벨에서는 잘 드러나지 않던 행렬 연산들이 SceneComponent의 부모–자식 계층, 스켈레탈 본 트리 계산, WVP 변환 같은 부분에서 어떻게 쓰이는지 직접 구현해 보았습니다.  

이 과정에서 선형(행렬)·비선형(회전) 변환, 그리고 SIMD가 주는 이점을 실제 코드와 함께 이해하게 되었습니다.

---

### 6. 쉐이딩 모델과 라이팅에 대한 이해 <a id="gain-shading"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

Phong 쉐이딩부터 Blinn-Phong, PBR 모델까지 직접 구현해 보면서 게임 화면의 표면이 어떤 라이팅 연산을 수행하는지,  
하이라이트·반사·러프니스 같은 개념이 수식과 결과 이미지에서 어떻게 연결되는지 이해할 수 있었습니다.

---

### 7. Transform, 회전, 짐벌락을 다루며 쌓은 수학적 감각 <a id="gain-rotation"></a> [(⬆표로 이동)](#프로젝트에서-얻은-것-목차-이동)

Transform 시스템을 구현하고 에디터에서 즉시 반영되는 구조를 만들면서 잘못된 회전 적용, 축 고정, 짐벌락 같은 문제들을 직접 마주했습니다.  

Quat ↔ Euler 변환을 수학적으로 다뤄 보며 회전 표현에 대한 직관과 문제 해결 능력을 키울 수 있었습니다.

</br>

# 📘개발 계기 [(목차 이동)](#목차)
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

# 📘구현 상세 내용 [(목차 이동)](#목차)
<h3 id="core">1. Core Architecture </h3>

<details>
  <summary>GameThread/RenderThread 분리 아키텍처 (MailBox 기반 DoubleBuffer 스냅샷 소비 모델)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 설계 의도</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>수직 동기화(Present 대기)로 인해 렌더링과 게임 로직이 같은 사이클에 묶이는 문제를 해결하고자 했습니다.</li>
          <li>렌더링 스톨이 게임 로직 실행을 지연시키지 않도록 스레드 분리를 설계했습니다.</li>
        </ul>
        <b>🗺️ 구조 구성</b><br/>
        <img width="520" alt="flow" src="https://github.com/user-attachments/assets/e685b1d4-5d78-4f23-bb8f-4670ab0ab85e" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>GameThread는 렌더링에 필요한 데이터를 스냅샷 형태로 생성합니다.</li>
          <li>생성된 스냅샷은 MailBox에 발행됩니다.</li>
          <li>RenderThread는 MailBox에서 가장 최신 스냅샷 하나만 가져와 소비합니다.</li>
          <li>Double Buffer 구조로 스냅샷 생성과 소비를 분리했습니다.</li>
        </ul>
        <b>🔍 검증 </b><br/>
        <img width="520" alt="evidence" src="https://github.com/user-attachments/assets/b4259807-6562-4f9b-b5a3-a67ad2aedfa5" /><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>기존 구조에서는 렌더링 스톨로 인해 게임 로직 호출 빈도도 함께 낮아졌습니다.</li>
          <li>스레드 분리 이후에는 렌더링과 무관하게 게임 로직 업데이트가 안정적으로 수행됩니다.</li>
          <li>게임 로직의 실행 빈도와 시간 안정성이 눈에 띄게 개선되었습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>델리게이트 시스템</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경 </b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>함수 포인터 방식은 사용성이 떨어져, 게임 모듈에서 쓰기 쉬운 이벤트 시스템이 필요했습니다.</li>
          <li>이에 따라 델리게이트(이벤트) 시스템을 직접 구현했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="237" height="38" alt="image" src="https://github.com/user-attachments/assets/5dab9a3f-fac3-448f-983c-7444157a6d3f" />
        <img width="575" height="57" alt="image" src="https://github.com/user-attachments/assets/8bb6369d-a2e4-40e3-9b69-1efb60b05495" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>엔진에서는 델리게이트를 바인딩한 뒤, Broadcast로 이벤트를 전파해 사용합니다.</li>
        </ul>
        <b>🧠 구현 포인 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li><code>std::function</code>(람다 캡처) 방식과 <code>Stub + void*</code>(type erasure) 방식을 비교했습니다.</li>
          <li>현재 프로젝트 단계에서는 구현 속도와 디버깅 편의성이 더 중요하다고 판단했습니다.</li>
          <li>이에 따라 람다 기반 방식을 우선 적용했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>


<details>
  <summary>Editor → GameThread 작업 전달용 Command Queue로 RaceCondition 제거 & Lock 스톨 최소화</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <img width="572" height="267" alt="image" src="https://github.com/user-attachments/assets/221325ea-2590-4400-8434-552333d7c713" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>에디터에서 액터 값을 수정할 때, GameThread가 동시에 접근해 레이스 컨디션이 발생했습니다.</li>
          <li>스레드 간 안전하게 변경 사항을 전달할 구조가 필요하다고 판단했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="949" height="234" alt="image" src="https://github.com/user-attachments/assets/ac4dd3be-f6df-4c09-9d1f-3c0949a9e595" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>에디터는 N 프레임에 변경값을 명령(Command) 형태로 큐에 등록합니다.</li>
          <li>GameThread는 N+1 프레임에 큐를 소비해 Actor에 반영합니다.</li>
          <li>Editor ↔ GameThread 간 직접 접근을 차단하고, 단방향 전달 구조로 설계했습니다.</li>
        </ul>
        <b>🧠 설계 고민 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>초기에는 에디터 값 변경마다 Lock 기반 동기화를 고려했습니다.</li>
          <li>하지만 변경 빈도가 높아 GameThread 스톨 위험이 크다고 판단했습니다.</li>
          <li>이에 따라 변경값을 누적한 뒤, 게임 로직 시작 시점에 일괄 적용하는 방식으로 전환했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

---

<h3 id="world">2. World / Object </h3>

<details>
  <summary>Transform 시스템 (월드/로컬 변환 및 위치 정보 관리)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>오브젝트의 위치·회전·스케일을 한 곳에서 책임지고 일관되게 관리하기 위해 설계했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="533" height="125" alt="image" src="https://github.com/user-attachments/assets/0027499a-d783-4129-a16d-651c682d99ec" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>Transform은 Euler와 Quaternion을 함께 보관합니다.</li>
          <li>Euler는 에디터에서의 직관적인 수정·노출을 위해 사용합니다.</li>
          <li>Quaternion은 런타임 및 애니메이션 보간의 안정성을 위해 사용합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <img width="781" height="159" alt="image" src="https://github.com/user-attachments/assets/2ab08265-ad51-4901-a207-4f3dc42ff6a8" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>에디터 편집 시에는 Euler → Quaternion 단방향 갱신만 수행해 불필요한 재계산을 피했습니다.</li>
        </ul>
        <img width="540" height="119" alt="image" src="https://github.com/user-attachments/assets/ea4fe2ca-1387-49fb-8388-ba18cac79431" /><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>Quaternion → Euler 변환 시에는 −360 / 0 / +360 조합의 27개 후보 중, 이전 값과 가장 가까운 Euler를 선택해 회전 튐을 방지했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary> UWorld 클래스 </summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>월드 단위 객체들의 수명 관리를 일관되게 책임지기 위해 설계했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="226" height="266" alt="image" src="https://github.com/user-attachments/assets/4344c7f3-a93c-469d-b9ec-ab8a26143919" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>UWorld는 월드에 존재하는 Actor의 생성·보관·수명 관리를 담당합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <img width="789" height="307" alt="image" src="https://github.com/user-attachments/assets/0c7e5193-1baf-483e-9379-6f213a0210de" /><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>초기에는 기본 생성 후 Set()으로 데이터를 주입해, 호출 누락 문제가 발생했습니다.</li>
          <li>이를 해결하기 위해 스폰 함수가 가변 인자를 받아 생성 시점에 바로 데이터를 설정하도록 개선했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>


<details>
  <summary> ActorComponent / SceneComponent / PrimitiveComponent / RenderProxy 구조</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>초기에는 모델마다 공통 기능을 반복 구현해야 했습니다.</li>
          <li>이를 해결하기 위해 역할 기반 컴포넌트 계층 구조를 설계했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="245" height="441" alt="image" src="https://github.com/user-attachments/assets/52dbd18d-b001-4f00-ba7f-c7419a05d77a" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>ActorComponent는 어태치 기능을 담당하는 기본 컴포넌트입니다.</li>
          <li>SceneComponent는 부모–자식 계층과 위치·회전·스케일 정보를 관리합니다.</li>
          <li>PrimitiveComponent는 정점·인덱스·머티리얼 등 렌더링 데이터를 관리합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">Transform은 포인터가 아닌 value 타입으로 관리했습니다.</div>
        <img width="488" height="183" alt="image" src="https://github.com/user-attachments/assets/2d8e7baf-afa6-4e68-8cf5-3d45282e247d" /><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>초기에는 포인터 방식을 검토했으나, 성능과 소유권 특성을 고려해 Transform을 value 타입으로 관리하기로 결정했습니다.</li>
          <li>객체가 Transform을 값으로 소유하면, 액터 접근 시 함께 캐시에 적재되어 메모리 접근 효율이 좋아집니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary> Actor 클래스</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>여러 컴포넌트를 하나의 생명 주기와 공간 단위로 묶어, 월드에서 일관되게 관리하기 위한 추상 계층이 필요했기 때문입니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">액터가 삭제될 때</div>
        <img width="792" height="65" alt="image" src="https://github.com/user-attachments/assets/efb40374-7cc2-4d94-aa58-5de83ec4a4b5" /><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>Tick 도중 액터를 즉시 삭제할 경우 댕글링 포인터로 인한 크래시와 미정의 동작이 발생할 수 있어, 삭제 시점을 특히 신중히 설계했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary> StaticMeshComponent / SkinnedMeshComponent / SkeletalMeshComponent 클래스 </summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>컴포넌트를 3단계로 분리한 이유는 메시 컴포넌트의 책임을 명확히 나누기 위함입니다.</li>
          <li>캐릭터가 아닌 경우에는 본 가중치와 같은 데이터가 불필요해, 이를 분리하는 구조가 적합하다고 판단했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="573" height="696" alt="image" src="https://github.com/user-attachments/assets/0bbdd2e0-e7c9-40cc-96b2-c1c5b246c6d8" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>
            <b>StaticMeshComponent</b>  
            정점(Vertex)과 인덱스(Index) 데이터를 기반으로, 캐릭터가 아닌 정적 물체를 렌더링하는 컴포넌트입니다.
          </li>
          <li>
            <b>SkinnedMeshComponent</b>  
            스켈레탈 메시 렌더링을 위한 기본 클래스이며, 본 구조와 본 가중치(Skin Weight) 데이터를 포함하는 추상 컴포넌트입니다.
          </li>
          <li>
            <b>SkeletalMeshComponent</b>  
            SkinnedMeshComponent를 상속한 인스턴스화 가능한 컴포넌트로, UAnimInstance를 통해 애니메이션을 계산하고 스키닝 결과를 렌더링에 반영합니다.
          </li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>초기에는 Static / Skeletal 두 컴포넌트만으로 충분하다고 판단했습니다.</li>
          <li>그러나 스키닝은 필요하지만 애니메이션을 사용하지 않는 케이스가 존재함을 확인했습니다.</li>
          <li>이에 따라 본·스킨 데이터와 같은 스키닝 공통 책임을 Skinned 추상 계층으로 분리했습니다.</li>
          <li>Skeletal은 UAnimInstance 기반 애니메이션 구동 책임만 담당하도록 설계했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>
  
<details>
  <summary>Pawn / Controller / GameMode 계층</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>언리얼과 유사하게 조작 로직과 조작 대상을 분리해 재사용성을 높이기 위해 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="694" height="337" alt="image" src="https://github.com/user-attachments/assets/97f86fd5-84e1-4265-8d29-a93f8ae5946a" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li><b>Pawn</b>: 입력 또는 AI의 대상이 되는 조작 가능한 객체입니다.</li>
          <li><b>Controller</b>: 입력·AI 로직을 담당하며, 필요 시 여러 Pawn에 재사용할 수 있습니다.</li>
          <li><b>GameMode</b>: 게임 시작 시 Pawn과 Controller를 생성하고, 카메라 포커싱·소유 등 초기 흐름을 일관되게 설정합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>초기에는 bUseController를 Controller가 관리했습니다.</li>
          <li>그러나 Pawn마다 회전 반영 기준(Yaw/Pitch 등)이 달라, Controller 소유 방식은 확장성이 떨어졌습니다.</li>
          <li>이에 따라 회전 반영 정책(bUseController)을 Pawn이 소유하도록 변경했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>ClassID 기반 Actor 런타임 스폰 팩토리 (리플렉션 없이 타입 등록/생성)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>GameMode 역시 Actor이므로, 게임 시작 시점에 스폰할 Actor를 런타임에 선택하고 싶었습니다.</li>
          <li>그러나 템플릿 기반 생성은 타입이 컴파일 타임에 고정되어 런타임 선택에 제약이 있었습니다.</li>
          <li>이에 따라 리플렉션 없이 런타임 타입 선택이 가능한 스폰 팩토리를 설계했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="704" height="99" alt="image" src="https://github.com/user-attachments/assets/c96fa3f9-dcf2-4fc3-aa52-2cdd4e057f08" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>매크로로 클래스 타입을 받아 생성 람다를 만들고, 이를 ClassID에 등록합니다.</li>
          <li>이후 문자열 키로 람다를 조회·호출해 액터를 생성합니다.</li>
        </ul>
        <img width="699" height="113" alt="image" src="https://github.com/user-attachments/assets/f74f895b-8e62-4fbe-bfb5-57a79ed3ac12" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>GameMode는 Pawn과 Controller의 구체 타입을 알 필요 없이, 클래스 이름만으로 데이터 기반 객체 생성을 수행합니다.</li>
        </ul>
        <b>🔍 구현 포인트 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>Actor를 if/switch로 생성하면 수가 늘수록 분기 비용이 선형으로 증가합니다.</li>
          <li>이를 ClassID → 생성 함수 매핑 테이블로 대체해, 해시 기반의 상수 시간에 가까운 조회로 개선했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

---

<h3 id="rendering">3. Rendering </h3>

<details>
  <summary>그래픽 파이프라인 상태 집합 객체(PSO) 설계 (Shader·Blend·Rasterizer·Depth 상태 일체 관리)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>오브젝트마다 셰이더와 RasterizerState를 매번 지정하는 과정이 번거로웠습니다.</li>
          <li>이를 해결하기 위해 렌더 상태를 한 번에 설정할 수 있는 PSO(Render State) 객체를 설계했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="593" height="472" alt="image" src="https://github.com/user-attachments/assets/e288f0d8-5e0e-486d-a557-5317e006d391" /><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>DirectX11 렌더 파이프라인 상태(셰이더, InputLayout, Blend·Depth·Rasterizer)를 하나의 GraphicsPSO 객체로 묶어 적용합니다.</li>
        </ul>
        <b>🔍 구현 포인트 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>Primitive가 PSO를 직접 참조하면 표현 모드 확장 시 렌더 정책과 과결합되는 문제가 있었습니다.</li>
          <li>이를 해결하기 위해 Primitive에는 PSO 대신 RenderType(enum)만 두고, 프레임 직전에 타입별로 분류하도록 구성했습니다.</li>
          <li>Draw 단계에서는 Renderer가 그룹 단위로 PSO를 한 번만 전환해, 상태 전환과 드로우콜을 최소화했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>RenderManager(패스 구성) / DrawBatch(오브젝트 유형별 배치·정렬) / Renderer(드로우 호출) 구조</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>Renderer가 패스 구성·순서 제어와 드로우를 모두 담당해, 패스 확장 시 내부 로직 수정이 반복되며 코드가 비대해졌습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <img width="303" height="555" alt="image" src="https://github.com/user-attachments/assets/f356963e-7b12-48ed-84ce-052a41675900" /><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>패스 구성·순서 결정·프레임 데이터 생성을 URenderManager로 분리했습니다.</li>
          <li>URenderManager가 프록시를 RenderType 기준으로 분류해 URenderer에 전달합니다.</li>
          <li>URenderer는 드로우만 담당해, 패스 추가·재배치를 Manager 단일 지점에서 처리할 수 있도록 개선했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>HDR RenderTarget 파이프라인과 PBR, MSAA, ToneMapping</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>PBR 적용을 위해 HDR 파이프라인을 사용했습니다.</li>
          <li>MSAA를 직접 적용·리졸브하며 DX11 렌더링 기술을 이해하고 응용했습니다.</li>
          <li>HDR 결과를 LDR로 변환하기 위해 톤매핑을 적용했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[파이프라인 이미지]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>제 엔진 렌더링 파이프라인은 다음과 같은 흐름으로 동작합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>MSAA 적용으로 인해 리졸브 단계가 필요했습니다.</li>
          <li>MSAA 텍스처는 셰이더에서 직접 샘플링할 수 없어, x1 텍스처로 리졸브해 처리했습니다.</li>
          <li>HDR 색 범위가 0~1을 초과하므로, 최종 출력 전 톤매핑으로 색을 보정했습니다.</li>
          <li>언차티드 2 톤매핑을 분석하며 적용 경험을 쌓았습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>
  
<details>
  <summary>Fog / Bloom / Shadow </summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>렌더링 장면을 보다 현실적으로 표현하기 위한 기법들을 적용해보기 위해 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:14px;">[이미지]</div>
        <b>🔍 구현 포인트 </b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[그 그림자 이미지]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>픽셀 단위 그림자 계산으로 계단 현상이 심하게 발생했습니다.</li>
          <li>이를 완화하기 위해 PCF 3×3 가우시안 필터를 적용해 그림자를 부드럽게 처리했습니다.</li>
        </ul>
        <div style="margin-top:6px; margin-bottom:6px;">[안개 복원 이미지]</div>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>후처리 단계에서는 색 텍스처만 있어 픽셀의 위치 정보를 알 수 없어, view 공간으로 복원해야 했습니다.</li>
          <li>NDC 상태에서 clip.w를 알 수 없어 clip 공간 변환이 어려웠습니다.</li>
          <li>임시로 w = 1을 적용해 InvProjection을 사용했습니다.</li>
          <li>동차좌표는 스케일에 불변이므로, 마지막에 /w를 수행하면 배율이 상쇄되어 올바르게 복원됩니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>


---

<h3 id="animation">4. Animation </h3>

<details>
  <summary>애니메이션 재생 시스템 UAnimInstance (재생,루프,특정 프레임 선택)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>애니메이션 단계에서 메시 행렬까지 생성하면, 메시 구조에 종속되어 재사용성이 떨어졌습니다.</li>
          <li>블렌딩 시 불필요한 연산이 발생하고, 애니메이션 계산 책임이 과도해지는 문제가 있었습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>UAnimInstance는 로컬 포즈 계산만 담당합니다.</li>
          <li>SkeletalMesh는 바인드 포즈(메시 정보)를 사용해 스키닝 행렬을 생성합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[이미지]</div>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>로컬 포즈 단위로 블렌딩해, 포즈 계산을 1회로 제한했습니다.</li>
          <li>UAnimInstance가 메시 정보를 알 필요가 없어 책임이 명확해졌습니다.</li>
          <li>애니메이션 계산 로직을 메시와 분리해 재사용성을 확보했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>애니메이션 블렌딩 (키프레임 보간 기반 블렌딩)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>실제 게임처럼 애니메이션 전환이 부드럽게 이루어지도록 블렌딩을 도입했습니다.</li>
        </ul>
        <b>🗺️ 동작 흐름</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>블렌딩 요청 시, 현재 애니메이션과 대상 애니메이션의 로컬 포즈를 각각 계산합니다.</li>
          <li>경과 시간에 따라 블렌딩 가중치를 산출합니다.</li>
          <li>두 로컬 포즈의 position, rotation, scale을 보간해 출력 포즈를 생성합니다.</li>
          <li>블렌딩 시간이 종료되면 대상 애니메이션으로 전환을 확정합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>정수 프레임 샘플링은 FPS나 애니메이션 길이에 따라 떨림이 발생할 수 있었습니다.</li>
          <li>이를 해결하기 위해 키 프레임 보간 기반 샘플링을 적용했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>카메라 거리 기반 Tick 최적화 (Update Rate Optimization)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>애니메이션 계산량이 많아 CPU 병목이 발생했습니다.</li>
          <li>카메라에서 먼 오브젝트까지 동일한 빈도로 계산할 필요는 없다고 판단했습니다.</li>
        </ul>
        <b>🗺️ 구현 포인트 </b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>카메라 거리 기준으로 애니메이션 Tick 빈도를 단계적으로 조절했습니다.</li>
          <li>중간 거리는 1/2 Tick, 매우 먼 거리는 1/8 Tick만 수행합니다.</li>
          <li>일반 Tick과 분리해, 애니메이션 계산만 선택적으로 줄이도록 구성했습니다.</li>
        </ul>
        <b>🔍 검증</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[이미지]</div>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>평균 FPS가 18.1 → 36.4로 개선되었습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

---

<h3 id="asset">5. Asset </h3>

<details>
  <summary>.fbx 파일 로드 파이프라인 (메시·본·애니메이션 데이터 파싱)</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>외부 툴에서 제작된 메시·본·애니메이션 데이터를 메모리에 적재하기 위해 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>Assimp를 사용해 .fbx 파일을 파싱합니다.</li>
          <li>파싱된 데이터를 엔진 내부에서 사용하는 사용자 정의 구조체로 변환해 저장합니다.</li>
          <li>메시, 본, 애니메이션 데이터를 각각 독립된 로드 흐름으로 처리합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>애니메이션 로드와 메시 로드를 명확히 분리했습니다.</li>
          <li>하나의 메시 데이터를 여러 애니메이션이 공유할 수 있도록 설계했습니다.</li>
          <li>애니메이션 로드 시 메시 데이터를 반복 로딩하지 않아 불필요한 비용을 줄였습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>CPU / GPU / 애니메이션 자원 공유 관리 클래스</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>같은 모델임에도 메시 데이터를 각 인스턴스가 개별로 보유해, 복사 비용이 크게 발생했습니다.</li>
          <li>중복 데이터를 제거하고 자원 사용을 최적화하기 위해 공유 관리 구조를 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[이미지]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>로드된 메시·애니메이션 자원을 에셋 관리 클래스에 등록해 중앙에서 관리합니다.</li>
          <li>자원 조회를 위해 unordered_map 컨테이너를 사용했습니다.</li>
        </ul>
        <b>🔍 구현 포인트 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>파일 경로를 Key로 사용해 자원을 식별합니다.</li>
          <li>이미 등록된 Key가 존재하면 재로드 없이 기존 자원을 재사용합니다.</li>
          <li>이를 통해 디스크 I/O와 메모리 복사 비용을 줄였습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>


---

<h3 id="editor">6. Editor </h3>

<details>
  <summary>ImGui Docking 기반 에디터 UI</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>상용 엔진과 유사한 레이아웃을 구성해, 엔진 사용성과 작업 효율을 높이기 위해 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[이미지]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>ImGui Docking 기능을 사용해 에디터 창을 자유롭게 배치할 수 있도록 구성했습니다.</li>
          <li>SceneView, Inspector 등 각 에디터 패널을 독립적인 Dock 윈도우로 구성했습니다.</li>
        </ul>
        <b>🔍 구현 포인트 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>에디터 UI는 렌더링 파이프라인의 마지막 단계(PostProcess 이후)에 실행됩니다.</li>
          <li>SceneView에 출력할 화면 텍스처를 사용하기 위해, 최종 렌더 결과 이후에 그리도록 설계했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary>WorldOutliner / ActorOutliner / InspectorWindow를 통한 런타임 액터/컴포넌트 편집</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>폴링 방식 대신, 선택 이벤트 기반으로 UI를 갱신해 불필요한 연산을 줄이고자 했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[이미지]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>WorldOutliner, ActorOutliner, InspectorWindow를 독립적인 편집 패널로 구성했습니다.</li>
          <li>액터 또는 컴포넌트 선택 시, 관련 UI 패널이 이벤트를 통해 동기화됩니다.</li>
        </ul>
        <b>🔍 구현 포인트 </b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>UI를 매 프레임 갱신하지 않고, 선택 이벤트 발생 시에만 업데이트하도록 설계했습니다.</li>
          <li>선택 이벤트를 구독한 UI 패널만 갱신해 연산 비용을 최소화했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>


---

<h3 id="profiling">7. Profiling </h3>

<details>
  <summary>구역 단위 실행 시간 측정 & 파일 저장 프로파일러</summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>코드 구간별 실행 시간을 측정해 병목 지점을 파악하기 위해 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[이미지]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>문자열 기반 마킹으로 측정 구간을 정의합니다.</li>
          <li>수집된 시간 데이터를 파싱해 파일로 저장합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>begin/end 호출 순서 오류를 방지하기 위해 예외 처리를 추가했습니다.</li>
          <li>begin–begin, end–end와 같은 잘못된 사용을 감지해 안정성을 확보했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>

<details>
  <summary> CPU/GPU 프레임 타임 측정 런타임 시스템 </summary>
  <br/>
  <table>
    <tr>
      <td>
        <b>🎯 도입 배경</b><br/>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>프로그램 종료 후 로그가 아닌, 런타임 중에도 프레임 타임을 실시간으로 확인하고자 도입했습니다.</li>
        </ul>
        <b>🗺️ 구조/핵심 구성</b><br/>
        <div style="margin-top:6px; margin-bottom:6px;">[코드 사용하는 모습]</div>
        <ul style="margin-top:6px; margin-bottom:14px;">
          <li>begin/end 구간을 마킹해 CPU·GPU 프레임 시간을 측정합니다.</li>
          <li>측정된 구간의 시간 차이를 계산해 런타임 UI로 노출합니다.</li>
        </ul>
        <b>🔍 구현 포인트</b><br/>
        <ul style="margin-top:6px; margin-bottom:0px;">
          <li>GPU 타임 측정을 위해 ID3D11 타임스탬프 쿼리를 사용했습니다.</li>
          <li>GPU는 비동기적으로 동작하므로, 명령 직후가 아닌 2~3프레임 뒤에 결과를 회수하도록 설계했습니다.</li>
        </ul>
      </td>
    </tr>
  </table>
  <br/>
</details>
