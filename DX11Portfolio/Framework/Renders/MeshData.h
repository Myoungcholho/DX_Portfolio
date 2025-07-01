#pragma once

#include <directxtk/SimpleMath.h>
#include <string>
#include <vector>

#include "VertexLayout.h"

struct MeshData
{
	vector<FVertex> vertices;
	vector<uint32_t> indices;
	string textureFilename;
	string fullFilename;
};