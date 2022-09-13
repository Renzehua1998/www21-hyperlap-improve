#include <iostream>  // 输入输出
#include <fstream>  // 读写磁盘文件
#include <ctime>  // 计时
#include <vector>  // 向量（Vector）是一个封装了动态大小数组的顺序容器
#include <algorithm>  // STL库中最重要的头文件之一，提供了大量基于迭代器的非成员模版函数。
#include <cstdlib>  // 标准库函数-类型转换、动态分配内存、随机数生成
#include <random>  // C++11新库，用作随机数生成-随机数引擎和随机数分布引擎
#include <unordered_map> // 内部实现了一个哈希表（也叫散列表，通过把关键码值映射到Hash表中一个位置来访问记录，
// 查找的时间复杂度可达到O(1)，其在海量数据处理中有着广泛应用）--和map相比，查找更快
#include <unordered_set>  // hash实现的集合容器，只能前向迭代
#include "read_data.cpp"
#include "tools.cpp"


using namespace std;

int main(int argc, char *argv[])
{
	clock_t start; // 计时变量

	// INPUT: (1) original hypergraph (2) number of Lvels
	// execute run_nav.sh
	//
	// OUTPUT: generated hypergraph

	string dataname = argv[1]; // 数据集名称
	int L = stoi(argv[2]); // 层数L

	string graphFile = "../dataset/" + dataname + "_graph.txt"; // 输入文件
	string genFile = "../dataset/" + dataname + "_uniform.txt"; // 输出文件

	// Read data 读取数据
	start = clock(); // 开始计时
	vector< vector<int> > node2hyperedge; // 每个节点上都有哪些超边，i为节点编号，里面的vector<int>内存超边序号
	vector< vector<int> > hyperedge2node; // 每条超边上有哪些节点，i为超边序号，里面的vector<int>内存节点编号
	vector< unordered_set<int> > hyperedge2node_set; // 超边的节点集合，与上一个向量中的数据相同
	read_data(graphFile, node2hyperedge, hyperedge2node, hyperedge2node_set); // 读取

	int V = (int)node2hyperedge.size(); // 节点数
	int E = (int)hyperedge2node.size(); // 超边数
	cout << "# of nodes: " << V << endl;
	cout << "# of hyperedges: " << E << endl;
	cout << "Reading data done: "  // 读数据时间
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl; // CLOCKS_PER_SEC一秒钟内CPU运行的时钟周期数
	cout << "--------------------------------------------------------------------" << endl << endl;

	// Preprocessing 统计预处理
	start = clock();

	/* node degree & hyperedge size */
	vector<int> Vdeg; // 每个节点的度（超边个数）
	vector<int> Esize;  // 每个超边的节点数

	for (int i = 0; i < V; i++) Vdeg.push_back((int)node2hyperedge[i].size());
	for (int i = 0; i < E; i++) Esize.push_back((int)hyperedge2node[i].size());

	random_shuffle(Vdeg.begin(), Vdeg.end()); // 随机重新排列范围内的元素，打乱[first, last)之间元素的顺序
	random_shuffle(Esize.begin(), Esize.end());

	int max_Esize = *max_element(Esize.begin(), Esize.end()); // 最大超边点数值
	cout << "Max hyperedge size: " << max_Esize << endl;

	/* hyperedge homogeneity 超边同质性分析，计算最大同质度 */
	double homogeneity_max, homogeneity_dist;
	vector<double> E_homogeneity;
	get_homogeneity(E_homogeneity, node2hyperedge, hyperedge2node);

	homogeneity_max = *max_element(E_homogeneity.begin(), E_homogeneity.end()); // 计算最大同质度
	cout << "Max hyperedge homogeneity: " << homogeneity_max << endl;

	cout << "Preprocessing done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------------------------" << endl << endl;

	// Hypergraph generation (0): Preprocessing 超边生成预处理（候选节点分组）
	start = clock();
	vector<int> level2Ecnt; // 第i个元素为第i层被选中的次数
	vector< vector<int> > level2group2V; // (i,j)的值为i层j组的节点数
	vector< vector< vector<int> > > level2group2E; // (i,j)号vector<int>为所有i层j组被选用为节点集合的超边的序号
	vector< vector< vector<int> > > level2group2Vlist; // (i,j)号vector<int>长度为i层j组节点乘度数的和，值为节点标号
	vector< vector< vector<int> > > level2group2Evisit; // (i,j)号vector<int>长度为i层j组被选用为节点集合个数，值全为false

	level2Ecnt.resize(L, 0);
	level2group2V.resize(L);
	level2group2E.resize(L);
	level2group2Vlist.resize(L);
	level2group2Evisit.resize(L);

	for (int level = 0; level < L; level++){ // 遍历每一层
		int num_group = (int)pow(2, level); // 2^level,转换成整型-分组数

		level2group2V[level].resize(num_group, 0);
		level2group2E[level].resize(num_group);
		level2group2Vlist[level].resize(num_group);
		level2group2Evisit[level].resize(num_group);

		for (int group = 0; group < num_group; group++){ // 遍历每一组
			pair<int, int> idx = get_index(V, num_group, group); // 获取V个节点，分成num组，第group组的节点下标
			int s_idx = idx.first;
			int e_idx = idx.second;
			for (int i = s_idx; i < e_idx; i++){
				level2group2V[level][group]++; // level层第group组的总节点数
				for (int j = 0; j < Vdeg[i]; j++)
					level2group2Vlist[level][group].push_back(i);
			}
		}
	}

	cout << "Hypergraph generation (0): Preprocessing done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------------------------" << endl << endl;

	// Hypergraph generation (1): Hyperedge generation 超边生成
	start = clock();
	unordered_set<int> N; // 每次插入的节点标号集合
	vector< vector<int> > V2E; // 生成超图的节点集
	vector< vector<int> > E2V; // 生成的超边集

	V2E.resize(V);
	E2V.resize(E);

	/* generate hyperedges */
	int level = 0, group = 0;

	for (int e = 0; e < E; e++){ // 遍历超边个数生成超边
		int e_size = Esize[e]; // 本超边应该有几个节点
		
		level = rand() % L; // 随机生成层数，组数
		group = rand() % (int)pow(2, level);

		while (level2group2V[level][group] < e_size){ // 循环直到随机挑选的层内某组节点数大于需要的节点数
			level = rand() % L;
			group = rand() % (int)pow(2, level);
		}

		N.clear(); // 清除N
		while (N.size() < e_size){ // 循环填充N直到其节点数达到要求
			int idx = rand() % level2group2Vlist[level][group].size(); // 随机挑选该组中的一个节点，获取其在组中下标
			int v = level2group2Vlist[level][group][idx]; // 利用组中下标idx找到在原始节点中的下标
			N.insert(v); // 节点下标插入N
		}

		for (const int &v: N){ // 遍历插入节点标号集N
			V2E[v].push_back(e); // 生成超图的点集和边集插入元素
			E2V[e].push_back(v);
		}

		level2group2E[level][group].push_back(e);
		level2group2Evisit[level][group].push_back(false);
		level2Ecnt[level]++;
	}

	/* compute hyperedge homogeneity 计算超边同质度 */
	vector<double> homogeneity; // 超边同质度
	get_homogeneity(homogeneity, V2E, E2V);
	homogeneity_max = *max_element(homogeneity.begin(), homogeneity.end()); // 计算最大同质度
	homogeneity_dist = get_D_double(E_homogeneity, homogeneity); // 求得D-statistic值

	cout << "UNIFORM\t"
	     << "Homogeneity Max:\t" << homogeneity_max << "\tHomogeneity D-stat:\t" << homogeneity_dist << endl;

	cout << "Hypergraph generation (1: Hyperedge generation done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------------------------" << endl << endl;

	// Node reindexing 节点重新索引
	start = clock();
	int Vindex = 0;
	unordered_map<int, int> node2index; // 生成超图中节点编号和序号（出现次序）的对应关系
	unordered_set<int> node_visit; // 访问过的节点集合

	for (int i = 0; i < E; i++){ // 遍历生成超边
		if (E2V[i].size() > 0){
			for (const int &v: E2V[i]){ // 遍历生成超边的每个点
				bool v_in = (node_visit.find(v) != node_visit.end()); // 节点集合中包含该节点
				if (!v_in) node2index[v] = Vindex++; // 若不包含，v号节点的值为Vindex放入node2index
				node_visit.insert(v); // 将节点插入节点集合
			}
		}	
	}

	cout << "Node reindexing done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------------------------" << endl << endl;

	// File output 文件输出
	start = clock();
	ofstream outFile(genFile.c_str()); // 存储文件名为genFile

	for (int i = 0; i < E; i++){ // 遍历生成超边
		if (E2V[i].size() > 0){
			string line = "";
			for (const int &v: E2V[i]) line += to_string(node2index[v]) + ","; // 生成一行
			line = line.substr(0, line.size() - 1); // 取有效位
			outFile << line << endl; // 输出
		}
	}

	outFile.close(); // 关闭文件

	cout << "File output done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "--------------------------------------------------------------------" << endl << endl;



	return 0;
}
