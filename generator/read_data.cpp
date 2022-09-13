#include <iostream>  // std::cout std::cin
#include <fstream>
#include <sstream>  // 字符串流 sstream（常用于格式转换）std::stringstream
#include <algorithm>
#include <unordered_set>

using namespace std;

vector<string> split(string str, char delimiter){  // 用于字符串分割的函数
	vector<string> internal;
	stringstream ss(str);  // 将str复制到ss-创建存储str的副本的 stringstream 对象
	string temp;
	while (getline(ss, temp, delimiter)){  // is 进行读入操作的输入流 str 用来存储读入的内容 delim 终结符，遇到该字符停止读取操作，不写的话默认为回车
		internal.push_back(temp);
	}
	return internal;
}

void read_data(string path, // 读取数据
		vector< vector<int> >& node2hyperedge, // 存储有vector<int>的vector容器
		vector< vector<int> >& hyperedge2node,
		vector< unordered_set<int> >& hyperedge2node_set){
	
	ifstream graphFile(path.c_str()); // 默认以输入方式打开文件，存储在graphFile中，括号中获取到path字符串的指针const char*
	string line; // 读取文件内容的一行
	int num_hyperedge = 0; // 超边数
	while (getline(graphFile, line)){ // 每次读文件的一行
		vector<string> nodes = split(line, ','); // 把这行按逗号拆开，获取这条超边的节点
		vector<int> tokens;
		unordered_set<int> tokens_set;
		for (int i = 0; i < nodes.size(); i++){ // 遍历超边的每个节点
			tokens.push_back(stoi(nodes[i])); // 使用stoi将节点从str转换为int，插入tokens
			tokens_set.insert(stoi(nodes[i]));
			while (node2hyperedge.size() <= stoi(nodes[i])) // 扩充node2hyperedge的大小，使其适应节点编号，每次加一填充空值
				node2hyperedge.push_back(vector<int>());
			node2hyperedge[stoi(nodes[i])].push_back(num_hyperedge); // node2hyperedge第节点编号个元素末尾追加超边编号
		}
		hyperedge2node.push_back(tokens); // hyperedge2node末尾追加tokens，即节点向量
		hyperedge2node_set.push_back(tokens_set); // hyperedge2node_set末尾追加tokens_set，即节点集合
		num_hyperedge++; // 循环变量自增
	}
}
