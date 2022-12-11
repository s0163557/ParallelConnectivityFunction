#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>
using namespace std;

void PrintGraph(vector<vector<int>> Graph)
{
	for (int i = 0; i < Graph.size(); i++)
	{
		cout << i << ":";
		for (int j = 0; j < Graph[i].size(); j++)
			cout << Graph[i][j] << " ";
		cout << endl;
	}
}

vector<vector<int>> CreatedGraph(int VertexCount)
{
	vector<vector<int>> Graph;
	int x;
	Graph.resize(VertexCount);
	for (int i = 0; i < VertexCount; i++)
	{
		cout << i << ":";
		for (int j = 0; j < VertexCount; j++)
		{
			cin >> x;
			if (x < 0)
				break;
			Graph[i].push_back(x);
		}
	}
	return Graph;
}

vector<vector<int>> RandomGraph(int VertexCount, int EdgeCount)
{
	srand(time(0));
	vector<vector<int>> Graph;
	int i, j, vert1, vert2;
	Graph.resize(VertexCount);
	for (i = 0; i < VertexCount; i++)
		Graph[i].resize(VertexCount, -1); // -1 означает пустое место

	//Генерировать будем таким образом: в каждом узле номер соединяемого узла стоит на своём же месте - нужно будет для более быстрой генерации графа
	while (EdgeCount > 0)
	{
		vert1 = rand() % VertexCount;//выбирается случайно первая вершина - в которую будет происходить запись
		if (Graph[vert1].size() != VertexCount - 1)
		{
			vert2 = rand() % VertexCount; //Если в первой найденной вершине осталось свободное место, то случайно выбираем место для ребра
			if (Graph[vert1][vert2] == -1 && vert1 != vert2)
			{
				Graph[vert1][vert2] = vert2;
				Graph[vert2][vert1] = vert1; //Если добавим эту строку, то получим неориентированный граф с кольцевыми рёбрами.
				EdgeCount--;
			}
		}
	}

	//Удалим -1
	for (i = 0; i < Graph.size(); i++)
	{

		for (j = 0; j < Graph[i].size(); j++)
			if (Graph[i][j] == -1)
			{
				Graph[i].erase(next(Graph[i].begin(), j)); // удаление элемента
				j--; // чтобы не убежать вперёд удаляемого размера
			}
	}

	return Graph;
}

void PullTogetherVetices(vector<vector<int>>* Graph, vector<vector<int>>* EdgeWeights, int Vert1, int Vert2)
{

	for (int i = 0; i < (*EdgeWeights).size(); i++) // Сначала изменим матрицу весов
	{
		(*EdgeWeights)[Vert1][i] += (*EdgeWeights)[Vert2][i];
		(*EdgeWeights)[i][Vert1] += (*EdgeWeights)[Vert2][i]; //Это если что можно убрать, подумай
		(*EdgeWeights)[Vert2][i] = 0;
		(*EdgeWeights)[i][Vert2] = 0;
		(*EdgeWeights)[Vert1][Vert1] = 0;
	}

	for (int i = 0; i < (*Graph).size(); i++) //Теперь удалим включения объединямой вершины
	{
		for (int j = 0; j < (*Graph)[i].size(); j++)
		{
			if ((*Graph)[i][j] == Vert2)
			{
				(*Graph)[i].erase((*Graph)[i].begin() + j);
				j--;
			}

		}
	}

	(*Graph)[Vert2].clear();
	(*Graph)[Vert1].clear();

	for (int i = 0; i < (*EdgeWeights)[Vert1].size(); i++) // добавим те рёбра, которых могло не быть в Vert1
	{
		if ((*EdgeWeights)[Vert1][i] != 0 && i != Vert1)
		{
			(*Graph)[Vert1].push_back(i);

			bool Check = 0;
			for (int j = 0; j < (*Graph)[(*Graph)[Vert1].size() - 1].size(); j++)//т.е. мы проверяем последний добавленный элемент нужно ли в нем обновить вершины
			{
				if ((*Graph)[(*Graph)[Vert1].size() - 1][j] == Vert1)
					Check = 1;
			}
			if (Check == 0)
				(*Graph)[(*Graph)[Vert1].size() - 1].push_back(Vert1);

		}

	}

}

vector<int> AssistAlgShtorVagner(int VertexCount, int EdgeCount, vector<vector<int>> Graph, vector<vector<int>> EdgeWeights, int Missing)
{
	//Assist - помогает в графе создать множество A и понять какие вершины мы будем объединять
	//Выберем произвольную вершину из графа
	vector<int> Visited, Weight; //Visited = A
	Weight.resize(Graph.size()); //нулём обозначены вершины, которые мы ещё не посетили.
	vector<bool> InVisited;
	InVisited.resize(Graph.size());

	for (int k = 0; k < Graph.size(); k++)
		if (Graph[k].size() != 0)
		{
			Visited.push_back(k);
			InVisited[k] = 1;
			break;
		}

	for (int i = 0; i < Graph[Visited[0]].size(); i++)//добавим стартовые веса
		Weight[Graph[Visited[0]][i]] += EdgeWeights[Visited[0]][Graph[Visited[0]][i]];

	int Max = 0, Vert = -1;
	for (int i = 0; i < Graph.size() - Missing; i++) // -Misssing, потому что на уже удалённые+добавленные вершины мы не смотрим
	{
		Max = 0;
		Vert = -1;
		for (int c = 0; c < Weight.size(); c++)
			if (Max < Weight[c] && InVisited[c] != 1)
			{
				Max = Weight[c];
				Vert = c;
			}

		Visited.push_back(Vert);
		InVisited[Vert] = 1;

		for (int j = 0; j < Graph[Vert].size(); j++)//добавим веса добавленной в множество вершины
			Weight[Graph[Vert][j]] += EdgeWeights[Vert][Graph[Vert][j]];
	}
	vector<int>Result;
	Result.push_back(Max);
	Result.push_back(Visited[Visited.size() - 2]);
	Result.push_back(Visited[Visited.size() - 1]);

	return Result;
}

int AlgShtorVagner(vector<vector<int>> Graph, int VertexCount, int EdgeCount)
{
	vector<vector<int>> EdgeWeights;
	EdgeWeights.resize(Graph.size());
	for (int c = 0; c < Graph.size(); c++)
	{
		EdgeWeights[c].resize(Graph.size());
		for (int k = 0; k < Graph[c].size(); k++)
			EdgeWeights[c][Graph[c][k]] = 1; //Заполним веса пересечений вершин.

	}

	int i, Min = INT_MAX;
	vector<int> Result;
	vector<vector<int>> GraphCopy;
	GraphCopy.resize(Graph.size());
	for (i = 0; i < Graph.size(); i++)
	{
		GraphCopy[i].resize(Graph[i].size());
		for (int k = 0; k < Graph[i].size(); k++)
			GraphCopy[i][k] = Graph[i][k];
	}

	i = 0;
	for (i = 1; i < GraphCopy.size() - 1; i++)
	{
		Result = AssistAlgShtorVagner(VertexCount, EdgeCount, GraphCopy, EdgeWeights, i);

		PullTogetherVetices(&GraphCopy, &EdgeWeights, Result[1], Result[2]);

		if (Result[0] < Min)
			Min = Result[0];

	}

	// cout << endl << Min;
	return Min;
}
/*
void AssistCutVertexFinder(vector<vector<int>> Graph, int Start, vector<bool>* Visited, vector<bool>* IsCutVertex, vector<int>* Discover, vector<int>* Low, int Parent, int* Depth)
{
	int Children = 0;
	(*Visited)[Start] = 1;
	(*Discover)[Start] = (*Low)[Start] = ++(*Depth);

	cout << endl;
	for (int i = 0; i < (*Discover).size(); i++)
	{
		cout << (*Discover)[i] << " ";
	}
	cout << endl;

	for (int i = 0; i < Graph[Start].size(); i++)
	{
		if ((*Visited)[Graph[Start][i]] == 0)
		{
			Children++;
			AssistCutVertexFinder(Graph, Graph[Start][i], Visited, IsCutVertex, Discover, Low, Start, Depth);
			(*Low)[Start] = min((*Low)[Start], (*Low)[Graph[Start][i]]);
			if (Parent != -1 && (*Low)[Graph[Start][i]] > (*Low)[Start])
				(*IsCutVertex)[Graph[Start][i]] = 1;
		}

		else if (Graph[Start][i] != Parent)
			(*Low)[Start] = min((*Low)[Start], (*Discover)[Graph[Start][i]]);

	}

	if (Parent == -1 && Children > 1)
		(*IsCutVertex)[Start] = 1;

}

vector<int> CutVertexFinder(vector<vector<int>> Graph)
{
	vector<int> Discover, Low;
	vector<bool> Visited, IsCutVertex;
	int Depth = 0, Parent = -1;
	Discover.resize(Graph.size());
	Low.resize(Graph.size());
	Visited.resize(Graph.size());
	IsCutVertex.resize(Graph.size());

	for (int i = 0; i < Graph.size(); i++)
		if (Visited[i] == 0)
			AssistCutVertexFinder(Graph, i, &Visited, &IsCutVertex, &Discover, &Low, Parent, &Depth);

	vector<int> CutVertex;
	for (int j = 0; j < IsCutVertex.size(); j++)
		if (IsCutVertex[j] == 1)
			CutVertex.push_back(j);

	cout << endl;
	for (int j = 0; j < CutVertex.size(); j++)
		cout << CutVertex[j] << " ";

	return CutVertex;
}
*/
void DelEdge(vector<vector<int>>* Graph, int Start)
{
	for (int i = 0; i < (*Graph)[Start].size(); i++)
	{
		int From = (*Graph)[Start][i];
		for (int j = 0; j < (*Graph)[From].size(); j++)
			if ((*Graph)[From][j] == Start)
			{
				(*Graph)[From].erase(next((*Graph)[From].begin(), j));
				j--;
			}

		(*Graph)[Start].erase(next((*Graph)[Start].begin(), i));
		i--;
	}
}

void FindAllVertexCut(vector<vector<int>>* Graph, vector<int>* DelVer, int Max, int Start, int End)
{

	for (int i = Start; i < End; i++)
	{
		int VertexCount = 0;
		int EdgeCount = 0;
		vector<vector<int>> GraphCopy;
		GraphCopy.resize((*Graph).size());
		for (int j = 0; j < (*Graph).size(); j++)
		{
			GraphCopy[j].resize((*Graph)[j].size());
			for (int c = 0; c < (*Graph)[j].size(); c++)
				GraphCopy[j][c] = (*Graph)[j][c];

		}
		DelEdge(&GraphCopy, i);
		GraphCopy.erase(next(GraphCopy.begin(), i));
		for (int c = 0; c < GraphCopy.size(); c++)
			for (int j = 0; j < GraphCopy[c].size(); j++)
				if (GraphCopy[c][j] >= i)
					GraphCopy[c][j]--;

		//   cout << endl;
		   //PrintGraph(GraphCopy);
		for (int j = 0; j < GraphCopy.size(); j++)
			EdgeCount += GraphCopy[j].size();
		EdgeCount /= 2;
		// cout << GraphCopy.size()<< " " << EdgeCount;
		(*DelVer)[i] = Max - AlgShtorVagner(GraphCopy, GraphCopy.size(), EdgeCount);
		GraphCopy.clear();
	}

}

int main()
{
	setlocale(LC_ALL, "rus");
	for (int O = 40; O < 80; O++)
	{
		int i, j;
		int VertexCount = O;
		//cout << "Введите количество вершин неориентированного графа:";
		//cin >> VertexCount;

		int EdgeCount = 0;
		for (i = 1; i < VertexCount; i++)
			EdgeCount += VertexCount - i;

		//cout << endl << "Введите количество рёбер(максимум для заданного графа - " << EdgeCount << "):";
		//cin >> EdgeCount; // перезапишем, чтоб не гонять лишнюю память
		//EdgeCount = 15 * O;
		vector<vector<int>> Graph;

		//cout << "Граф задавать случайным образом(1), или с клавиатуры(0)?" << endl;
		int check;
		//cin >> check;
		Graph = RandomGraph(VertexCount, EdgeCount);
		//if (check == 0) Graph = CreatedGraph(VertexCount);
		//cout << endl;
		//PrintGraph(Graph);
		int Max;
		Max = AlgShtorVagner(Graph, VertexCount, EdgeCount);
		//cout << endl << Max << endl;
		if (Max == 1)
		{
			cout << "Граф имеет минимальный разрез равный 1, поэтому у него нет пар связности.";
		}
		vector<int> CutVertex;
		//CutVertex = CutVertexFinder(Graph);
		//cout << endl << "Del Edge";
		vector<int> DelVer;
		DelVer.resize(Graph.size());
		auto Begin = chrono::steady_clock::now();
		/*
		thread T1(FindAllVertexCut, &Graph, &DelVer, Max, 0, Graph.size() / 4);
		thread T2(FindAllVertexCut, &Graph, &DelVer, Max, Graph.size() / 4, Graph.size() / 2);
		thread T3(FindAllVertexCut, &Graph, &DelVer, Max, Graph.size() / 2, (3 * Graph.size()) / 4);
		thread T4(FindAllVertexCut, &Graph, &DelVer, Max, (3 * Graph.size()) / 4, Graph.size());
		T1.join();
		T4.join();
		T3.join();
		T2.join();
		*/
		auto End = chrono::steady_clock::now();
		auto Difference = chrono::duration_cast<std::chrono::milliseconds>(End - Begin);
		//cout << "Время для четырёх потоков: " << Difference.count() << " ms\n" << endl;

		Begin = chrono::steady_clock::now();
		thread T8(FindAllVertexCut, &Graph, &DelVer, Max, 0, Graph.size() / 3);
		thread T9(FindAllVertexCut, &Graph, &DelVer, Max, Graph.size() / 3, (2 * Graph.size()) / 3);
		thread T10(FindAllVertexCut, &Graph, &DelVer, Max, (2 * Graph.size()) / 3, Graph.size());
		T8.join();
		T9.join();
		T10.join();
		End = chrono::steady_clock::now();
		Difference = chrono::duration_cast<std::chrono::milliseconds>(End - Begin);
		cout << O << " " << Difference.count() << " ";

		Begin = chrono::steady_clock::now();
		thread T6(FindAllVertexCut, &Graph, &DelVer, Max, 0, Graph.size() / 2);
		thread T7(FindAllVertexCut, &Graph, &DelVer, Max, Graph.size() / 2, Graph.size());
		T6.join();
		T7.join();
		End = chrono::steady_clock::now();
		Difference = chrono::duration_cast<std::chrono::milliseconds>(End - Begin);
		cout << Difference.count() << " ";

		DelVer.resize(Graph.size());
		Begin = chrono::steady_clock::now();
		thread T5(FindAllVertexCut, &Graph, &DelVer, Max, 0, Graph.size());
		T5.join();
		End = chrono::steady_clock::now();
		Difference = chrono::duration_cast<std::chrono::milliseconds>(End - Begin);
		cout << Difference.count() << endl;

		//cout << endl;
		//for (i = 0; i < DelVer.size(); i++)
		//	cout << DelVer[i] << " ";
		vector<vector<int>> ConnectivityPair;
		ConnectivityPair.resize(Max + 1);
		ConnectivityPair[0].resize(Max, 1);

		int DelMax = 0;
		for (i = 1; i < Max; i++)
		{
			//Найдем максимальное количество ребёр, которые можем удалить
			int temp = 0, DelVert = -1;
			ConnectivityPair[i].resize(Max, 0);
			for (j = 0; j < DelVer.size(); j++)
				if (temp < DelVer[j])
				{
					temp = DelVer[j];
					DelVert = j;
				}
			DelMax += temp;

			if (DelVert != -1)
			{
				DelVer[DelVert] = 0;
				//Удалим уже рассмотренную вершину из рассмотрения других вершин
				for (j = 0; j < Graph[DelVert].size(); j++)
					if (DelVer[Graph[DelVert][j]] > 0)
						DelVer[Graph[DelVert][j]]--;
			}

			for (j = 0; j < Max - max(DelMax, i); j++)//Нам в любом случае нужно будет удалять i вершин, ведь у нас становится меньше вершин
				ConnectivityPair[i][j] = 1;
		}
		/*
				cout << endl;
				cout << "Искомые пары связностей(кол-во вершин,кол-во рёбер):";
				for (i = 1; i < ConnectivityPair.size(); i++)
					for (j = 1; j < ConnectivityPair[i].size(); j++)
						if (ConnectivityPair[i][j] == 0 && ConnectivityPair[i][j - 1] == 1 && ConnectivityPair[i - 1][j] == 1)
							cout << "(" << i << "," << j << ")";
		*/
	}

	return 0;
}
//Почему он на этом входе выдает 2, а не 1? Дурость. Поразбирайся, будь любезен.
/*
7
8
1 2 -1
0 3 -1
0 3 -1
1 2 4 -1
3 5 6 -1
4 6 -1
4 5 -1
*/

//Интересный случай. Почему он всё ломает? Или не ломает?
/*
1 7 2 -1
7 3 0 -1
0 4 -1
1 5 -1
2 5 6 -1
4 3 6 -1
4 5 -1
0 1 -1
*/

/*
2 1 4 3 -1
0 2 4 -1
0 1 -1
0 4 5 -1
5 3 0 1 -1
3 4 -1
*/

//Здесь тоже и количество рёбер и вершина находится неправильно
/*
8
14
6 1 2 5 -1
0 6 5 3 -1
0 3 7 4 -1
1 2 4 7 -1
2 3 7 -1
6 0 1 -1
0 1 5 -1
4 2 3 -1
*/

/*
8
15
6 1 2 5 3 -1
0 6 5 3 -1
0 3 7 4 -1
1 2 4 7 0 -1
2 3 7 -1
6 0 1 -1
0 1 5 -1
4 2 3 -1
*/