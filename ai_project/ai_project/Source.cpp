// ⁄„· «·ÿ·«»:
// Õ« „ „Õ„œ ⁄Ìœ Õ·„Ì «·Â‘·„Ê‰ 211139
// „‰Ãœ ‰«∆· —»⁄Ì «· „Ì„Ì 211069

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <stack>
using namespace std;

const int stateSize = 7;
const int boxValue = 50;
const int goalValue = 5000;
vector<vector<double>> Qtable(0, vector<double>(4));
vector<vector<double>> Rtable(0, vector<double>(4));
map<vector<vector<int>>, int> states;
int nextState = 0;

int dx[] = { 1, -1, 0, 0 };
int dy[] = { 0, 0, 1, -1 };

vector<vector<int>> init()
{
	vector<vector<int>> state = {
		   {1,1,1,1,1,1,1},
		   {1,0,0,0,3,0,1},
		   {1,2,0,0,2,0,1},
		   {1,0,1,0,0,2,1},
		   {1,3,0,0,0,3,1},
		   {1,0,0,0,0,0,1},
		   {1,1,1,1,1,1,1}
	};

	return state;
}

bool isGoal(vector<vector<int>> state)
{
	bool yes = true;
	for (int i = 0; i < state.size(); i++)
	{
		for (int j = 0; j < state[i].size(); j++)
		{
			if (state[i][j] == 2)
			{
				yes = false;
				break;
			}
		}

		if (!yes)
			break;
	}

	return yes;
}

bool isDeadlock(vector<vector<int>> state)
{

	int xSize = state.size();
	int ySize = state[0].size();
	vector<pair<int, int>> boxes;
	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			if (state[i][j] == 2)
			{
				boxes.push_back({ i, j });
			}
		}
	}

	for (auto box : boxes) {
		int count = 0;
		bool xchange = false, ychange = false;
		for (int i = 0; i < 4; i++)
		{
			int x = box.first, y = box.second;

			if (state[x + dx[i]][y + dy[i]] == 1 || state[x + dx[i]][y + dy[i]] == 2 || state[x + dx[i]][y + dy[i]] == 4)
			{
				count++;


				if (xchange && dx[i] != 0)
					count--;
				else if (ychange && dy[i] != 0)
					count--;

				if (dx[i] != 0)
				{
					xchange = true;
					ychange = false;
				}
				else
				{
					xchange = false;
					ychange = true;
				}
			}

		}

		if (count > 1)
			return true;
	}

	return false;
}

int getGoalCount(vector<vector<int>> state)
{
	int goalCount = 0;
	for (int i = 0; i < stateSize; i++)
	{
		for (int j = 0; j < stateSize; j++)
		{
			if (state[i][j] == 4)
				goalCount++;
		}
	}

	return goalCount;
}

void initializeStateStructures(vector<vector<int>> state)
{
	states[state] = nextState;
	vector<double> init = { 0,0,0,0 };
	Qtable.push_back(init);
	Rtable.push_back(init);

	nextState++;
}

vector<vector<int>> generateChild(vector<vector<int>> state, int x, int y, bool& boxGoal, int action)
{
	int stateNum = states[state];
	int nextX = x + dx[action];
	int nextY = y + dy[action];

	bool valid = nextX >= 0 && nextX < state.size() && nextY >= 0 && nextY < state[x].size();
	if (!valid)
		return state;

	vector<vector<int>> child = state;
	switch (state[nextX][nextY])
	{
	case 0:
		child[x][y] = 0;
		child[nextX][nextY] = 5;
		break;
	case 2:
		int beyondX = x + 2 * dx[action];
		int beyondY = y + 2 * dy[action];

		valid = beyondX >= 0 && beyondX < state.size() && beyondY >= 0 && beyondY < state[x].size();
		if (!valid)
			break;

		switch (state[beyondX][beyondY])
		{
		case 0:
			child[x][y] = 0;
			child[nextX][nextY] = 5;
			child[beyondX][beyondY] = 2;
			break;
		case 3:
			child[x][y] = 0;
			child[nextX][nextY] = 5;
			child[beyondX][beyondY] = 4;

			boxGoal = true;
			break;
		}
		break;
	}

	return child;
}

vector< vector<vector<int>> > generateChildren(vector<vector<int>>& state)
{
	vector <vector<vector<int>>> children;
	int stateNum = states[state];

	int x = -1, y = -1;
	for (int i = 0; i < state.size(); i++)
	{
		for (int j = 0; j < state[i].size(); j++)
		{
			if (state[i][j] == 5)
			{
				x = i, y = j;
				break;
			}
		}

		if (x != -1)
			break;
	}

	int nextX, nextY;
	for (int i = 0; i < 4; i++)
	{
		bool boxGoal = false;
		vector<vector<int>> child = generateChild(state, x, y, boxGoal, i);

		if (boxGoal)
		{
			if (isGoal(child))
				Rtable[stateNum][i] = goalValue;
			else
				Rtable[stateNum][i] = boxValue;
		}

		if (states[child] == 0)
		{
			initializeStateStructures(child);
		}

		children.push_back(child);
	}

	return children;
}

vector<pair<int, int>> getAvailableStates(vector<vector<int>> state)
{
	vector<pair<int, int>> available;
	for (int i = 0; i < state.size(); i++)
	{
		for (int j = 0; j < state[i].size(); j++)
		{
			if (state[i][j] == 0)
				available.push_back({ i, j });
		}
	}

	return available;
}

void QLearn(vector<vector<int>> emptyState, int iterations, double gamma)
{
	vector<pair<int, int>> availableStates = getAvailableStates(emptyState);
	vector<vector<int>> state;
	for (int i = 0; i < iterations; i++)
	{
		state = emptyState;
		pair<int, int> agentLoc = availableStates[rand() % availableStates.size()];
		state[agentLoc.first][agentLoc.second] = 5;
		initializeStateStructures(state);
		do
		{
			int stateNum = states[state];
			vector< vector<vector<int>> > children = generateChildren(state);

			vector<int> possibleActions;
			for (int j = 0; j < 4; j++)
			{
				if (children[j] != state)
					possibleActions.push_back(j);
			}

			int action = possibleActions[rand() % possibleActions.size()];
			auto nextState = children[action];
			int nextStateNum = states[nextState];

			int reward = Rtable[stateNum][action];
			double mxNext = 0;
			for (auto a : Qtable[nextStateNum])
			{
				mxNext = max(mxNext, a);
			}

			Qtable[stateNum][action] = reward + ceil(gamma * mxNext);
			state = nextState;
			if (isGoal(state))
			{
				break;
			}

			//for (auto x : state)
			//{
			//	for (auto y : x)
			//		cout << y << " ";
			//	cout << endl;
			//}
			cout << i << endl;
		} while (!isDeadlock(state));
	}
}

int cc = 0;
void playGame(vector<vector<int>> state, stack< vector<vector<int>> >& Steps)
{
	if (cc > 100)
		return;
	cc++;
	if (isDeadlock(state))
		return;

	if (isGoal(state))
	{
		Steps.push(state);
		return;
	}

	int action = 0;
	double mxNext = -1;

	int stateNum = states[state];
	for (int i = 0; i < 4; i++)
	{
		if (Qtable[stateNum][i] > mxNext)
		{
			mxNext = Qtable[stateNum][i];
			action = i;
		}
	}

	int x = -1, y = -1;
	for (int i = 0; i < state.size(); i++)
	{
		for (int j = 0; j < state[i].size(); j++)
		{
			if (state[i][j] == 5)
			{
				x = i, y = j;
				break;
			}
		}

		if (x != -1)
			break;
	}

	bool temp = false;
	vector<vector<int>> nextState = generateChild(state, x, y, temp, action);
	playGame(nextState, Steps);
	Steps.push(state);
}

int main()
{
	srand((unsigned)time(NULL));
	vector<vector<int>> state = init();

	QLearn(state, 200, 0.8);

	//for (int i = 0; i < Qtable.size(); i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		cout << Qtable[i][j] << " ";
	//	}
	//	cout << endl << endl;
	//}

	vector<pair<int, int>> availableStates = getAvailableStates(state);
	pair<int, int> agentLoc = availableStates[rand() % availableStates.size()];
	state[agentLoc.first][agentLoc.second] = 5;


	int c = 5;
	while (c--)
	{
		cc = 0;
		stack< vector<vector<int>> > Steps;
		playGame(state, Steps);

		while (!Steps.empty())
		{
			for (int i = 0; i < state.size(); i++)
			{
				for (int j = 0; j < state[i].size(); j++)
				{
					cout << Steps.top()[i][j] << " ";
				}
				cout << endl << endl;
			}

			for (int i = 0; i < 4; i++)
			{

			}
			cout << endl;

			//cout << " DONE " << endl;
			Steps.pop();
		}
	}
}