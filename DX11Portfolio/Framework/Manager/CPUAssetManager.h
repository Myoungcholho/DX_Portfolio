#pragma once

struct CPUMeshAsset {
    vector<PBRMeshData> meshes;
};

struct AssetKey {
    string path;
    bool operator==(const AssetKey& o) const noexcept { return path == o.path; }
};

struct AssetKeyHash {
    size_t operator()(const AssetKey& k) const noexcept { // 에셋 키의 path 문자열을 해시값으로 변경
        return hash<string>{}(k.path);
    }
};


class CPUAssetManager
{
public:
    // Mesh
    static shared_ptr<const CPUMeshAsset> LoadCPUMesh(const string& root,const string& file);
    static shared_ptr<const CPUMeshAsset> CreateProcedural(const string& id, const vector<PBRMeshData>& meshes);
    static shared_ptr<const CPUMeshAsset> GetProcedural(const string& id);
    static void ClearUnused();

    // Animation
    static shared_ptr<AnimationData> SaveAnimation(const string& id, shared_ptr<AnimationData>& aniData);
    static shared_ptr<AnimationData> GetAnimation(const string& id);
private:
    static shared_ptr<const CPUMeshAsset> BuildCPUMesh_(const string& root,const string& file);

private:
    // 3번째 인자는 커스텀 해시 함수, 접근은 Data, 내부 데이터는 Heap
    // Mesh
    static unordered_map<AssetKey, weak_ptr<const CPUMeshAsset>, AssetKeyHash> cache;

    // Animation
    static unordered_map<AssetKey, weak_ptr<AnimationData>, AssetKeyHash> animationCache;
};