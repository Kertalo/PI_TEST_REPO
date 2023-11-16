#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <stack>
#include <unordered_set>
#include <sstream>
#include <ctime>
#include <cmath>

using namespace std;

const unsigned char countColumn = 7;
const unsigned char countRow = 6;
const unsigned char minWin = 4;

struct state
{
	vector<unsigned char> field;
	state* parent;
	priority_queue<state> kids;
	int alpha;
	int beta;
	int heuristic;
	unsigned char countOperations;
	unsigned char column;
	bool isKidsWasCreated = false;
};

bool operator<(const state& rhs, const state& lhs)
{
	return lhs.heuristic < rhs.heuristic;
}

inline bool operator==(state const& lhs, state const& rhs)
{
	if (lhs.countOperations != rhs.countOperations)
		return false;
	if (lhs.heuristic != rhs.heuristic)
		return false;
	if (lhs.column != rhs.column)
		return false;
	for (int i = 0; i < countRow * countColumn; i++)
		if (lhs.field[i] != rhs.field[i])
			return false;
	state lhs2 = lhs;
	state rhs2 = rhs;
	while (lhs2.parent != nullptr && rhs2.parent != nullptr)
	{
		lhs2 = *lhs2.parent;
		rhs2 = *rhs2.parent;
		if (lhs2.heuristic != rhs2.heuristic)
			return false;
	}
	if (lhs2.parent == nullptr && rhs2.parent == nullptr)
		return true;
	return false;
}

struct stateHash
{
	size_t operator() (const state& s) const
	{
		ostringstream os{};
		for (unsigned char i = 0; i < countRow * countColumn; i++)
			os << s.field[i] << " ";
		return hash<string>{}(os.str());
	}
};

unsigned char PutChip(vector<unsigned char>& field,
	const unsigned char column, const unsigned char player)
{
	unsigned char row = countRow;
	for (unsigned char i = 0; i < countRow; i++)
	{
		if (field[countColumn * i + column] == 0)
		{
			row = i;
			field[countColumn * i + column] = player;
			break;
		}
	}
	return row;
}

int ChangeHeuristic(unsigned char count, unsigned char countPlayer,
	unsigned char countClosestPlayer, unsigned char player)
{
	if (player == 0 || count < minWin)
		return 0;
	return player == 2 ? pow(countPlayer, 3) : -(pow(countPlayer, 3));
}

int Heuristic(const vector<unsigned char> field)
{
	int heuristic = 0;
	// vertical
	for (unsigned char i = 0; i < countColumn; i++)
	{
		unsigned char count = 0;
		unsigned char countPlayer = 0;
		unsigned char player = 0;
		for (unsigned char j = 0; j < countRow; j++)
		{
			unsigned char index = j * countColumn + i;
			if (field[index] == 0)
			{
				count += countRow - j;
				break;
			}
			else if (field[index] == player)
			{
				count++;
				countPlayer++;
			}
			else
			{
				count = 1;
				countPlayer = 1;
				player = field[index];
			}
		}
		heuristic += ChangeHeuristic(count, countPlayer, countPlayer, player);
	}
	// horizontal
	for (unsigned char i = 0; i < countRow; i++)
	{
		unsigned char count = 0;
		unsigned char countPlayer = 0;
		unsigned char countClosestPlayer = 0;
		unsigned char player = 0;
		for (unsigned char j = 0; j < countColumn; j++)
		{
			unsigned char index = i * countColumn + j;
			if (field[index] == 0)
			{
				ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
				count++;
				countClosestPlayer = 0;
			}
			else if (field[index] == player)
			{
				count++;
				countPlayer++;
				countClosestPlayer++;
			}
			else
			{
				if (player == 0)
				{
					count++;
					countPlayer++;
					countClosestPlayer++;
				}
				else
				{
					heuristic += ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
					count = 1;
					countPlayer = 1;
					countClosestPlayer = 1;
				}
				player = field[index];
			}
		}
		heuristic += ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
	}
	unsigned char countDiagonals = countColumn + countRow + 1 - 2 * minWin;
	// diagonal 1
	for (unsigned char i = 0; i < countDiagonals; i++)
	{
		unsigned char count = 0;
		unsigned char countPlayer = 0;
		unsigned char countClosestPlayer = 0;
		unsigned char player = 0;
		int row = countRow - minWin - i;
		unsigned char index = row >= 0 ? countColumn * row : -row;
		do
		{
			if (field[index] == 0)
			{
				ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
				count++;
				countClosestPlayer = 0;
			}
			else if (field[index] == player)
			{
				count++;
				countPlayer++;
				countClosestPlayer++;
			}
			else
			{
				if (player == 0)
				{
					count++;
					countPlayer++;
					countClosestPlayer++;
				}
				else
				{
					heuristic += ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
					count = 1;
					countPlayer = 1;
					countClosestPlayer = 1;
				}
				player = field[index];
			}
			index += countColumn + 1;
		} 
		while (index % countColumn != 0 &&
			index < countColumn * countRow);
		heuristic += ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
	}
	// diagonal 2
	for (unsigned char i = 0; i < countDiagonals; i++)
	{
		unsigned char count = 0;
		unsigned char countPlayer = 0;
		unsigned char countClosestPlayer = 0;
		unsigned char player = 0;
		int row = -minWin + 1 + i;
		unsigned char index = row >= 0 ? 
			countColumn * (row + 1) - 1 : countColumn + row;
		do
		{
			if (field[index] == 0)
			{
				ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
				count++;
				countClosestPlayer = 0;
			}
			else if (field[index] == player)
			{
				count++;
				countPlayer++;
				countClosestPlayer++;
			}
			else
			{
				if (player == 0)
				{
					count++;
					countPlayer++;
					countClosestPlayer++;
				}
				else
				{
					heuristic += ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
					count = 1;
					countPlayer = 1;
					countClosestPlayer = 1;
				}
				player = field[index];
			}
			index += countColumn - 1;
		} 
		while ((index + 1) % countColumn != 0 &&
			index < countColumn * countRow);
		heuristic += ChangeHeuristic(count, countPlayer, countClosestPlayer, player);
	}
	return heuristic;
}

bool IsWin(vector<unsigned char> field)
{
	// vertical
	for (unsigned char i = 0; i < countColumn; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		for (unsigned char j = 0; j < countRow; j++)
		{
			unsigned char index = j * countColumn + i;
			if (field[index] == 0)
				break;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
		}
	}
	// horizontal
	for (unsigned char i = 0; i < countRow; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		for (unsigned char j = 0; j < countColumn; j++)
		{
			unsigned char index = i * countColumn + j;
			if (field[index] == 0)
				count = 0;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
		}
	}
	unsigned char countDiagonals = countColumn + countRow + 1 - 2 * minWin;
	// diagonal 1
	for (unsigned char i = 0; i < countDiagonals; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		int row = countRow - minWin - i;
		unsigned char index = row >= 0 ? countColumn * row : -row;
		do
		{
			if (field[index] == 0)
				count = 0;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
			index += countColumn + 1;
		} while (index % countColumn != 0 &&
			index < countColumn * countRow);
	}
	// diagonal 2
	for (unsigned char i = 0; i < countDiagonals; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		int row = -minWin + 1 + i;
		unsigned char index = row >= 0 ?
			countColumn * (row + 1) - 1 : countColumn + row;
		do
		{
			if (field[index] == 0)
				count = 0;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
			index += countColumn - 1;
		} while ((index + 1) % countColumn != 0 &&
			index < countColumn * countRow);
	}
	return false;
}

bool IsFinish(vector<unsigned char> field, unsigned char &value)
{
	unsigned char countClose = 0;
	for (unsigned char i = 0; i < countColumn; i++)
		if (field[countColumn * (countRow - 1) + i] != 0)
			countClose++;
		else
			break;
	if (countClose == countColumn)
	{
		value = 4;
		return true;
	}
	// vertical
	for (unsigned char i = 0; i < countColumn; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		for (unsigned char j = 0; j < countRow; j++)
		{
			unsigned char index = j * countColumn + i;
			if (field[index] == 0)
				break;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
		}
	}
	// horizontal
	for (unsigned char i = 0; i < countRow; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		for (unsigned char j = 0; j < countColumn; j++)
		{
			unsigned char index = i * countColumn + j;
			if (field[index] == 0)
				count = 0;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
		}
	}
	unsigned char countDiagonals = countColumn + countRow + 1 - 2 * minWin;
	// diagonal 1
	for (unsigned char i = 0; i < countDiagonals; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		int row = countRow - minWin - i;
		unsigned char index = row >= 0 ? countColumn * row : -row;
		do
		{
			if (field[index] == 0)
				count = 0;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
			index += countColumn + 1;
		} while (index % countColumn != 0 &&
			index < countColumn * countRow);
	}
	// diagonal 2
	for (unsigned char i = 0; i < countDiagonals; i++)
	{
		unsigned char count = 0;
		unsigned char player = 0;
		int row = -minWin + 1 + i;
		unsigned char index = row >= 0 ?
			countColumn * (row + 1) - 1 : countColumn + row;
		do
		{
			if (field[index] == 0)
				count = 0;
			else if (field[index] == player)
			{
				count++;
				if (count >= minWin)
					return true;
			}
			else
			{
				count = 1;
				player = field[index];
			}
			index += countColumn - 1;
		} while ((index + 1) % countColumn != 0 &&
			index < countColumn * countRow);
	}
	return false;
}

void DrawField(vector<unsigned char> field)
{
	for (int i = countRow - 1; i >= 0; i--)
	{
		cout << endl;
		for (unsigned char j = 0; j < countColumn; j++)
		{
			if (field[countColumn * i + j] == 0)
				cout << " " << (char)176 << (char)176;
			else if (field[countColumn * i + j] == 1)
				cout << " " << (char)219 << (char)219;
			else
				cout << " " << (char)206 << (char)206;
		}
		cout << endl;
	}
	cout << endl;
}

unsigned char AlphaBeta(vector<unsigned char> field)
{
	//srand(time(0));
	bool isWin;
	state dynamicState{ field, nullptr, {}, -500, 500, Heuristic(field), 0};
	unordered_set<state, stateHash> setStates;
	vector<unsigned char> goodMoves{};
	setStates.insert(dynamicState);

	int bound = 4;
	int i = 0;
	
	while (true)
	{
		state* setState = const_cast<state*>(&(*setStates.find(dynamicState)));
		//cout << "heur = " << dynamicState.heuristic;
		//cout << ". A = " << dynamicState.alpha;
		//cout << ". B = " << dynamicState.beta << endl;
		if (setState->alpha > setState->beta)
		{
			dynamicState = *setState->parent;
			continue;
		}
		if (setState->countOperations == bound || IsWin(setState->field))
		{
			i++;
			//cout << endl << heuristic << endl;
			//DrawField(dynamicState.field);
			if (setState->countOperations % 2 == 1)
				setState->beta = setState->heuristic;
			else
				setState->alpha = setState->heuristic;
			//cout << "wow" << endl;
			setState->isKidsWasCreated = true;
		}
		if (setState->kids.size() == 0)
		{
			if (setState->isKidsWasCreated)
			{
				if (setState->parent == nullptr)
					break;
				//cout << "lol" << endl;
				if (setState->countOperations % 2 == 1)
				{
					if (setState->beta > setState->parent->alpha)
					{
						if (setState->countOperations == 1)
						{
							//if (heuristic != dynamicState.parent->alpha)
							goodMoves.clear();
							//if (heuristic > dynamicState.parent->alpha)
							//{
							//	goodMoves.clear();
							goodMoves.push_back(setState->column);
							//}
						}
						setState->parent->alpha = setState->beta;
						//dynamicState.parent->heuristic = heuristic;
					}
				}
				else
					if (setState->alpha < setState->parent->beta)
					{
						setState->parent->beta = setState->alpha;
					}
				dynamicState = *setState->parent;
				

				/*if (dynamicState.countOperations != 0)
				{
					if (dynamicState.countOperations % 2 == 0)
					{
						if (heuristic > dynamicState.parent->beta)
							dynamicState = *setState->parent->parent;
					}
					else
					{
						if (heuristic < dynamicState.parent->alpha)
							dynamicState = *setState->parent->parent;
					}
				}*/
				continue;
			}
			//cout << "1--------" << endl;
			//cout << (int)setState->countOperations << endl;
			//cout << "2--------" << endl;
			setState->isKidsWasCreated = true;
			priority_queue<state> kids{};
			for (unsigned char i = 0; i < countColumn; i++)
				if (setState->field[countColumn * (countRow - 1) + i] == 0)
				{
					auto newField = setState->field;
					PutChip(newField, i, -(setState->countOperations % 2) + 2);
					bool isWin = false;
					int newHeuristic = Heuristic(newField);
					state newState{ newField, setState, {}, setState->alpha,
						setState->beta, newHeuristic, setState->countOperations + 1, i };
					
					//cout << "3--------" << endl;
					//cout << Heuristic(newField) << endl;
					//cout << "4--------" << endl;
					setStates.insert(newState);
					kids.push(newState);
				}
			//cout << "--------" << endl;
			setState->kids = kids;
		}
		dynamicState = setState->kids.top();
		dynamicState.alpha = setState->alpha;
		dynamicState.beta = setState->beta;
		setStates.erase(setState->kids.top());
		setState->kids.pop();
		setStates.insert(dynamicState);
		
		//i--;
		//if (i == 0)
		//	break;
	}
	//cout << i << endl;
	//cout << "--------" << endl;
	//cout << (int)dynamicState.countOperations << endl;
	
	//for (auto x : goodMoves)
	//	cout << (int)x << endl;
	return goodMoves[rand() % goodMoves.size()];
}

void ComputerTurn(vector<unsigned char>& field, stack<unsigned char>& allTurns)
{
	cout << "Computer turn: ";
	unsigned char column;
	if (allTurns.empty())
		column = rand() % countColumn;
	else
		column = AlphaBeta(field);
	cerr << (int)column << endl;
	allTurns.push(column);
	PutChip(field, column, 2);
	DrawField(field);
}

void PlayerTurn(vector<unsigned char>& field, stack<unsigned char>& allTurns)
{
	cout << "Your turn: ";
	int column;
	cin >> column;
	if (column >= 0 && column < countColumn)
	{
		if (PutChip(field, column, 1) == countRow)
		{
			cout << endl << "Column is full" << endl << endl;
			PlayerTurn(field, allTurns);
			return;
		}
		allTurns.push(column);
	}
	else if (!allTurns.empty())
	{
		unsigned char turn = allTurns.top();
		allTurns.pop();
		if (!allTurns.empty())
		{
			for (unsigned char i = countRow; i >= 0; i--)
				if (field[i * countColumn + turn] != 0)
				{
					field[i * countColumn + turn] = 0;
					break;
				}
			turn = allTurns.top();
			allTurns.pop();
			for (unsigned char i = countRow; i >= 0; i--)
				if (field[i * countColumn + turn] != 0)
				{
					field[i * countColumn + turn] = 0;
					break;
				}
			DrawField(field);
			PlayerTurn(field, allTurns);
			return;
		}
		else
		{
			allTurns.push(turn);
			DrawField(field);
			PlayerTurn(field, allTurns);
			return;
		}
	}
	else
	{
		DrawField(field);
		PlayerTurn(field, allTurns);
		return;
	}
	DrawField(field);
}

int main()
{
	srand(time(0));
	/*vector<unsigned char> field
	{
		1, 0, 2, 2, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0
	};*/

	vector<unsigned char> field;
	for (unsigned char i = 0; i < countColumn * countRow; i++)
		field.push_back(0);

	DrawField(field);
	
	stack<unsigned char> allTurns;
	
	cout << "Choose your color (0 or 1): ";
	int turn;
	cin >> turn;
	cout << endl;
	if (turn == 0)
		PlayerTurn(field, allTurns);
	turn = 1;
	while (true)
	{
		unsigned char finishValue = 0;
		if (IsFinish(field, finishValue))
		{
			if (finishValue == 4)
				return finishValue;
			return turn % 2 == 0 ? 0 : 3;
		}
		if (turn % 2 == 1)
			ComputerTurn(field, allTurns);
		else
			PlayerTurn(field, allTurns);
		turn++;
	}
}