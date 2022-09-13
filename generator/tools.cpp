#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

pair<int, int> get_index(int V, int g, int i)
{
	int s = V / g;
	int r = V % g;
	if (i < r) return {(s+1)*i, (s+1)*(i+1)};
	return {(s+1)*r + s*(i-r), (s+1)*r + s*(i+1-r)};
}

void get_degDist(vector<int>& degDist,
		 vector< vector<int> >& node2hyperedge,
		 vector< vector<int> >& hyperedge2node)
{
	int V = (int)node2hyperedge.size();
	int E = (int)hyperedge2node.size();

	for (int i = 0; i < V; i++) degDist.push_back((int)node2hyperedge[i].size());
}

void get_overlapness(vector<double>& overlapness,
		     vector< vector<int> >& node2hyperedge,
		     vector< vector<int> >& hyperedge2node)
{
	int V = (int)node2hyperedge.size();
	int E = (int)hyperedge2node.size();

	unordered_set<int> m;

	for (int v = 0; v < V; v++){
		m.clear();
		double size_sum = 0;
		for (const int &e: node2hyperedge[v]){
			size_sum += (double)hyperedge2node[e].size();
			for (const int &v: hyperedge2node[e]){
				m.insert(v);
			}
		}
		double neigh = (double)m.size();
		overlapness.push_back(size_sum / neigh);
	}
}

void get_homogeneity(vector<double>& locality,
		  vector< vector<int> >& node2hyperedge,
		  vector< vector<int> >& hyperedge2node)
{
	int V = (int)node2hyperedge.size();
	int E = (int)hyperedge2node.size();

	vector< unordered_map<int, int> > V_adj;
	V_adj.resize(V);

	for (int i = 0; i < V; i++){
		for (const int &e: node2hyperedge[i]){
			for (const int &v: hyperedge2node[e]){
				if (v == i) continue;
				V_adj[i][v]++;
			}
		}
	}

	for (int e = 0; e < E; e++){
		double l = 0;
		int e_size = (int)hyperedge2node[e].size();
		for (int i = 0; i < e_size; i++){
			int v_i = hyperedge2node[e][i];
			for (int j = i + 1; j < e_size; j++){
				int v_j = hyperedge2node[e][j];
				l += V_adj[v_i][v_j];
			}
		}
		if (e_size > 1) l /= (e_size * (e_size - 1) / 2);
		locality.push_back(l);
	}
}

// ---------------------------------------------------------
void project(vector< vector<int> > &hyperedge2node,
            vector< vector<int> > &node2hyperedge,
            vector< vector< pair<int, int> > > &node_adj, 
            vector< unordered_map<int, int> > &node_inter){
	// input node2hyperedge, hyperedge2 node
	// output clique expansion result into the unordered map and adjacency list

	int V = (int)node2hyperedge.size();
    node_adj.resize(V);
    node_inter.resize(V);
	vector<long long> upd_time(V, -1LL);
	
	for (int node_a = 0; node_a < V; node_a++){
		for (const int &hyperedge: node2hyperedge[node_a]){
			for (const int &node_b: hyperedge2node[hyperedge]){
				if (node_a == node_b) continue;
				if ((upd_time[node_b] >> 31) ^ node_a){
					upd_time[node_b] = ((long long)node_a << 31)
							      + (long long)node_adj[node_a].size();
					node_adj[node_a].push_back({node_b, 0});
				}
				node_adj[node_a][(int)(upd_time[node_b] & 0x7FFFFFFFLL)].second++;
			}
		}
	}
	for (int node_a = 0; node_a < V; node_a++){
        node_inter[node_a].rehash((int)node_adj[node_a].size());
        for (int i = 0; i < node_adj[node_a].size(); i++){
            int node_b = node_adj[node_a][i].first;
            int C_ab = node_adj[node_a][i].second;
            node_inter[node_a].insert({node_b, C_ab});
        }
    }
}

void get_triple(vector<int>& bin0,
		  vector< vector<int> >& node2hyperedge,
		  vector< vector<int> >& hyperedge2node)
{
	int V = (int)node2hyperedge.size();
	int E = (int)hyperedge2node.size();
	int bin_size = E+1;
	vector< int > bin(bin_size, 0);
	vector< vector< pair<int, int> > > node_adj; // store < neighbor node, how many hyperedges share each other >
    vector< unordered_map<int, int> > node_inter;
    project(hyperedge2node, node2hyperedge, node_adj, node_inter);
	for(int va=0 ; va < V ; va++){
        int deg_a = (int)node_adj[va].size();

        for(int i=0 ; i < deg_a ; i++){
            int vb = node_adj[va][i].first;
            int deg_b = (int)node_adj[vb].size();

            vector<int> intersection;
            for(auto &h : node2hyperedge[va]){
                if(find(node2hyperedge[vb].begin(), node2hyperedge[vb].end(), h) != node2hyperedge[vb].end()){
                    intersection.push_back(h);
                }
            }

            for (int j = i+1; j < deg_a; j++){
                int vc = node_adj[va][j].first;
                    
                int inter_abc = 0;
                if(node_inter[vb][vc] !=0 && va < min(vb,vc)){ // all connected 3 nodes
                    for(auto &h: node2hyperedge[vc]){
                        if(find(intersection.begin(), intersection.end(), h) != intersection.end()){
                            inter_abc++;
                        }
                    }
                }
                else continue; // exclude zero intersection_abc
                int bin_idx = min( bin_size-1, inter_abc);
                bin[bin_idx]++;
            }
        }
    }
	bin0 = bin;
}

double get_D_triple(vector<int>& a, vector<int>& b)
{
	int len_a = (int)a.size(), len_b = (int)b.size(), i = 1, j = 1;
	double cum_a = 0, cum_b = 0;
	double max_D = -1;
	
	vector< pair<int, int> > a_dic, b_dic;
	while (i < len_a){
		a_dic.push_back({i, a[i]});
		i++;
	}
	while (j < len_b){
		b_dic.push_back({j, b[j]});
		j++;
	}

	int sum_a = (int)a_dic.size(), sum_b = (int)b_dic.size();
	i = 0; j = 0;
	while (i < sum_a || j < sum_b){
		if (j == sum_b){
			cum_a += (double)a_dic[i++].second;
		} else if (i == sum_a){
			cum_b += (double)b_dic[j++].second;
		} else if (a_dic[i].first < b_dic[j].first){
			cum_a += (double)a_dic[i++].second;
		} else if (a_dic[i].first > b_dic[j].first){
			cum_b += (double)b_dic[j++].second;
		} else{
			cum_a += (double)a_dic[i++].second;
			cum_b += (double)b_dic[j++].second;
		}
		double D = abs((cum_a/len_a) - (cum_b/len_b));
		max_D = max(max_D, D);
	}

	return max_D;
}
// --------------------------------------------------------------

double get_bhat(vector<double>& a, vector<double>& b)
{
	int len = (int)a.size(), _max = -1;
	double bc = 0.0;

	unordered_map<int, int> d_a, d_b;
	for (int i = 0; i < len; i++){
		d_a[(int)a[i]]++;
		d_b[(int)b[i]]++;
		_max = max(_max, max((int)a[i], (int)b[i]));
	}

	for (int i = 0; i <= _max; i++){
		bc += pow(((double)d_a[i] / len) * ((double)d_b[i] / len), 0.5);
	}

	return -log(bc);
}

double get_D_int(vector<int>& a, vector<int>& b, bool sorted=false)
{
	int len_a = (int)a.size(), len_b = (int)b.size(), i = 0, j = 0;
	double cum_a = 0, cum_b = 0;
	double max_D = -1;

	if (!sorted){
		sort(a.begin(), a.end());
		sort(b.begin(), b.end());
	}
	
	vector< pair<int, int> > a_dic, b_dic;
	while (i < len_a){
		a_dic.push_back({a[i], 1});
		while (i < len_a-1 && a[i] == a[i+1]){
			a_dic[a_dic.size()-1].second++;
			i++;
		}
		i++;
	}
	while (j < len_b){
		b_dic.push_back({b[j], 1});
		while (j < len_b-1 && b[j] == b[j+1]){
			b_dic[b_dic.size()-1].second++;
			j++;
		}
		j++;
	}

	int sum_a = (int)a_dic.size(), sum_b = (int)b_dic.size();
	i = 0; j = 0;
	while (i < sum_a || j < sum_b){
		if (j == sum_b){
			cum_a += (double)a_dic[i++].second;
		} else if (i == sum_a){
			cum_b += (double)b_dic[j++].second;
		} else if (a_dic[i].first < b_dic[j].first){
			cum_a += (double)a_dic[i++].second;
		} else if (a_dic[i].first > b_dic[j].first){
			cum_b += (double)b_dic[j++].second;
		} else{
			cum_a += (double)a_dic[i++].second;
			cum_b += (double)b_dic[j++].second;
		}
		double D = abs((cum_a/len_a) - (cum_b/len_b));
		max_D = max(max_D, D);
	}

	return max_D;
}

double get_D_double(vector<double>& a, vector<double>& b, bool sorted=false)
{
	int len_a = (int)a.size(), len_b = (int)b.size(), i = 0, j = 0;
	double cum_a = 0, cum_b = 0;
	double max_D = -1;

	if (!sorted){
		sort(a.begin(), a.end());
		sort(b.begin(), b.end());
	}

	vector< pair<double, int> > a_dic, b_dic;
	while (i < len_a){
		a_dic.push_back({a[i], 1});
		while (i < len_a-1 && a[i] == a[i+1]){
			a_dic[a_dic.size()-1].second++;
			i++;
		}
		i++;
	}
	while (j < len_b){
		b_dic.push_back({b[j], 1});
		while (j < len_b-1 && b[j] == b[j+1]){
			b_dic[b_dic.size()-1].second++;
			j++;
		}
		j++;
	}

	int sum_a = (int)a_dic.size(), sum_b = (int)b_dic.size();
	i = 0; j = 0;
	while (i < sum_a || j < sum_b){
		if (j == sum_b){
			cum_a += (double)a_dic[i++].second;
		} else if (i == sum_a){
			cum_b += (double)b_dic[j++].second;
		} else if (a_dic[i].first < b_dic[j].first){
			cum_a += (double)a_dic[i++].second;
		} else if (a_dic[i].first > b_dic[j].first){
			cum_b += (double)b_dic[j++].second;
		} else{
			cum_a += (double)a_dic[i++].second;
			cum_b += (double)b_dic[j++].second;
		}
		double D = abs((cum_a/len_a) - (cum_b/len_b));
		max_D = max(max_D, D);
	}
	
	return max_D;
}

void get_Llist(vector<int>& Llist, vector<double>& homogeneity, int L, bool sorted=false) {
	if (!sorted){
		sort(homogeneity.begin(), homogeneity.end());
	}
	int length = homogeneity.size(); 
	double max = *max_element(homogeneity.begin(), homogeneity.end()); // 计算最大同质度
	double basic = log2(max)/L;
	for(int i=0; i < length; i++){
		double num = log2(homogeneity[i]);
		Llist.push_back(int(num/basic));
		// cout << int((num/basic)) << endl;
	}
}