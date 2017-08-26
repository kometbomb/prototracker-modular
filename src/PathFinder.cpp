#include "PathFinder.h"
#include <set>
#include <queue>
#include <cmath>
#include <cstdio>

void PathFinder::setNetwork(const std::vector<PathFinder::Node>& network)
{
	mNetwork = network;
}


static int sign(int x)
{
	if (x < 0)
		return -1;
	else if (x > 0)
		return 1;
	else
		return 0;
}


int PathFinder::realCost(const PathFinder::Node& from, const PathFinder::Node& to) const
{
	int turnCost = 0;
	
	if (from.cameFrom != -1)
	{
		int dx = from.x - mNetwork[from.cameFrom].x;
		int dy = from.y - mNetwork[from.cameFrom].y;
		if (sign(dx) != sign(to.x - from.x))
			turnCost += 1;
		
		if (sign(dy) != sign(to.y - from.y))
			turnCost += 1;
	}
	
	return (from.cost + to.cost + 1) * estimate(from, to) + turnCost;
}


int PathFinder::estimate(const PathFinder::Node& from, const PathFinder::Node& to) const
{
	int dx = from.x - to.x;
	int dy = from.y - to.y;
	return sqrt(dx * dx + dy * dy) * 32;
}


std::vector<int> PathFinder::findPath(int from, int to)
{
	std::set<int> visited;
	std::set<int> queued;
	
	queued.insert(from);
	
	for (auto& node : mNetwork)
	{
		node.gScore = 999999;
		node.fScore = 999999;
		node.cameFrom = -1;
	}
	
	mNetwork[from].gScore = 0;
	mNetwork[from].fScore = estimate(mNetwork[from], mNetwork[to]);
	
	while (!queued.empty()) 
	{
        int current = -1;
		
		for (auto index : queued)
			if (current == -1 || mNetwork[current].fScore > mNetwork[index].fScore)
				current = index;
		
        if (current == to)
            return reconstructPath(current);
		
		queued.erase(queued.find(current));
		visited.insert(current);

        for (auto neighbor : mNetwork[current].connections)
		{
            if (visited.find(neighbor.node) != visited.end()) 
                continue;

            if (queued.find(neighbor.node) == queued.end())
                queued.insert(neighbor.node);
            
            int tentativeGScore = mNetwork[current].gScore + realCost(mNetwork[current], mNetwork[neighbor.node]);
            
			if (tentativeGScore >= mNetwork[neighbor.node].gScore)
                continue;

            mNetwork[neighbor.node].cameFrom = current;
            mNetwork[neighbor.node].gScore = tentativeGScore;
            mNetwork[neighbor.node].fScore = mNetwork[neighbor.node].gScore + estimate(mNetwork[neighbor.node], mNetwork[to]);
		}
	}
	
	return std::vector<int>();
}


std::vector<int> PathFinder::reconstructPath(int current) const
{
    std::vector<int> totalPath;
	totalPath.push_back(current);
	
    while (true)
	{
        current = mNetwork[current].cameFrom;
		if (current == -1)
			break;
		
        totalPath.push_back(current);
	}
	
    return totalPath;
}


PathFinder::Node::Connection::Connection(int _node)
	: node(_node), timesUsed()
{}


void PathFinder::Node::Connection::resetCost()
{
	timesUsed = 0;
}


void PathFinder::Node::resetCosts()
{
	cost = 0;
	for (auto& connection : connections)
		connection.resetCost();
}


PathFinder::Node::Connection& PathFinder::Node::findConnectionTo(int node)
{
	for (auto& connection : connections)
	{
		if (connection.node == node)
			return connection;
	}
	
	return connections[0];
}
