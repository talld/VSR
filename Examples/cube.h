#ifndef VSR_SUITE_CUBE_H
#define VSR_SUITE_CUBE_H

enum {kCubeVertexCount= 8, kCubeIndexCount = 6 * 6};

VSR_Vertex cubeVerts[kCubeVertexCount] = {
	//Top
	{-1, -1, -1}
	, {1, -1, -1}
	, {1, 1, -1}
	, {-1, 1, -1}
	, {-1, -1, 1}
	, {1, -1, 1}
	, {1, 1, 1,}
	, { -1, 1, 1 }
};

VSR_UV cubeUVs[kCubeIndexCount] = {
	//Top
	{0.5, 1}
	, {0.5, 1}
	, {0.5, 1}
	, {1, 1}
	, {1, 1}
	, {1, 1}
	, {1, 1}
	, { 1, 1}
};

VSR_Index cubeIndices[kCubeIndexCount] = {
	//Top
	{0}, {1}, {3},
	{3}, {1}, {2},
	{1}, {5}, {2},
	{2}, {5}, {6},
	{5}, {4}, {6},
	{6}, {4}, {7},
	{4}, {0}, {7},
	{7}, {0}, {3},
	{3}, {2}, {7},
	{7}, {2}, {6},
	{4}, {5}, {0},
	{0}, {5}, {1}
			  };

#endif //VSR_SUITE_CUBE_H
