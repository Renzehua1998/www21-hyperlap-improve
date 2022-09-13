#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include "read_data.cpp"
#include "tools.cpp"
#define P 20 // hyperedge flow rate 更新分辨率p=0.05，迭代20次

using namespace std;

int main(int argc, char *argv[])
{
	clock_t start, runtime; // 计时变量，！多了一个runtime

	// INPUT: original hypergraph
	// OUTPUT: generated hypergraph
	string dataname = argv[1]; // 数据集名称
	// ！此处没有层数L因为是自适应的

	string graphFile = "../dataset/" + dataname + "_graph.txt";
	string genFile = "../dataset/" + dataname + "_gen.txt";

	// Read data 读取数据
	start = clock();
	vector< vector<int> > node2hyperedge; // 每个节点上都有哪些超边，i为节点编号，里面的vector<int>内存超边序号
	vector< vector<int> > hyperedge2node; // 每条超边上有哪些节点，i为超边序号，里面的vector<int>内存节点编号
	vector< unordered_set<int> > hyperedge2node_set; // 超边的节点集合，与上一个向量中的数据相同
	read_data(graphFile, node2hyperedge, hyperedge2node, hyperedge2node_set); // 读取
	
	int V = (int)node2hyperedge.size(); // 节点数
	int E = (int)hyperedge2node.size(); // 超边数
	cout << "# of nodes: " << V << endl;
	cout << "# of hyperedges: " << E << endl;
	cout << "Reading data done: "  // 读数据时间
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "---------------------------------------------------------------------" << endl << endl;

	// Preprocessing 统计预处理
	start = clock();

	/* node degree & hyperedge size */
	vector<int> Vdeg; // 每个节点的度（超边个数）
	vector<int> Esize; // 每个超边的节点数

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
	cout << "---------------------------------------------------------------------" << endl << endl;

	runtime = clock(); // ！不同点，有个运行时间统计
	// Hypergraph generation (0): Preprocessing 超边生成预处理（候选节点分组）
	start = clock();
	int max_num_level = (int)log2(V); // 最大层数，使用log2(V)求得
	vector<int> level2Ecnt; // 第i个元素为第i层被选中的次数
	vector< vector<int> > level2group2V; // (i,j)的值为i层j组的节点数
	vector< vector< vector<int> > > level2group2E; // (i,j)号vector<int>为所有i层j组被选用为节点集合的超边的序号
	vector< vector< vector<int> > > level2group2Vlist; // (i,j)号vector<int>长度为i层j组节点乘度数的和，值为节点标号
	vector< vector< vector<bool> > > level2group2Evisit; // (i,j)号vector<int>长度为i层j组被选用为节点集合个数，值全为false
	vector<int> best_level2Ecnt; // 最佳-第i个元素为第i层被选中的次数
	vector< vector< vector<int> > > best_level2group2E; // 最佳-(i,j)号vector<int>为所有i层j组被选用为节点集合的超边的序号
	vector< vector< vector<bool> > > best_level2group2Evisit; // 最佳-(i,j)号vector<int>长度为i层j组被选用为节点集合个数，值全为false

	level2Ecnt.resize(max_num_level, 0);
	level2group2V.resize(max_num_level);
	level2group2E.resize(max_num_level);
	level2group2Vlist.resize(max_num_level);
	level2group2Evisit.resize(max_num_level);
	best_level2Ecnt.resize(max_num_level, 0);
	best_level2group2E.resize(max_num_level);
	best_level2group2Evisit.resize(max_num_level);

	for (int level = 0; level < max_num_level; level++){ // 遍历所有层
		int num_group = (int)pow(2, level); // 2^level,转换成整型-分组数

		level2group2V[level].resize(num_group, 0);
		level2group2E[level].resize(num_group);
		level2group2Vlist[level].resize(num_group);
		level2group2Evisit[level].resize(num_group);
		best_level2group2E[level].resize(num_group);
		best_level2group2Evisit[level].resize(num_group);

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
	cout << "---------------------------------------------------------------------" << endl << endl;

	// Hypergraph generation (1): Initialization ！初始化
	start = clock();
	double cur_dist = 1e9;
	unordered_set<int> N; // 每次插入的节点标号集合
	vector<double> homogeneity; // 同质度
	vector< vector<int> > V2E; // 生成超图的节点集
	vector< vector<int> > E2V; // 生成的超边集
	vector< pair<int, int> > E2levelgroup; // ！作用暂时未知，存放层数，组数对

	V2E.resize(V); 
	E2V.resize(E);

	/* generate hyperedges 初始化超边生成 */
	int level = 0, group = 0;

	for (int e = 0; e < E; e++){ // 遍历超边个数生成超边
		int e_size = Esize[e]; // 本超边应该有几个节点

		N.clear(); // 清除N
		while (N.size() < e_size){ // level，group都是0，从全体节点中选取-HyperCL
			int idx = rand() % level2group2Vlist[level][group].size();
			int v = level2group2Vlist[level][group][idx];
			N.insert(v); // 节点下标插入N
		}

		for (const int &v: N){ // 遍历插入节点标号集N
			V2E[v].push_back(e); // 生成超图的点集和边集插入元素
			E2V[e].push_back(v);
		}

		level2group2E[level][group].push_back(e);
		level2group2Evisit[level][group].push_back(false);
		level2Ecnt[level]++;
		E2levelgroup.push_back({level, group});
	}
	
	/* compute hyperedge homogeneity 计算超边同质度 */
	get_homogeneity(homogeneity, V2E, E2V);
	homogeneity_max = *max_element(homogeneity.begin(), homogeneity.end());
	homogeneity_dist = get_D_double(E_homogeneity, homogeneity);
	cur_dist = homogeneity_dist;

	cout << "HyperCL\t"
	     << "Homogeneity Max:\t" << homogeneity_max << "\tHomogeneity D-stat:\t" << homogeneity_dist << endl;

	cout << "Hypergraph generation (1): Initialization done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "---------------------------------------------------------------------" << endl << endl;

	// Hypergraph generation (2): Multilevel ！！！迭代求最优
	start = clock();
	int MAX_TRIAL = E * 5;
	vector< vector<int> > pre_V2E, best_V2E; // 上层和最佳点、边关系向量
	vector< vector<int> > pre_E2V, best_E2V; // 上层和最佳边、点关系向量
	vector<int> updE;

	pre_V2E.resize(V); best_V2E.resize(V);
	pre_E2V.resize(E); best_E2V.resize(E);

	best_V2E = V2E; // 初始定义为上步HyperCL产生的初始内容
	best_E2V = E2V;
	best_level2Ecnt = level2Ecnt; // 初始化为上步内容
	best_level2group2E = level2group2E;
	best_level2group2Evisit = level2group2Evisit;

	for (int level = 1; level < max_num_level; level++){ // 迭代每一层，从第一层开始，收敛退出
		int pre_level = level - 1; // 上一层层数
		int num_group = (int)pow(2, level); // 组数
		bool updated = false; // 是否更新，用于判断迭代是否收敛（无更优时值为false）
		int upd_p = -1; // 当前最佳p值序号

		V2E = best_V2E; // 此步初始点边集，各种统计量为上一步最优
		E2V = best_E2V;
		level2Ecnt = best_level2Ecnt;
		level2group2E = best_level2group2E;
		level2group2Evisit = best_level2group2Evisit;

		for (int p = 0; p < P; p++){ // 根据设定的迭代精度迭代P次，找到最佳
			pair<int, int> _p = get_index(level2Ecnt[pre_level], P, p); // 上层被选中的次数，分为P组，取第p组的标号范围
			int upd_Ecnt = _p.second - _p.first; // 这轮新增为本层的超边数
			int trial = 0;
			updE.clear(); // 清除updE

			pre_V2E = V2E; // 上一层节点集和边集
			pre_E2V = E2V;
			
			/* update hyperedges (level l -> level l+1) 把对应l层的超边更新为l+1层 */
			while (level2Ecnt[level] < upd_Ecnt * (p + 1) and trial++ < MAX_TRIAL){
				int pre_group = rand() % (int)pow(2, pre_level);
				if (level2group2E[pre_level][pre_group].size() == 0) continue;

				int idx = rand() % level2group2E[pre_level][pre_group].size();
				if (level2group2Evisit[pre_level][pre_group][idx]) continue;

				int e = level2group2E[pre_level][pre_group][idx];
				int e_size = Esize[e];

				int group = rand() % (int)pow(2, level);
				if (e_size > level2group2V[level][group]) continue;

				N.clear();
				while (N.size() < e_size){
					int _idx = rand() % level2group2Vlist[level][group].size();
					int v = level2group2Vlist[level][group][_idx];
					N.insert(v);
				}

				updE.push_back(e);
				E2V[e].clear();
				for (const int &v: N) E2V[e].push_back(v);
				E2levelgroup[e] = {level, group};

				level2group2E[level][group].push_back(e);
				level2group2Evisit[level][group].push_back(false);
				level2group2Evisit[pre_level][pre_group][idx] = true;
				level2Ecnt[level]++;
			}

			/* update V2E and E2V 更新V2E和E2V */
			for (const int &e: updE){
				for (const int &v: pre_E2V[e])
					V2E[v].erase(remove(V2E[v].begin(), V2E[v].end(), e), V2E[v].end());
				for (const int &v: E2V[e])
					V2E[v].push_back(e);
			}
	
			/* compute hyperedge homogeneity 计算超边同质度 */
			homogeneity.clear();
			get_homogeneity(homogeneity, V2E, E2V);

			homogeneity_max = *max_element(homogeneity.begin(), homogeneity.end()); // 当前最大同质度
			homogeneity_dist = get_D_double(E_homogeneity, homogeneity); // 当前D值
	
			if (homogeneity_dist < cur_dist){ // 若当前D值小于当前最优，更新最优
				updated = true; upd_p = p; // 有更新，下次继续迭代
				cur_dist = homogeneity_dist;
				best_V2E = V2E;
				best_E2V = E2V;
				best_level2Ecnt = level2Ecnt;
				best_level2group2E = level2group2E;
				best_level2group2Evisit = level2group2Evisit;
			}

			cout << "Level:\t" << level << "\t(p = " << p << ")\t"
			     << "Homogeneity Max:\t" << homogeneity_max << "\tHomogeneity D-stat:\t" << homogeneity_dist << endl;
		}
		cout << "Level:\t" << level << "\toptimal p:\t" << upd_p << endl;
		if (!updated) break; // 收敛则退出循环
	}

	cout << "Final hyperedge homogeneity:\t" << cur_dist << endl;
	cout << endl;
	vector<int> EperLevel(max_num_level, 0);
	for (int i = 0; i < E; i++) EperLevel[E2levelgroup[i].first]++;
	for (int i = 0; i < max_num_level; i++) cout << "Level " << i << ":\t" << EperLevel[i] << endl;

	cout << "Hypergraph generation (2): Multilevel done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "---------------------------------------------------------------------" << endl << endl;

	cout << "FINAL RUNTIME: " << (double)(clock() - runtime) / CLOCKS_PER_SEC << " sec" << endl;

	// Node reindexing
	start = clock();
	int Vindex = 0;
	unordered_map<int, int> node2index;
	unordered_set<int> node_visit;

	for (int i = 0; i < E; i++){
		if (best_E2V[i].size() > 0){
			for (const int &v: best_E2V[i]){
				bool v_in = (node_visit.find(v) != node_visit.end());
				if (!v_in) node2index[v] = Vindex++;
				node_visit.insert(v);
			}
		}
	}

	cout << "Node reindexing done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "---------------------------------------------------------------------" << endl << endl;

	// File output
	start = clock();
	ofstream outFile(genFile.c_str());

	for (int i = 0; i < E; i++){
		if (best_E2V[i].size() > 0){
			string line = "";
			for (const int &v: best_E2V[i]) line += to_string(node2index[v]) + ",";
			line = line.substr(0, line.size()-1);
			outFile << line << endl;
		}
	}

	outFile.close();

	cout << "File output done: " 
	     << (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "---------------------------------------------------------------------" << endl << endl;


	return 0;
}
