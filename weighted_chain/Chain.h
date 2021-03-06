#pragma once
#include <string>
#include <map>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <deque>
#include "Node.h"
#include "Pair.h"
#include "KDTree.h"

#include <fstream>

using namespace std;

string findNearestNeighbor(map<string, Node> m, Node n, double& preTime, double& exeTime)
{
	KDTree* kd = new KDTree();
	const int size = m.size();
	Node* dataSet = new Node[size];
	struct Tnode * root = NULL;
	int i = 0;
	string ret;
	clock_t startTime, endTime;

	startTime = clock();
	for (map<string, Node>::iterator it = m.begin(); it != m.end(); ++it)
	{
		dataSet[i] = it->second;
		i++;
	}
	root = kd->buildTree(dataSet, size, root);
	endTime = clock();
	preTime = 1.0 * (endTime - startTime) / CLOCKS_PER_SEC;

	delete[] dataSet;

	startTime = clock();
	ret = kd->searchNearest(root, n);
	endTime = clock();
	exeTime = 1.0 * (endTime - startTime) / CLOCKS_PER_SEC;

	return ret;
}

void weightedChain(map<string, Node>& P, map<string, Node>& O, deque<Node*>& C, vector<Pair>& A, double& preTime, double& exeTime)
{
	srand(time(0));

	while (!O.empty())
	{
		// pick the first object o from O
		map<string, Node>::iterator it = O.begin();
		advance(it, rand() % O.size());
		Node* o = &(it->second);
		// insert {o} into C
		C.push_front(o);

		while (!C.empty() && !O.empty())
		{
			Node* x = C.front();

			if (O.find(x->id) != O.end())
			{
				double preTimeSlice, exeTimeSlice;

				string y_key = findNearestNeighbor(P, *x, preTimeSlice, exeTimeSlice);
				Node* y = &P[y_key];

				preTime += preTimeSlice;
				exeTime += exeTimeSlice;

				// NN查询有时候会陷入死循环，加上这一步暂时跳出
				if (C.size() >= 2 && (y == C[1] || y == C[3]))
				{
					if (x->weight > y->weight)
					{
						Pair pair(*y, *x, y->weight);
						A.push_back(pair);
						x->weight -= y->weight;
						// remove y from P
						P.erase(y->id);
						// remove x and y from C
						C.pop_front(); C.pop_front();
					}
					else
					{
						Pair pair(*y, *x, x->weight);
						A.push_back(pair);
						y->weight -= x->weight;
						// remove x from O and C
						C.pop_front();
						O.erase(x->id);
						if (y->weight == 0)
						{
							// remove y from P and C 
							C.pop_front();
							P.erase(y->id);
						}
					}
				}
				else
					C.push_front(y);
			}
			else
			{
				double preTimeSlice, exeTimeSlice;

				string y_key = findNearestNeighbor(O, *x, preTimeSlice, exeTimeSlice);
				Node* y = &O[y_key];

				preTime += preTimeSlice;
				exeTime += exeTimeSlice;

				if (C.size() >= 2 && y == C[1])
				{
					if (y->weight > x->weight)
					{
						Pair pair(*x, *y, x->weight);
						A.push_back(pair);
						y->weight -= x->weight;
						// remove x from P
						P.erase(x->id);
						// remove x and y from C
						C.pop_front(); C.pop_front();
					}
					else
					{
						Pair pair(*x, *y, y->weight);
						A.push_back(pair);
						x->weight -= y->weight;
						// remove y from O and C
						C.erase(C.begin() + 1);
						O.erase(y->id);
						if (x->weight == 0)
						{
							// remove x from P and C 
							C.pop_front();
							P.erase(x->id);
						}
					}
				}
				else
					C.push_front(y);
			}
		}
	}
}