#include "Pch.h"
#include "APlayer.h"

APlayer::APlayer()
{
	name = "Player";
	staticMeshComponent = make_shared<UStaticMeshComponent>();
	root = staticMeshComponent;
	AddComponent(staticMeshComponent);
}

void APlayer::Initialize()
{
	staticMeshComponent->Init();

	//int a = 50;
	//cout << a;
}

void APlayer::Tick()
{
	//static int tick = 0;

	//++tick;
	////cout << tick << "\n";
	//if (tick >= 50000 && bData == false)
	//{
	//	bData = true;
	//	vector<AActor*> actors = world->GetActorsOf();
	//	
	//	for (AActor* actor : actors)
	//	{
	//		if (actor->GetName() == "Plane")
	//		{
	//			actor->Destroy();
	//		}
	//	}
	//}
}

shared_ptr<UStaticMeshComponent> APlayer::GetStaticMeshComponent()
{
	return staticMeshComponent;
}
