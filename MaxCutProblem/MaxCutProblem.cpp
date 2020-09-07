#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
using namespace std;

struct Graph
{
	int size;
	int max_size;
	char** names;
	vector<vector<int>> list;
}graph;

struct Subset
{
	int size;
	int max_cut;
	char** names;
	vector<vector<int>> list;
}subset;

vector<int> null_vertex;

//Считывает граф из файла
Graph load_graph(const char* path)
{
	string buf;
	ifstream fin(path);
	getline(fin, buf, '\n');


	graph.size = stoi(buf);
	graph.names = new char* [graph.size];
	for (int i = 0; i < graph.size; i++)
	{
		char buff[60];
		getline(fin, buf, '\n');
		strcpy_s(buff, buf.c_str());
		graph.names[i] = _strdup(buff);
	}

	// Считываем список смежности
	vector<int> buffer;
	int ch = 0;
	int i = 0;
	while ((ch = fin.get()) != EOF)
	{
		if (char(ch) != '\n')
		{
			buffer.push_back(ch - 48);
		}
		else
		{
			graph.list.push_back(buffer);
			buffer.clear();
		}
	}

	return graph;
}


//Ищет вершину по имени
int find_vertex(const char* name)
{
	for (int i = 0; i < graph.size; i++)
	{
		if (!strcmp(graph.names[i], name))
			return i;
	}
	return -1;
}

//Выводит граф в консоль
void print_graph(const Graph graph)
{
	for (int i = 0; i < graph.size; i++)
	{
		cout << graph.names[i];
		cout << " ";

		for (int j = 0; j < graph.list[i].size(); j++)
		{
			cout << graph.list[i][j];
			cout << " ";
		}

		cout << "\n";
	}
}

void print_subset(vector<vector<int>> list)
{
	for (int i = 0; i < graph.size; i++)
	{
		if (list[i] != null_vertex)
		{
			cout << graph.names[i];
			cout << " ";
			for (int j = 0; j < list[i].size(); j++)
			{
				cout << list[i][j];
				cout << " ";
			}
			cout << "\n";
		}
	}
}

//Проверяет, есть ли вершина в подмножестве отсекаемых вершин
bool is_in_subset(int vertex)
{
	if (subset.list[vertex] != null_vertex)
	{
		return true;
	}
	return false;
}

bool is_subset(vector<int> list)
{
	for (int i = 0; i < subset.list.size(); i++)
	{
		if (list == subset.list[i])
		{
			return true;
		}
	}
	return false;
}

//Ищет смежную вершину с наибольшим количеством рёбер
int find_max_edges(vector<int> vertex)
{
	int max_edges = 0;
	int max_vertex = 0;
	for (int i = 0; i < vertex.size(); i++)
	{
		int curr_vertex = 0;
		int edges = 0;
		curr_vertex = vertex[i];
		edges = graph.list[curr_vertex - 1].size();
		if (edges > max_edges)
		{
			max_vertex = curr_vertex;
			max_edges = edges;
		}
	}

	return max_vertex - 1;
}

//Возвращает число разрезов
int cut_number(vector<vector<int>> list)
{
	int result = 0;

	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] != null_vertex)
		{
			result += list[i].size();
		}
	}
	return result;
}


bool is_related(int vertex)
{
	for (int i = 0; i < subset.list.size(); i++)
	{
		if (subset.list[i] != null_vertex && (vertex - 1) == i)
		{
			return true;
		}
	}
	return false;
}

void delete_relation()
{
	for (int i = 0; i < subset.list.size(); i++)
	{
		if (subset.list[i] != null_vertex)
		{
			for (int j = 0; j < subset.list[i].size(); j++)
			{

				if (is_related(subset.list[i][j]))
				{
					int buf = subset.list[i][j] - 1;
					subset.list[i].erase(remove(subset.list[i].begin(), subset.list[i].end(), subset.list[i][j]), subset.list[i].end());
					subset.list[buf].erase(remove(subset.list[buf].begin(), subset.list[buf].end(), i + 1), subset.list[buf].end());
					//break;
				}

			}
		}
	}
}

bool is_in_vertex(int vertex, vector<int> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] == vertex + 1)
		{
			return true;
		}
	}
	return false;
}

void restore_relation(int vertex)
{
	for (int i = 0; i < subset.list.size(); i++)
	{
		if (subset.list[i] != null_vertex)
		{
			if (is_in_vertex(vertex, graph.list[i]) && !is_in_vertex(vertex, subset.list[i]))
			{
				subset.list[i].push_back(vertex + 1);
			}
		}
	}
}

//Заносит вершину в подмножесто отсекаемых вершин
void vertex_to_subset(int vertex)
{
	subset.list[vertex] = graph.list[vertex];
	delete_relation();
	subset.max_cut = cut_number(subset.list);
}


bool is_increase(int vertex, vector<vector<int>> subset_list)
{
	vector<int> graph_vertex = graph.list[vertex];

	subset_list[vertex] = graph_vertex;
	for (int i = 0; i < subset_list.size(); i++)
	{
		if (subset_list[i] != null_vertex)
		{
			for (int j = 0; j < subset_list[i].size(); j++)
			{

				if (is_related(subset_list[i][j]))
				{
					int buf = subset_list[i][j] - 1;
					subset_list[i].erase(remove(subset_list[i].begin(), subset_list[i].end(), subset_list[i][j]), subset_list[i].end());
					subset_list[buf].erase(remove(subset_list[buf].begin(), subset_list[buf].end(), i + 1), subset_list[buf].end());
					//break;
				}

			}
		}
	}
	int increased_cuts = cut_number(subset_list);

	if (increased_cuts > subset.max_cut)
	{
		//subset.max_cut = increased_cuts;
		return true;
	}
	return false;
}

bool delete_vertex(int vertex)
{
	vector<vector<int>> subset_list = subset.list;

	if (subset_list[vertex] != null_vertex)
	{
		subset_list[vertex] = null_vertex;
		if (is_increase(vertex, subset_list))
		{
			return true;
		}
	}

	return false;
}

//Ищет максимальный разрез графа
vector<vector<int>> find_max_cut()
{
	vector<vector<int>> best_result;
	best_result.resize(graph.list.size());
	for (int i = 0; i < graph.list.size(); i++)
	{
		best_result[i] = null_vertex;
	}
	for (int beg_vertex = 0; beg_vertex < graph.size; beg_vertex++)
	{
		subset.list.resize(graph.list.size());

		if (graph.max_size == 0)
		{
			int max_size = 0;
			for (int i = 0; i < graph.list.size(); i++)
			{
				if (graph.list[i].size() > max_size)
				{
					max_size = graph.list[i].size();
				}
			}
			null_vertex.resize(max_size);
			graph.max_size = max_size;
		}

		for (int i = 0; i < graph.list.size(); i++)
		{
			subset.list[i] = null_vertex;
			//subset.list[i].resize(max_size);
		}
		subset.max_cut = 0;

		//Ищем начальную вершину по имени
		//int beg_vertex = find_vertex(beg_name);

		//Смежная вершина с наибольшим количеством рёбер
		int max_edges_vertex = 0;

		//Текущая вершина
		int vertex = beg_vertex;

		//Список со связями текуещй вершины
		vector<int> buf = graph.list[vertex];
		int cut = 0;
		bool end = false;
		//Заносим начальную вершину в подмножество отсекаемых вершин
		vertex_to_subset(beg_vertex);

		//cout << "Start vertex: " << graph.names[vertex] << endl;
		//cout << "------------------------------\n";
		//cout << "Current vertex: " << graph.names[vertex] << " " << cut_number(subset.list) << " cuts\n";
		do
		{
			if (subset.max_cut < cut_number(subset.list))
			{
				subset.max_cut = cut_number(subset.list);
			}
			max_edges_vertex = find_max_edges(buf);
			if (!is_in_subset(max_edges_vertex))
			{
				if (is_increase(max_edges_vertex, subset.list))
				{
					vertex_to_subset(max_edges_vertex);
					vertex = max_edges_vertex;
					buf = graph.list[vertex];
					//cout << "Current vertex: " << graph.names[vertex] << " " << cut_number(subset.list) << " cuts\n";
				}
				else
				{
					buf.erase(remove(buf.begin(), buf.end(), max_edges_vertex + 1), buf.end());
				}
			}
			else
			{
				buf.erase(remove(buf.begin(), buf.end(), max_edges_vertex + 1), buf.end());
			}
			if (buf.empty())
			{
				for (int i = 0; i < graph.list.size(); i++)
				{
					if (!is_in_subset(i))
					{
						if (is_increase(i, subset.list))
						{
							vertex_to_subset(i);
							//cout << "Current vertex: " << graph.names[i] << " " << cut_number(subset.list) << " cuts\n";
						}
					}
				}
				for (int i = 0; i < graph.list.size(); i++)
				{
					if (delete_vertex(i))
					{
						restore_relation(i);
						subset.list[i] = null_vertex;
					}
				}
				end = true;
				//cout << "Max cut: " << cut_number(subset.list) << "\n------------------------------\n";
			}
		} while (!end);

		if (cut_number(subset.list) > cut_number(best_result))
		{
			best_result = subset.list;
		}
	}
	cout << "\nBest subset is:\n";
	print_subset(best_result);
	return best_result;
}


void run_testcase()
{
	graph = load_graph("graph3.txt");
	print_graph(graph);
	cout << "Max cut is: " << cut_number(find_max_cut()) << endl;
}


int main()
{
	run_testcase();
	return 0;
}