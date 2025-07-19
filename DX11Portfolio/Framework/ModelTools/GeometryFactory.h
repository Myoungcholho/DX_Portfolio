#pragma once

class GeomtryGenerator
{
public:
	static vector<PBRMeshData> ReadFromFileModel(string basePath, string filename);

	static PBRMeshData MakeSquare(const float scale = 1.0f, const Vector2 texScale = Vector2(1.0f));
	static PBRMeshData MakeBox(const float scale = 1.0f);
	static PBRMeshData MakeSphere(const float radius, const int numSlices,const int numStacks,const Vector2 texScale = Vector2(1.0f));
	static PBRMeshData MakeSquareGrid(const int numSlices, const int numStacks,const float scale = 1.0f,const Vector2 texScale = Vector2(1.0f));
};