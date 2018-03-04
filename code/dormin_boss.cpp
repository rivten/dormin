#pragma once

internal test_boss
TestBossInit(void)
{
	test_boss Result = {};
	for(u8 TileIndex = 0; TileIndex < TEST_BOSS_LENGTH; ++TileIndex)
	{
		Result.WorldTiles[TileIndex] = V2i(3 + TEST_BOSS_LENGTH - TileIndex, 3);
	}
	Result.LifePoints = GlobalWindowTileCountX;
	Result.Dir = Dir_Right;
	Result.PathFinding = {};
	Result.CurrentPathFindingNodeIndex = 0;
	Result.Entropy = RandomSeed(3456);

	return(Result);
}

internal v2i
TestBossGetValidDestPoint(game_state* GameState)
{
	v2i RandomTile = GetRandomWorldTile(&GameState->WorldDynamics.Boss.Entropy);
	while(GetStaticWorldTileValue(GameState->StaticWorld, RandomTile) != 0)
	{
		RandomTile = GetRandomWorldTile(&GameState->WorldDynamics.Boss.Entropy);
	}

	return(RandomTile);
}

internal void
StepTestBoss(game_state* GameState)
{
	test_boss* Boss = &GameState->WorldDynamics.Boss;

	if(Boss->CurrentPathFindingNodeIndex >= Boss->PathFinding.PathNodeCount)
	{
		Boss->CurrentPathFindingNodeIndex = 0;

		v2i DestPoint = TestBossGetValidDestPoint(GameState);
		Boss->PathFinding = SearchPath(GameState->StaticWorld, Boss->WorldTiles[0], DestPoint);
	}
	else
	{
		// NOTE(hugo): Make the rest of the body follow
		for(u8 TileIndex = TEST_BOSS_LENGTH - 1; TileIndex > 0; --TileIndex)
		{
			Boss->WorldTiles[TileIndex] = Boss->WorldTiles[TileIndex - 1];
		}

		Assert(Boss->CurrentPathFindingNodeIndex < Boss->PathFinding.PathNodeCount);
		Boss->WorldTiles[0] = Boss->PathFinding.PathNodes[Boss->CurrentPathFindingNodeIndex];
		++Boss->CurrentPathFindingNodeIndex;
	}
}
