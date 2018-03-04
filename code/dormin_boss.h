#pragma once

#define TEST_BOSS_LENGTH 10
struct test_boss
{
	v2i WorldTiles[TEST_BOSS_LENGTH];
	u8 LifePoints;
	direction Dir;

	pathfinding_result PathFinding;
	u32 CurrentPathFindingNodeIndex;

	random_series Entropy;
};

