#pragma once

class GeometryGenerator
{
public:
	static vector<PBRMeshData> ReadFromFile(string basePath, string filename, bool revertNormals = false);
	static auto ReadAnimationFromFile(string basePath, string filename, bool revertNormals = false)
		-> tuple<vector<PBRMeshData>, AnimationData>;

	static void Normalize(const Vector3 center, const float longestLength, vector<PBRMeshData>& meshes, AnimationData& aniData);

	static PBRMeshData MakeSquare(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
	static PBRMeshData MakeBox(const float scale = 1.0f);
	static PBRMeshData MakeSphere(const float radius, const int numSlices,const int numStacks,const Vector2 texScale = Vector2(1.0f));
	static PBRMeshData MakeSquareGrid(const int numSlices, const int numStacks,const float scale = 1.0f,const Vector2 texScale = Vector2(1.0f));
};