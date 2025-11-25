#include "Framework.h"
#include "InspectorWindow.h"

InspectorWindow::InspectorWindow()
{
	SetName("InspectorWindow");
	SetSize(ImVec2(300, 600));
}

InspectorWindow::~InspectorWindow()
{
	if (InspectorHandle.IsValid())
		EditorSelection::OnSelected.Remove(InspectorHandle);
    if (InspectorComponentHandle.IsValid())
        EditorSelection::OnSelectedComponent.Remove(InspectorComponentHandle);

	mEditors.clear();
}

void InspectorWindow::Initialize()
{
	InspectorHandle = EditorSelection::OnSelected.AddDynamic(this, &InspectorWindow::OnUpdateTarget, "InspectorWindow::OnUpdateTarget");
    InspectorComponentHandle = EditorSelection::OnSelectedComponent.AddDynamic(this, &InspectorWindow::OnUpdateTarget, "InspectorWindow::OnUpdateTarget");
}

void InspectorWindow::Run()
{
	bool Active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &Active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void InspectorWindow::Update()
{
	for (auto& editor : mEditors) editor->Update();
}

void InspectorWindow::OnGUI()
{
	for (auto& editor : mEditors) editor->OnGUI();
}


void InspectorWindow::OnEnable()
{
}

void InspectorWindow::OnDisable()
{
}

void InspectorWindow::OnDestroy()
{
}

// EditorSelection::OnSelected 구독
void InspectorWindow::OnUpdateTarget()
{
    mEditors.clear();

    AActor* actor = EditorSelection::GetActor();
    if (!actor) return;

    UActorComponent* selComp = EditorSelection::GetActorComponent();

    // -- Actor 전용 ---
    if (auto* pawn = dynamic_cast<APawn*>(actor))
    {
        auto pawnEditor = make_unique<PawnEditor>(pawn);
        pawnEditor->SetName("Pawn");
        mEditors.emplace_back(move(pawnEditor));
    }

    if (auto* controller = dynamic_cast<AController*>(actor))
    {
        auto controllerEditor = make_unique<ControllerEditor>(controller);
        controllerEditor->SetName("Controller");
        mEditors.emplace_back(move(controllerEditor));
    }

    // 공통: 컴포넌트 타입별 에디터 추가 람다
    auto addEditors = [&](UActorComponent* comp, const char* overrideName = nullptr)
        {
            if (!comp) return;

            if (auto* sc = dynamic_cast<USceneComponent*>(comp)) 
            {
                auto te = std::make_unique<TransformEditor>(sc);
                te->SetName(overrideName ? overrideName : sc->mName);
                mEditors.emplace_back(std::move(te));
            }
            if (auto* light = dynamic_cast<ULightComponent*>(comp)) 
            {
                auto le = std::make_unique<LightEditor>(light);
                le->SetName(light->mName);
                mEditors.emplace_back(std::move(le));
            }
            if (auto* mesh = dynamic_cast<UPrimitiveComponent*>(comp)) 
            {
                auto se = std::make_unique<PrimitiveEditor>(mesh);
                se->SetName(mesh->mName);
                mEditors.emplace_back(std::move(se));
            }
            if (auto* skeletal = dynamic_cast<USkeletalMeshComponent*>(comp))
            {
                auto sk = std::make_unique<AnimationEditor>(skeletal);
                sk->SetName(skeletal->mName);
                mEditors.emplace_back(std::move(sk));
            }
        };

    // 1) 컴포넌트가 선택되어 있으면 → 그 컴포넌트만
    if (selComp) 
    {
        // 선택이 루트 씬컴포넌트라면 이름을 "Root"로 보여주고 싶을 때
        USceneComponent* root = actor->GetRootComponent().get();
        const bool isRoot = (root && selComp == root);
        addEditors(selComp, isRoot ? "Root" : nullptr);
        return;
    }

    // 2) 컴포넌트 선택이 없으면 → 액터의 모든 컴포넌트
    auto rootSP = actor->GetRootComponent();
    if (rootSP) 
    {
        addEditors(rootSP.get(), "Root");
    }
    for (auto& compSP : actor->GetComponents()) 
    {
        UActorComponent* c = compSP.get();

        // 루트는 위에서 처리했으니 중복 방지
        if (rootSP && c == rootSP.get()) 
            continue;

        addEditors(c);
    }
}
