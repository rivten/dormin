#pragma once

#define MAX_PATH_NODES 64
#define MAX_NODES_TO_EXPLORE 128

struct pathfinding_result
{
	u32 PathNodeCount;
	v2i PathNodes[MAX_PATH_NODES];
};

struct dijkstra_node
{
	u32 Distance;
	bool Visited;
	v2i WorldTile;
	v2i PreviousNodeWorldTile;
};

