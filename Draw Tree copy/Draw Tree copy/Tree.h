#pragma once
#include <vector>
#include <tuple>
#include <fstream>
#include <cmath>
#include <algorithm>

struct Node
{
	int Level;
	double X;
	double Y;
	bool IsCircle;
	int ID;
	double Weight;
	double Probability;
	Node *Parent;
	Node *Child1;
	Node *Child2;
	Node(int iD, Node *child1, Node *child2)
	{
		if(child1->X > child2->X)
		{
			Node *hold = child1;
			child1 = child2;
			child2 = hold;
		}
		IsCircle = true;
		ID = iD;
		Weight = child1->Weight + child2->Weight;
		Parent = nullptr;
		Child1 = child1;
		Child2 = child2;
		Child1->Parent = this;
		Child2->Parent = this;
		Level = child1->Level - 1;
		X = child1->X < child2->X ? child1->X : child2->X;
		Y = child1->Y - 1;
	}
	Node(int iD, double weight, double x)
	{
		IsCircle = false;
		ID = iD;
		Weight = weight;
		Parent = nullptr;
		Child1 = nullptr;
		Child2 = nullptr;
		X = x;
		Y = 0;
	}
};

std::vector<double> GetWeights(const std::vector<double> &Probabilities)
{
	double alpha = 1;
	for(int i = 0; i < Probabilities.size(); i++)
	{
		alpha *= (1 - Probabilities[i]);
	}
	alpha = (1 - alpha);

	std::vector<double> weights;
	double qj = 1;
	for(int i = 0; i < Probabilities.size(); i++)
	{
		weights.push_back(Probabilities[i] * qj / alpha);
		qj *= (1 - Probabilities[i]);
	}
	return weights;
}

double Frx(double x, int r)
{
	double result = 1;
	double currentX = x;
	for(int i = 1; i < r; i++)
	{
		currentX *= x;
		result -= currentX;
	}
	return result;
}
static double RootFinder(int r, double precision)
{
	double currentPrecision = 1;
	double left = 0;
	double right = 1;
	while(std::abs(currentPrecision) > precision)
	{
		double middle = (left + right) / 2;
		currentPrecision = Frx(middle, r);
		if(currentPrecision > 0)
		{
			left = middle;
		} else
		{
			right = middle;
		}
	}
	return (left + right) / 2;
}

struct Tree
{
	int MaxLevel;
	int MaxID;
	Node *Head;
	std::vector<Node *> StartingList;
	Tree(const std::vector<double> &weights, const std::vector<int> &IDs, bool valley = false)
	{
		MaxLevel = 0;
		MaxID = 0;
		if(valley)
		{
			FastHuTucker(weights, IDs);
		} else
		{
			SlowHuTucker(weights, IDs);
		}
		std::vector<Node *> Weights(StartingList);
		std::vector<Node *> Holder;
		while(Weights.size() > 0)
		{
			Holder.push_back(Weights[0]);
			Weights.erase(Weights.begin());
			int i = Holder.size() - 1;
			while(i > 0 && Holder[i]->Level == Holder[i - 1]->Level)
			{
				Combine(i - 1, i, Holder);
				i--;
			}
		}
		Head = Holder[0];
		MaxID = weights.size();
	}
private:
	void FastHuTucker(const std::vector<double> &weights, const std::vector<int> &IDs)
	{
		std::vector<Node *> Weights;
		for(int i = 0; i < weights.size(); i++)
		{
			Node *node = new Node(IDs[MaxID++], weights[i], i);
			Weights.push_back(node);
		}
		StartingList = std::vector<Node *>(Weights);

		std::vector<Node *> Left; //Right of Left always smallest
		std::vector<Node *> Center; //Left of Center always smallest
		std::vector<Node *> Right; //Left of Right always smallest

		{
			double Min = weights[0];
			for(int i = 1; i < weights.size(); i++)
			{
				if(Min > weights[i])
					Min = weights[i];
			}
			Left.push_back(Weights[0]);
			int i = 1;
			while(i < Weights.size() - 1 && Weights[i]->Weight > Min)
			{
				Left.push_back(Weights[i]);
				i++;
			}
			while(i < Weights.size())
			{
				Right.push_back(Weights[i]);
				i++;
			}
		}

		std::reverse(Left.begin(), Left.end()); //Makes Left go in ascending order
		{
			int count = 0;
			int i = 0;
			int j = 0;
			int k = 0;
			while(i < Left.size() || j < Right.size() || k < Center.size() - 1)
			{
				count++;
				std::vector<Node *> nodes;
				int leftCount = 0;
				if(i + 1 < Left.size())
				{
					nodes.push_back(Left[i + 1]);
					leftCount++;
				}
				if(i < Left.size())
				{
					nodes.push_back(Left[i]);
					leftCount++;
				}
				int centerCount = 0;
				if(k < Center.size())
				{
					nodes.push_back(Center[k]);
					centerCount++;
				}
				if(k + 1 < Center.size())
				{
					nodes.push_back(Center[k + 1]);
					centerCount++;
				}
				int rightCount = 0;
				if(j < Right.size())
				{
					nodes.push_back(Right[j]);
					rightCount++;
				}
				if(j + 1 < Right.size())
				{
					nodes.push_back(Right[j + 1]);
					rightCount++;
				}
				auto [min1, min2] = MinInAll(nodes);
				Center.push_back(new Node(++MaxID, nodes[min1], nodes[min2]));
				if(min1 < leftCount)
				{
					i++;
				}
				if(min2 < leftCount)
				{
					i++;
				}
				if(min2 >= leftCount && min2 < centerCount + leftCount)
				{
					k++;
				}
				if(min1 >= leftCount && min1 < centerCount + leftCount)
				{
					k++;
				}
				if(min2 >= centerCount + leftCount && min2 < rightCount + centerCount + leftCount)
				{
					j++;
				}
				if(min1 >= centerCount + leftCount && min1 < rightCount + centerCount + leftCount)
				{
					j++;
				}
			}
			//std::cout << Weights.size() << " : " << count << '\n';
			SetLevels(Center[Center.size() - 1], 0);
			Head = Center[Center.size() - 1];
		}
	}
	void SlowHuTucker(const std::vector<double> &weights, const std::vector<int> &IDs)
	{
		std::vector<Node *> Weights;
		for(int i = 0; i < weights.size(); i++)
		{
			Node *node = new Node(IDs[MaxID++], weights[i], i);
			Weights.push_back(node);
		}
		StartingList = std::vector<Node *>(Weights);
		while(Weights.size() > 1)
		{
			auto [min1, min2] = MinInAll(Weights);
			Combine(min1, min2, Weights);
		}
		SetLevels(Weights[0], 0);
		Head = Weights[0];
	}
	void SetLevels(Node *head, int level)
	{
		if(level > MaxLevel)
			MaxLevel = level;
		head->Level = level;
		head->Y = level;
		if(head->Child1 != nullptr)
		{
			SetLevels(head->Child1, level + 1);
		}
		if(head->Child2 != nullptr)
		{
			SetLevels(head->Child2, level + 1);
		}
	}
	std::tuple<int, int> MinInAll(const std::vector<Node *> &Weights)
	{
		int min1 = 0;
		int min2 = 1;
		for(int i = 0; i < Weights.size() - 1;)
		{
			int sectionStart = i;
			i++;
			while(i < Weights.size() && Weights[i]->IsCircle)
			{
				i++;
			}
			if(i == Weights.size())
			{
				i--;
			}
			auto [sMin1, sMin2] = MinInGroup(sectionStart, i, Weights);
			if(Weights[min1]->Weight + Weights[min2]->Weight > Weights[sMin1]->Weight + Weights[sMin2]->Weight)
			{
				min1 = sMin1;
				min2 = sMin2;
			}
		}
		return {min1, min2};
	}
	std::tuple<int, int> MinInGroup(int i1, int i2, const std::vector<Node *> &Weights)
	{
		int min1 = i1;
		int min2 = i1 + 1;
		if(Weights[min1]->Weight > Weights[min2]->Weight)
		{
			int hold = min1;
			min1 = min2;
			min2 = hold;
		}
		for(int i = i1 + 2; i <= i2; i++)
		{
			if(Weights[i]->Weight < Weights[min2]->Weight)
			{
				min2 = i;
				if(Weights[min1]->Weight > Weights[min2]->Weight)
				{
					int hold = min1;
					min1 = min2;
					min2 = hold;
				}
			}
		}
		return {min1, min2};
	}
	void Combine(int i1, int i2, std::vector<Node *> &Weights)
	{
		if(i1 > i2)
		{
			int hold = i1;
			i1 = i2;
			i2 = hold;
		}
		Node *parent = new Node(++MaxID, Weights[i1], Weights[i2]);
		Weights.erase(Weights.begin() + i2);
		Weights.erase(Weights.begin() + i1);
		Weights.insert(Weights.begin() + i1, parent);
	}
};