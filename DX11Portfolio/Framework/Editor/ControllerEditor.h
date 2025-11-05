#pragma once

class AController;

class ControllerEditor : public Editor
{
public:
	explicit ControllerEditor(AController* target = nullptr);

	void SetTarget(AController* target);
	bool IsValid() const { return mTarget != nullptr; }

	void Update() override;
	void OnGUI() override;

private:
	void RefreshPawnList();

private:
	AController* mTarget = nullptr;
	std::vector<APawn*> mPawnList;   // 월드의 Pawn 목록 캐시
	int mSelectedPawnIndex = -1;     // 현재 선택된 Pawn 인덱스
};