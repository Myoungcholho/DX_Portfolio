#include "Pch.h"
#include "APlayer.h"

APlayer::APlayer()
{
	name = "APlayer";

	/*mesh = make_shared<UStaticMeshComponent>();
	rootShared = staticMeshComponent;
	AddComponent(staticMeshComponent);*/

	mesh = CreateDefaultSubobject<UStaticMeshComponent>("mesh");
	SetRootComponent(mesh);

}

void APlayer::Initialize()
{
	mesh->Init();

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