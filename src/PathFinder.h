#pragma once

#include <vector>

class PathFinder
{
public:
	struct Node {
		int gScore, fScore;
		int cameFrom;
		
		int cost;
		int x, y;
		
		struct Connection {
			int node;
			int timesUsed;
			Connection(int _node);
			void resetCost();
		};
		
		Connection& findConnectionTo(int node);
		
		std::vector<Connection> connections;
		
		void resetCosts();
	};
	
private:
	std::vector<Node> mNetwork;
	
	int estimate(const Node& from, const Node& to) const;
	int realCost(const Node& from, const Node& to) const;
	std::vector<int> reconstructPath(int current) const;

public:
	void setNetwork(const std::vector<Node>& network);
	std::vector<int> findPath(int from, int to);
};
