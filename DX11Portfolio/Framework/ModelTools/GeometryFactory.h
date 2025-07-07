#pragma once

class GeomtryGenerator
{
public:
	static vector<MeshData> ReadFromFile(string basePath, string filename);

	static MeshData MakeSquare();
	static MeshData MakeBox(const float scale = 1.0f);

	static PBRMeshData MakeSquareGrid(const int numSlices, const int numStacks,const float scale = 1.0f,const Vector2 texScale = Vector2(1.0f));
};