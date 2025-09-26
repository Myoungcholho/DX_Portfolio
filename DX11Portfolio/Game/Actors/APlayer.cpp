#include "Pch.h"
#include "APlayer.h"

APlayer::APlayer()
{
	m_name = "Player";
	m_staticMeshComponent = make_shared<UStaticMeshComponent>();
	root = m_staticMeshComponent;
	AddComponent(m_staticMeshComponent);
}

void APlayer::Initialize()
{
	m_staticMeshComponent->Init();

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
	//	vector<AActor*> actors = m_world->GetActorsOf();
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
	return m_staticMeshComponent;
}
