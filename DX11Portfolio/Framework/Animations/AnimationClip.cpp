#include "Framework.h"
#include "AnimationClip.h"

void AnimationData::ComposeSkinPalette(const vector<Matrix>& csPose, vector<Matrix>& outPalette) const
{
	const int n = (int)boneParents.size();
	if ((int)outPalette.size() != n)
		outPalette.assign(n, Matrix());

	Matrix defaultInv = defaultTransform.Invert();
	for (int i = 0; i < n; ++i)
	{
		outPalette[i] = defaultInv *
			offsetMatrices[i] *
			csPose[i] *
			defaultTransform;

		outPalette[i] = outPalette[i].Transpose();
	}
}

void AnimationData::EvaluateLocalPose(int clipId, int frame, double accum, vector<AnimationClip::Key>& outLocalPose) const
{
	const AnimationClip& clip = clips[clipId];
	const int boneCount = (int)boneParents.size();

	// 출력 버퍼 크기 보장용
	if ((int)outLocalPose.size() != boneCount)
		outLocalPose.assign(boneCount, AnimationClip::Key());

	const float t = (float)clamp(accum, 0.0, 1.0);

	for (int boneId = 0; boneId < boneCount; ++boneId)
	{
		const vector<AnimationClip::Key>& keys = clip.keys[boneId];
		const int keyCount = (int)keys.size();

		if (keyCount == 0) {
			outLocalPose[boneId] = AnimationClip::Key(); // 바인드/아이덴티티 기본값
			continue;
		}

		// f0: 현재 프레임, f1: 다음 프레임
		// (non-loop을 안전하게 처리하기 위해 마지막에선 f1=f0로 고정)
		const int f0 = clamp(frame, 0, keyCount - 1);
		const int f1 = (f0 + 1 < keyCount) ? (f0 + 1) : f0;

		const AnimationClip::Key& k0 = keys[f0];
		const AnimationClip::Key& k1 = keys[f1];

		AnimationClip::Key out;
		out.pos = Vector3::Lerp(k0.pos, k1.pos, t);
		out.scale = Vector3::Lerp(k0.scale, k1.scale, t);
		out.rot = MathHelper::SlerpSafe(k0.rot, k1.rot, t);

		outLocalPose[boneId] = out;
	}
}

void AnimationData::BuildBonePaletteFromLocal(const vector<AnimationClip::Key>& localPose, int frame, vector<Matrix>& outCSPose, Matrix& accumulatedRootTransform, Vector3& prevPos) const
{
	const int boneCount = (int)boneParents.size();
	if (boneCount <= 0) return;

	if ((int)outCSPose.size() != boneCount)
		outCSPose.assign(boneCount, Matrix());

	for (int boneId = 0; boneId < boneCount; ++boneId)
	{
		const int parentIdx = boneParents[boneId];
		const Matrix parentMatrix = (parentIdx >= 0) ? outCSPose[parentIdx] : accumulatedRootTransform;

		AnimationClip::Key key = (boneId < (int)localPose.size()) ? localPose[boneId] : AnimationClip::Key();

		// 루트 처리(루트모션 누적)
		// 루트 본은 좌표 기준이 없다, 정확히는 없는게 아니라 모델의 월드 기준이다.
		// ㅈ
		if (parentIdx < 0)
		{
			if (frame != 0)
			{
				// 로컬 위치 기준으로 이전 값을 현재 매트릭스에 반영한 것
				accumulatedRootTransform = Matrix::CreateTranslation(key.pos - prevPos) * accumulatedRootTransform;
			}
			else
			{
				// 0프레임일 때는 루트의 높이(Y)는 애니메이션 키의 Y값으로 초기화하고,
				// XZ 평면 이동 누적값은 그대로 유지한다.
				// 단, 점프처럼 Y축 루트모션이 실제 이동을 표현하는 애니메이션에는 이 처리가 부적절할 수 있음.
				Vector3 t = accumulatedRootTransform.Translation();
				t.y = key.pos.y;
				accumulatedRootTransform.Translation(t);
			}

			prevPos = key.pos;
			key.pos = Vector3(0.0f);	// 누적해서 다음 프레임에 사용하는데, 여기서 사용해버리면 1회 + 다음 프레임에 1회 해서 2번 적용되는 문제가 발생한다
		}

		outCSPose[boneId] = key.GetTransform() * parentMatrix;
	}
}

void AnimationData::BuildComponentPoseFromLocal2(const vector<AnimationClip::Key>& localPose, int frame, vector<Matrix>& outCSPose, FRootMotionDelta& outRootMotionDelta, Vector3& prevPos, Quaternion& prevRot, int& prevFrame) const
{
	const int boneCount = (int)boneParents.size();
	if (boneCount <= 0) return;

	if ((int)outCSPose.size() != boneCount)
		outCSPose.assign(boneCount, Matrix());

	// 이번 프레임 루트모션 Δ 초기화
	outRootMotionDelta = FRootMotionDelta{};

	// yaw 언랩(연속화) 헬퍼: 직전 각도 기준으로 -pi..pi 범위로 보정
	auto UnwrapAdd = [](double prev, double d) {
		double raw = prev + d;
		// wrap to [-pi, pi] around prev
		double twoPi = 6.28318530718f;
		double diff = raw - prev;
		while (diff > 3.14159265f) diff -= twoPi;
		while (diff < -3.14159265f) diff += twoPi;
		return prev + diff;
		};

	for (int boneId = 0; boneId < boneCount; ++boneId)
	{
		const int parentIdx = boneParents[boneId];

		const Matrix parentCS = (parentIdx >= 0) ? outCSPose[parentIdx] : Matrix();

		AnimationClip::Key key = (boneId < (int)localPose.size()) ? localPose[boneId] : AnimationClip::Key();

		if (parentIdx < 0)
		{
			// --- helpers: yaw 추출 / yaw 쿼터니언 / yaw 제거(pr만 남김) ---
			auto YawFromQ = [](const Quaternion& q) -> double {
				// yaw(Up/Y)만 라디안으로 반환
				// 참고: roll(X), pitch(Z), yaw(Y) 회전계 기준
				const double siny_cosp = 2.f * (q.w * q.y + q.x * q.z);
				const double cosy_cosp = 1.f - 2.f * (q.y * q.y + q.x * q.x);
				return std::atan2(siny_cosp, cosy_cosp);
				};
			auto YawQuat = [](double yawRad) -> Quaternion {
				// Y축 회전 쿼터니언
				const double h = 0.5f * yawRad;
				return Quaternion(0.f, std::sin(h), 0.f, std::cos(h));
				};
			auto RemoveYaw = [&](const Quaternion& q) -> Quaternion {
				const double yaw = YawFromQ(q);
				Quaternion qYaw = YawQuat(yaw);
				Quaternion invYaw; qYaw.Conjugate(invYaw);
				Quaternion qPR = invYaw * q; // pitch/roll만 남음
				qPR.Normalize();
				return qPR;
				};

			// 프레임 처리
			if (frame == 0) {
				prevPos = key.pos;
				prevRot = key.rot;
				prevFrame = 0;
				outRootMotionDelta = FRootMotionDelta{}; // dPos=(0), dRot=identity
				acc = RootMotionAcc{};
			}
			else {
				if (frame == prevFrame) {
					outRootMotionDelta = FRootMotionDelta{}; // 같은 프레임은 스킵

				}
				else if (frame < prevFrame) {
					// 루프 시작: 기준 재설정
					outRootMotionDelta = FRootMotionDelta{};
					prevPos = key.pos;
					prevRot = key.rot;
					acc = RootMotionAcc{};
					prevFrame = frame;
				}
				else {
					// ΔPos (로컬), ΔRot
					Vector3 deltaPos = key.pos - prevPos; // 로컬
					Quaternion invPrev; prevRot.Conjugate(invPrev);
					Quaternion deltaRot = key.rot * invPrev;
					deltaRot.Normalize();

					// 루트모션은 Yaw만 사용
					const double dYaw = YawFromQ(deltaRot);
					const Quaternion dYawQ = YawQuat(dYaw);

					// 단위 변환만(필요 시): cm→m 등. 기존 0.001f 유지
					const double posScale = 0.001f;

					outRootMotionDelta.dPos = deltaPos * posScale; // 로컬 Δpos
					outRootMotionDelta.dRot = dYawQ;               // Yaw만

					// ---- 누적(디버그) ----
					acc.sumPosL += outRootMotionDelta.dPos;
					acc.sumYawRad = UnwrapAdd(acc.sumYawRad, dYaw);
					acc.count++;

					cout << "Count : " << acc.count << "Frame :" << frame << "\n";
					cout << "Pos : (" << acc.sumPosL.x << "," << acc.sumPosL.y << "," << acc.sumPosL.z << ")" << "\n";
					cout << "Yaw : (" << acc.sumYawRad << ")" << "\n";

					// prev 갱신
					prevPos = key.pos;
					prevRot = key.rot;
					prevFrame = frame;
				}
			}

			// 스켈레톤에는 제자리+Yaw 제거된 회전만 적용 (상체 스윙 보존)
			key.pos = Vector3(0.f, 0.f, 0.f);
			key.rot = RemoveYaw(key.rot);

		}

		outCSPose[boneId] = key.GetTransform() * parentCS;
	}
}

void AnimationData::BuildComponentPoseFromLocal3(
	const std::vector<AnimationClip::Key>& localPose,  int frame, 
	const AnimationClip::Key& rootStart,  const AnimationClip::Key& rootEnd, 
	std::vector<Matrix>& outCSPose,  FRootMotionDelta& outRootMotionDelta, 
	Vector3& prevPos, Quaternion& prevRot,  int& prevFrame, double posScale) const
{
	static double prevYaw = 0.0;

	const int boneCount = (int)boneParents.size();
	if (boneCount <= 0) return;

	if ((int)outCSPose.size() != boneCount)
		outCSPose.assign(boneCount, Matrix());

	outRootMotionDelta = FRootMotionDelta{};

	const bool hasPrev = (prevFrame >= 0);

	for (int boneId = 0; boneId < boneCount; ++boneId)
	{
		const int parentIdx = boneParents[boneId];
		const Matrix parentCS = (parentIdx >= 0) ? outCSPose[parentIdx] : Matrix();

		AnimationClip::Key k = (boneId < (int)localPose.size()) ? localPose[boneId] : AnimationClip::Key();

		k.rot.Normalize(); // 단위화, 방어코드

		if (parentIdx < 0) // ==== ROOT ====
		{
			const Vector3    curPos = k.pos;
			const Quaternion curRot = k.rot;   // already normalized
			const double     curYaw = MathHelper::YawFromQ_Y(curRot); // 절대 yaw (언랩 전)

			// ▼▼ 여기 "바로 아래"에 추가 ▼▼
			{
				// 기존 요소식(너 지금 쓰던 방식 그대로)
				double yaw_elem = std::atan2(
					2.0 * (curRot.w * curRot.y + curRot.x * curRot.z),
					1.0 - 2.0 * (curRot.y * curRot.y + curRot.x * curRot.x)
				);
				double yaw_fwd = curYaw; // 전방벡터 방식 (YawFromQ_Y)
				double diff = MathHelper::WrapPi(yaw_elem - yaw_fwd);
				std::cout << "yaw_elem=" << yaw_elem
					<< "  yaw_fwd=" << yaw_fwd
					<< "  diff=" << diff << "\n";
			}
			// ▲▲ 여기까지 추가 ▲▲

			if (!hasPrev) {
				prevPos = curPos;
				prevRot = curRot;
				prevYaw = curYaw;
				prevFrame = frame;
			}
			else if (frame == prevFrame) {
				// 같은 프레임 스킵
			}
			else if (frame < prevFrame) { // ===== loop boundary: (prev→end) + (start→cur) =====
				// --- 1) 각 구간의 planar Δ (절대 yaw를 unwrap해서 차이 계산)
				const double endYaw = MathHelper::YawFromQ_Y(rootEnd.rot);
				const double startYaw = MathHelper::YawFromQ_Y(rootStart.rot);

				const double dYaw1 = MathHelper::Unwrap(endYaw - prevYaw);    // prev → end
				const double dYaw2 = MathHelper::Unwrap(curYaw - startYaw);   // start → cur

				const Vector3 d1p = (rootEnd.pos - prevPos);        // prev → end (루트-로컬)
				const Vector3 d2p = (curPos - rootStart.pos);  // start → cur (루트-로컬)

				// --- 2) 평면 합성: 두 번째 구간은 첫 번째의 yaw만큼 회전시켜 더한다
				const Matrix  R_yaw1 = Matrix::CreateFromQuaternion(MathHelper::YawQuat_Y(dYaw1));
				const Vector3 dp = d1p + Vector3::Transform(d2p, R_yaw1);
				const double  dYaw = dYaw1 + dYaw2;

				// --- 3) 출력
				outRootMotionDelta.dPos = dp * posScale;
				outRootMotionDelta.dRot = MathHelper::YawQuat_Y(dYaw);

				// (디버그 누적 필요하면 여기서)
				acc.sumPosL += outRootMotionDelta.dPos;
				acc.sumYawRad = MathHelper::UnwrapAdd(acc.sumYawRad, dYaw);
				acc.count++;

				// --- 4) prev 갱신 (경계 이후를 기준으로)
				prevPos = curPos;
				prevRot = curRot;
				prevYaw = curYaw;
				prevFrame = frame;

				std::cout << "Count : " << acc.count << "Frame :" << frame << "\n";
				std::cout << "Pos : (" << acc.sumPosL.x << "," << acc.sumPosL.y << "," << acc.sumPosL.z << ")\n";
				std::cout << "Yaw : (" << acc.sumYawRad << ")\n";
				cout << "Delta Pos :" << outRootMotionDelta.dPos.x << "," << outRootMotionDelta.dPos.y << "," << outRootMotionDelta.dPos.z << "\n";
				cout << "Delta Rot :" << outRootMotionDelta.dRot.x << "," << outRootMotionDelta.dRot.y << "," << outRootMotionDelta.dRot.z << "," << outRootMotionDelta.dRot.w << "," << "\n";
			}
			else { // ===== 일반 프레임: prev→cur =====
				// 위치는 루트-로컬 그대로 Δ
				const Vector3 dp = (curPos - prevPos);
				outRootMotionDelta.dPos = dp * posScale;

				// 회전은 절대 yaw의 unwrap 차
				const double dYaw = MathHelper::Unwrap(curYaw - prevYaw);
				outRootMotionDelta.dRot = MathHelper::YawQuat_Y(dYaw);

				// (디버그 누적)
				acc.sumPosL += outRootMotionDelta.dPos;
				acc.sumYawRad = MathHelper::UnwrapAdd(acc.sumYawRad, dYaw);
				acc.count++;

				prevPos = curPos;
				prevRot = curRot;
				prevYaw = curYaw;
				prevFrame = frame;

				std::cout << "Count : " << acc.count << "Frame :" << frame << "\n";
				std::cout << "Pos : (" << acc.sumPosL.x << "," << acc.sumPosL.y << "," << acc.sumPosL.z << ")\n";
				std::cout << "Yaw : (" << acc.sumYawRad << ")\n";
				cout << "Delta Pos : (" << outRootMotionDelta.dPos.x << "," << outRootMotionDelta.dPos.y << "," << outRootMotionDelta.dPos.z << ") \n";
				cout << "Delta Rot : (" << outRootMotionDelta.dRot.x << "," << outRootMotionDelta.dRot.y << "," << outRootMotionDelta.dRot.z << "," << outRootMotionDelta.dRot.w << ")" << "\n";
			}

			// === 추출 끝났으니 이제 포즈에서 yaw 제거 (월드 적용은 바깥에서) ===
			k.pos = Vector3(0, 0, 0);
			k.rot = MathHelper::RemoveYaw_Y(k.rot);
		}

		outCSPose[boneId] = k.GetTransform() * parentCS;
	}
}

void AnimationData::BuildComponentPoseFromLocal4(
	const vector<AnimationClip::Key>& localPose, int frame, vector<Matrix>& outCSPose, 
	Vector3& outRootPos, Quaternion& outRootRot, Vector3& outRootScale, 
	Vector3& prevPos, Quaternion& prevRot) const
{
	const int boneCount = static_cast<int>(boneParents.size());
	if (boneCount <= 0)
		return;

	if (outCSPose.size() != boneCount)
		outCSPose.assign(boneCount, Matrix());

	for (int boneId = 0; boneId < boneCount; ++boneId)
	{
		const int parentIdx = boneParents[boneId];
		const Matrix parentMatrix =
			(parentIdx >= 0)
			? outCSPose[parentIdx]
			: Matrix();
			/*: Matrix::CreateScale(outRootScale)
			* Matrix::CreateFromQuaternion(outRootRot)
			* Matrix::CreateTranslation(outRootPos);*/

		AnimationClip::Key key =
			(boneId < (int)localPose.size())
			? localPose[boneId]
			: AnimationClip::Key();

		// --- 루트 (Hip) 처리 ---
		if (parentIdx < 0)
		{
			if (frame != 0)
			{
				// 이동 누적 (로컬 → 월드 변환은 외부에서 수행)
				outRootPos += (key.pos - prevPos) * 0.001;

				// 회전 누적
				Quaternion invPrev;
				prevRot.Inverse(invPrev);
				Quaternion deltaRot = key.rot * invPrev;
				outRootRot = outRootRot * deltaRot;

				// 스케일 누적 (대부분 1이므로 단순 곱)
				outRootScale *= key.scale;
			}
			else
			{
				// 0프레임일 때 초기화
				outRootPos = key.pos * 0.001;
				outRootRot = key.rot;
				outRootScale = key.scale;
			}

			prevPos = key.pos;
			prevRot = key.rot;

			// 루트는 제자리 재생 (루트모션 이동 제거)
			key.pos = Vector3();
			key.rot = Quaternion();
			key.scale = Vector3(1.0f);
		}

		// --- 컴포넌트 공간 포즈 계산 ---
		outCSPose[boneId] = key.GetTransform() * parentMatrix;
	}
}

AnimationClip::Key AnimationData::GetRootKeyAt(int clipIndex, int frame) const
{
	const AnimationClip& clip = clips[clipIndex];

	// 루트 본 인덱스 찾기
	int rootId = 0;
	for (int i = 0; i < (int)boneParents.size(); ++i)
	{
		if (boneParents[i] < 0)
		{
			rootId = i;
			break;
		}
	}

	// 2) 프레임 인덱스 안전화 (wrap)
	const int n = std::max(clip.numKeys, 0);
	int f = (n > 0) ? (frame % n + n) % n : 0;

	// 3) 원본 루트 트랙에서 키 반환 (가공 전)
	// keys: [Bone][Frame]
	// 방어: 인덱스 범위 체크
	if (rootId < 0 || rootId >= (int)clip.keys.size()) 
		return AnimationClip::Key{};

	if (f < 0 || f >= (int)clip.keys[rootId].size())    
		return AnimationClip::Key{};

	return clip.keys[rootId][f];
}

AnimationClip::Key AnimationData::GetRootKeyStart(int clipIndex) const
{
	return GetRootKeyAt(clipIndex, 0);
}

AnimationClip::Key AnimationData::GetRootKeyEnd(int clipIndex) const
{
	const auto& c = clips[clipIndex];
	const int last = max(0, c.numKeys - 1);

	return GetRootKeyAt(clipIndex, last);
}
