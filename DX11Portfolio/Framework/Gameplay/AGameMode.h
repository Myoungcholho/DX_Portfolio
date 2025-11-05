#pragma once

#include <typeindex>

class AActor;
class APlayerController;

class AGameMode : public AActor
{
public:
	AGameMode();

public:
	virtual void StartPlay();
	virtual void Tick() override;

	void SetDefaultPawnClass(const string& pawnName);
	void SetDefaultControllerClass(const string& controllerName);

private:
	string PawnClassName;
	string ControllerClassName;

private:
	APawn* PlayerPawn = nullptr;
	APlayerController* PlayerController = nullptr;
};