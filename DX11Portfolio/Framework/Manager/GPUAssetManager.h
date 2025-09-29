#pragma once

struct GPUMeshAsset
{
	vector<shared_ptr<Mesh>> meshes;
};

class GPUAssetManager
{
public:
	static shared_ptr<const GPUMeshAsset> LoadGPUMesh(shared_ptr<const CPUMeshAsset> cpuAsset);
	static void ClearUnused();

private:
	static shared_ptr<const GPUMeshAsset> BuildGPUMesh_(shared_ptr<const CPUMeshAsset> cpuAsset);

private:
	static unordered_map<const CPUMeshAsset*, weak_ptr<const GPUMeshAsset>> cache;
};