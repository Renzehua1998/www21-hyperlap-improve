'''
随机超图生成算法HYPERCL的python实现
'''

import time
import random
import argparse

def HyperCL(dataname):
    time0 = time.time()
    print('loading data......\n')
    graph_file = '../dataset/' + dataname + '_graph.txt'
    gen_file = '../dataset/' + dataname + '_hypercl.txt'
    data = []
    with open(graph_file, 'r') as f:
        for line in f.readlines():
            line = line.strip('\n')
            line = line.split(',')
            data.append(line)
    time1 = time.time()
    print('load data: ', dataname, '_graph.txt done, cost ', time1-time0, 'sec\n')
    print('counting dataset......\n')
    nodes_num = {}  # 节点累积概率字典
    tol_num = 0  # 总个数-用于归一化
    for edge in data:
        for node in edge:
            if node in nodes_num:
                nodes_num[node] += 1
            else:
                nodes_num[node] = 1
            tol_num += 1
    tol_p = 0  # 用于概率加和
    nodes = list(nodes_num)  # 节点列表
    nodes_prob = []  # 节点累计对应概率
    for node in nodes_num:
        tol_p = tol_p + nodes_num[node] / tol_num
        nodes_prob.append(tol_p)
    time2 = time.time()
    print('count dataset done, ', len(nodes_num), 'nodes, ', len(data), 'edges, cost ', time2-time1, 'sec\n')
    print('generating network......\n')
    gen_data = []  # 生成数据
    with open(gen_file, 'w') as f:
        for i in range(len(data)):
            gen_edge = []
            for j in range(len(data[i])):
                p = random.uniform(0,1)
                min = 0
                max = len(nodes) - 1
                while max - min > 1:
                    middle = int((min + max) / 2)
                    if nodes_prob[middle] < p:
                        min = middle
                    else:
                        max = middle
                gen_edge.append(nodes[max])
            gen_data.append(gen_edge)
            f.writelines(','.join(gen_edge) + '\n')
    time3 = time.time()
    print('generate network done, cost ', time3-time2, 'sec\n')
    print('save network: ', dataname, '_hypercl.txt done\n')
    print('Done ', dataname, ' cost ', time3-time0, 'sec\n')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-source', '--source_data', type=str, required=True, help='Select the dataset')
    args = parser.parse_args()
    HyperCL(args.source_data)
