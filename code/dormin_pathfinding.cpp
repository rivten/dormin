#pragma once

internal dijkstra_node*
FindNextNodeWithMinDistance(dijkstra_node* DijkstraWorld)
{
	dijkstra_node* Result = 0;
	u32 MinimalDistanceFound = MAX_U32;
	for(u32 NodeIndex = 0; NodeIndex < STATIC_WORLD_WIDTH * STATIC_WORLD_HEIGHT; ++NodeIndex)
	{
		dijkstra_node* Node = DijkstraWorld + NodeIndex;
		if(!Node->Visited &&
				(Node->Distance < MinimalDistanceFound))
		{
			MinimalDistanceFound = Node->Distance;
			Result = Node;
		}
	}

	return(Result);
}

internal pathfinding_result
SearchPath(u8* StaticWorld, v2i Begin, v2i End)
{
	Assert(!AreEqualsV2i(Begin, End));

	dijkstra_node DijkstraWorld[STATIC_WORLD_WIDTH * STATIC_WORLD_HEIGHT] = {};
	for(u32 NodeIndex = 0; NodeIndex < STATIC_WORLD_WIDTH * STATIC_WORLD_HEIGHT; ++NodeIndex)
	{
		dijkstra_node* Node = DijkstraWorld + NodeIndex;
		Node->Distance = MAX_U32;
		Node->PreviousNodeWorldTile = {};
		Node->WorldTile = V2i(NodeIndex % STATIC_WORLD_WIDTH, NodeIndex / STATIC_WORLD_WIDTH);

		// NOTE(hugo): Do not consider non-available tiles by marking them
		// already visited but with a max distance reach.
		Node->Visited = (StaticWorld[NodeIndex] != 0);
	}

	dijkstra_node* BeginNode = DijkstraWorld + (Begin.x + STATIC_WORLD_WIDTH * Begin.y);
	BeginNode->Distance = 0;

	dijkstra_node* CurrentNode = FindNextNodeWithMinDistance(DijkstraWorld);
	while(CurrentNode)
	{
		CurrentNode->Visited = true;
		v2i NodeWorldTile = CurrentNode->WorldTile;

		// NOTE(hugo): Tile up
		v2i UpTile = NodeWorldTile + V2i(0, -1);
		dijkstra_node* UpNode = DijkstraWorld + (UpTile.x + STATIC_WORLD_WIDTH * UpTile.y);
		if(!UpNode->Visited)
		{
			u32 CurrentDistance = CurrentNode->Distance + 1;
			if(CurrentDistance < UpNode->Distance)
			{
				UpNode->Distance = CurrentDistance;
				UpNode->PreviousNodeWorldTile = CurrentNode->WorldTile;
			}
		}

		// NOTE(hugo): Tile down
		v2i DownTile = NodeWorldTile + V2i(0, 1);
		dijkstra_node* DownNode = DijkstraWorld + (DownTile.x + STATIC_WORLD_WIDTH * DownTile.y);
		if(!DownNode->Visited)
		{
			u32 CurrentDistance = CurrentNode->Distance + 1;
			if(CurrentDistance < DownNode->Distance)
			{
				DownNode->Distance = CurrentDistance;
				DownNode->PreviousNodeWorldTile = CurrentNode->WorldTile;
			}
		}

		// NOTE(hugo): Tile left
		v2i LeftTile = NodeWorldTile + V2i(-1, 0);
		dijkstra_node* LeftNode = DijkstraWorld + (LeftTile.x + STATIC_WORLD_WIDTH * LeftTile.y);
		if(!LeftNode->Visited)
		{
			u32 CurrentDistance = CurrentNode->Distance + 1;
			if(CurrentDistance < LeftNode->Distance)
			{
				LeftNode->Distance = CurrentDistance;
				LeftNode->PreviousNodeWorldTile = CurrentNode->WorldTile;
			}
		}

		// NOTE(hugo): Tile right
		v2i RightTile = NodeWorldTile + V2i(1, 0);
		dijkstra_node* RightNode = DijkstraWorld + (RightTile.x + STATIC_WORLD_WIDTH * RightTile.y);
		if(!RightNode->Visited)
		{
			u32 CurrentDistance = CurrentNode->Distance + 1;
			if(CurrentDistance < RightNode->Distance)
			{
				RightNode->Distance = CurrentDistance;
				RightNode->PreviousNodeWorldTile = CurrentNode->WorldTile;
			}
		}

		// NOTE(hugo): At the end, compute the next node
		CurrentNode = FindNextNodeWithMinDistance(DijkstraWorld);
	}

	// NOTE(hugo): Compute path result
	// {
	pathfinding_result Result = {};

	u32 PathNodeCount = 0;
	CurrentNode = DijkstraWorld + (End.x + STATIC_WORLD_WIDTH * End.y);

	while(!AreEqualsV2i(CurrentNode->WorldTile, Begin))
	{
		Assert(CurrentNode->Distance < MAX_U32);
		Assert(CurrentNode->Visited);

		++PathNodeCount;
		v2i PreviousNodeWorldTile = CurrentNode->PreviousNodeWorldTile;
		CurrentNode = DijkstraWorld + (PreviousNodeWorldTile.x + STATIC_WORLD_WIDTH * PreviousNodeWorldTile.y);
	}

	Result.PathNodeCount = PathNodeCount;
	CurrentNode = DijkstraWorld + (End.x + STATIC_WORLD_WIDTH * End.y);
	for(u32 NodeIndex = 0; NodeIndex < PathNodeCount; ++NodeIndex)
	{
		Result.PathNodes[PathNodeCount - NodeIndex - 1] = CurrentNode->WorldTile;

		v2i PreviousNodeWorldTile = CurrentNode->PreviousNodeWorldTile;
		CurrentNode = DijkstraWorld + (PreviousNodeWorldTile.x + STATIC_WORLD_WIDTH * PreviousNodeWorldTile.y);
	}

	// }

	return(Result);
}
