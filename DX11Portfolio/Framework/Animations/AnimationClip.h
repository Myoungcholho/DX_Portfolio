#pragma once

#include <directxtk/SimpleMath.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;
using std::map;
using std::string;
using std::vector;

struct AnimationClip
{
	struct Key
	{
		double timeTicks = 0.0;				// 키의 시간
		Vector3 pos = Vector3(0.0f);
		Vector3 scale = Vector3(1.0f);
		Quaternion rot = Quaternion();

		Matrix GetTransform()
		{
			return Matrix::CreateScale(scale) *
				Matrix::CreateFromQuaternion(rot) *
				Matrix::CreateTranslation(pos);
		}
	};

	string name;					// 애니메이션 이름
	vector<vector<Key>> keys;		// [Bone][Frame]
	int numChannels;				// 뼈 개수
	int numKeys;					// 프레임 개수
	double duration;				// 애니메이션 총 길이
	double ticksPerSec;				// 재생 속도
};

struct AnimationData
{
	// == 정적 에셋 데이터 ==
	map<string, int32_t> boneNameToId;			// 뼈 이름으로 몇번째 index인지 확인하기 위함
	vector<string> boneIdToName;				// 뼈 index로 이름 확인용(5번 본이 뭐지? map을 전부 순회하는건 비효율 따라서 vector를 하나 더)
	vector<int32_t> boneParents;				// 배열로 부모의 인덱스를 저장
	vector<Matrix> offsetMatrices;				// 정점은 모델(0,0,0) 즉 WorldMatrix기준에 위치에 있어서 정점을 뼈 기준으로 바꾸기 위해 사용하는 inverse 행렬
	//vector<Matrix> boneTransforms;				// 스켈레톤 계층 구조에 따라 부모-자식 누적이 반영된 행렬 그니까 내 Transform에서 WorldMatrix라고 생각하면 된다.
	vector<AnimationClip> clips;				// 이 모델이 가진 모든 애니메이션 클립 모음(Idle, Run, ..)

	// 모델을 읽을 때 AABB계산하고 중심점을 center로 오도록 정규화하는데
	// 이때 정점 데이터는 변환했는데 그대로 두면 정점은 스케일된 상태인데
	// 뼈는 원래 크기라서 정점과 뼈가 어긋난다. 그래서 보정 행렬이 필요하다.
	// 메시 정규화에 사용한 보정 값을 저장한 행렬.
	Matrix defaultTransform;					// 모델을 불러올 때 좌표계를 맞추기 위한 보정 행렬
	Matrix rootTransform = Matrix();			// 루트 뼈 행렬
	
	//Matrix accumulatedRootTransform = Matrix();	// 루트 애니메이션용 누적
	//Vector3 prevPos = Vector3(0.0f);			// delta값 구하기 위해 이전 값 저장

	void ComposeSkinPalette(const vector<Matrix>& csPose, vector<Matrix>& outPalette) const
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


	/// <summary>
	/// 로컬 포즈 추출, 특정 클립의 frame에서 모든 본의 로컬 키를 vector로 저장
	/// </summary>
	void EvaluateLocalPose(int clipId, int frame, double accum, vector<AnimationClip::Key>& outLocalPose) const
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

	/// <summary>
	/// 로컬포즈 -> 컴포넌트 포즈로 변환
	/// </summary>
	void BuildComponentPoseFromLocal(const vector<AnimationClip::Key>& localPose,
		int frame,
		vector<Matrix>& outCSPose,
		Matrix& accumulatedRootTransform,  
		Vector3& prevPos                   
	) const
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
			if (parentIdx < 0)
			{
				if (frame != 0)
				{
					accumulatedRootTransform = Matrix::CreateTranslation(key.pos - prevPos) * accumulatedRootTransform;
				}
				else
				{
					auto t = accumulatedRootTransform.Translation();
					t.y = key.pos.y;
					accumulatedRootTransform.Translation(t);
				}

				prevPos = key.pos;
				key.pos = Vector3(0.0f); // 루트 본은 제자리 재생
			}

			outCSPose[boneId] = key.GetTransform() * parentMatrix;
		}
	}

};


/// <summary>
	/// 여기가 어지럽다
	/// 만약 정점이 (5,1,2)이고 뼈 이동이 (2,0,0)이라면 결과는 (7,1,2)여야할것이다.
	/// 근데 지금은 모델 정점 위치를 AABB로 정규화를 거쳤다. 만약 0.5 작아졌다면
	/// (2.5,0.5,1)이다. 여기서 (2,0,0)을 이동하면 정규화된 정점을 고려하지 않은체 본이 이동시키는 것이다.
	/// 따라서 (2,0,0)도 조정이 필요하다. 그래서 정규화시키고(defaultTransform) 계산하고
	/// 다시 원복하는 것 (defaultTransform.Invert())이다.
	/// 와 row major라서 오른쪽 끝부터 해야하는게 함정이네 진짜 어지럽다
	/// </summary>
	//Matrix Get(int clipId, int boneId, int frame)
	//{
	//	return defaultTransform.Invert() *
	//		offsetMatrices[boneId] *
	//		boneTransforms[boneId] *
	//		defaultTransform;
	//}

	/// <summary>
	/// 옛날에 썻던거 지금은 X
	/// </summary>
	//void Update(int clipId, int frame)
	//{
	//	AnimationClip& clip = clips[clipId];

	//	// 모든 뼈 순회
	//	for (int boneId = 0; boneId < boneTransforms.size(); ++boneId)
	//	{
	//		vector<AnimationClip::Key>& keys = clip.keys[boneId];				// 특정 Clip이 가지는 특정 Bone의 모든 Frame별 Transform 값

	//		const int parentIdx = boneParents[boneId];							// 본의 부모 인덱스
	//		const Matrix parentMatrix = parentIdx >= 0 ? 
	//			boneTransforms[parentIdx] : accumulatedRootTransform;			// 만약 루트가 아니라면 부모의 행렬을, 아니라면 루트뼈 행렬을

	//		AnimationClip::Key key = keys.size() > 0 ?
	//			keys[frame % keys.size()] : AnimationClip::Key();				// 프레임 수를 초과할 수 있으므로 %를 사용하고 키가 아예 없다면 Identity

	//		// 루트 뼈는 특별 처리
	//		if (parentIdx < 0)
	//		{
	//			if (frame != 0)
	//			{
	//				// 루트본은 앞으로 이동하는 값을 가짐. prevPos와 현재의 차이는 이번 프레임 동안 이동량
	//				// 이걸 accumulatedRootTransform에 누적
	//				accumulatedRootTransform = Matrix::CreateTranslation(key.pos - prevPos) * accumulatedRootTransform;
	//			}
	//			else // 첫 프레임은 delta값이 없어서 특별 처리
	//			{
	//				// y값만 키프레임 값으로 맞추고 나머지는 그대로
	//				auto temp = accumulatedRootTransform.Translation();
	//				temp.y = key.pos.y;
	//				accumulatedRootTransform.Translation(temp);
	//			}

	//			// 루트 본 자체에서 이동을 0으로 리셋 애니메이션은 제자리에서만 재생되고
	//			// 실제 이동은 accumulatedRootTransform이 담당
	//			prevPos = key.pos;
	//			key.pos = Vector3(0.0f);
	//		}

	//		// 본의 행렬, 현재 키프레임의 위치와 부모의 매트릭스 위치로부터 업데이트
	//		boneTransforms[boneId] = key.GetTransform() * parentMatrix;
	//	}
	//}