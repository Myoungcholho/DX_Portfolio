#include "Framework.h"
#include "CPUAssetManager.h"

// -------------------- 정적 멤버 정의 --------------------

//mutex CPUAssetManager::mtx;
unordered_map<AssetKey, weak_ptr<const CPUMeshAsset>, AssetKeyHash> CPUAssetManager::cache;

// -------------------- 구현 --------------------
shared_ptr<const CPUMeshAsset> CPUAssetManager::LoadCPUMesh(const string& root, const string& file)
{
    const AssetKey key{ root + file };                                                                       // 첫번째 멤버에 값을 초기화(path, list-init)

    unordered_map<AssetKey, weak_ptr<const CPUMeshAsset>, AssetKeyHash>::iterator it = cache.find(key);         // 캐시에 있는지 확인

    if (it != cache.end())                                                                                      // 캐시에 있다면
    {
        shared_ptr<const CPUMeshAsset> sp = it->second.lock();                                                  // weak를 shared로 승격

        if (sp)
            return sp;
    }

    shared_ptr<const CPUMeshAsset> asset = BuildCPUMesh_(root, file);
    cache.emplace(key, asset);                                                                                  // 컨테이너 안에서 객체를 생성해 복사/이동 비용 절감
    return asset;                                                                                               // 호출자에게 shared_ptr을 복사
}

shared_ptr<const CPUMeshAsset> CPUAssetManager::CreateProcedural(const string& id, const vector<PBRMeshData>& meshes)
{
    const AssetKey key{ id };

    auto it = cache.find(key);
    if (it != cache.end()) {
        if (auto sp = it->second.lock())
            return sp;
    }

    auto asset = make_shared<CPUMeshAsset>();
    asset->meshes = meshes;
    cache.emplace(key, asset);
    return asset;
}

void CPUAssetManager::ClearUnused()
{
    unordered_map<AssetKey, weak_ptr<const CPUMeshAsset>, AssetKeyHash>::iterator it = cache.begin();
    // 순회하면서 캐시 유효 확인
    while (it != cache.end()) 
    {
        if (it->second.expired())                                                                               // 약한 참조가 가리키는 객체가 파괴되었는지 확인
            it = cache.erase(it);
        else 
            ++it;
    }
}

shared_ptr<const CPUMeshAsset> CPUAssetManager::BuildCPUMesh_(const string& root, const string& file)
{
    vector<PBRMeshData> cpu = GeometryGenerator::ReadFromFile(root, file);
    shared_ptr<CPUMeshAsset> asset = make_shared<CPUMeshAsset>();
    
    asset->meshes = move(cpu);
    return asset;
}
