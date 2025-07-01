#pragma once

class GeomtryGenerator
{
public:
	static vector<MeshData> ReadFromFile(string basePath, string filename);

	static MeshData MakeSquare();
	static MeshData MakeBox(const float scale = 1.0f);
};