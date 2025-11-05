#pragma once

class APawn;

class AController : public AActor
{
public:
	AController();
	
public:
	virtual void Possess(APawn* InPawn);
	virtual void UnPossess();
	APawn* GetPawn() const { return pawn; }

	void SetControlRotation(const Quaternion& q);
	const Quaternion& GetControlRotation() const { return controlRotation; }

	virtual void Tick() override;

protected:
	APawn* pawn = nullptr;
	Quaternion controlRotation;
	// 트렌스폼은 관리할 필요가 없다, 그것은 실제로 움직이는 Pawn이 관리 대상이고
	// 뇌의 역할을 하는 컨트롤러기 때문에 어디를 보는지 정보는 필요함으로 회전만 가지고 있게
};