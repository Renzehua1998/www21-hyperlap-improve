'''
多层随机超图生成算法HYPERLAP的python实现
'''

import time
import random
import math
import argparse

def randomChoice(items, prop):  # 按分布概率正比从列表随机选择一个元素
    sum = 0
    prob = []
    for i in range(len(prop)):
        sum += prop[i]
        prob.append(sum)
    p = random.uniform(0, sum)
    min = 0
    max = len(items) - 1
    while max - min > 1:
        middle = int((min + max) / 2)
        if prob[middle] < p:
            min = middle
        else:
            max = middle
    return items[max]

def HyperLAP(dataname):
    time0 = time.time()
    print('loading data......\n')
    graph_file = '../dataset/' + dataname + '_graph.txt'
    gen_file = '../dataset/' + dataname + '_hyperlap.txt'
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
    nodes = list(nodes_num)
    time2 = time.time()
    print('count dataset done, ', len(nodes_num), 'nodes, ', len(data), 'edges, cost ', time2-time1, 'sec\n')
    print('grouping nodes......\n')
    L = int(math.log2(len(nodes_num)))  # 最大分组阶数
    rem = len(nodes_num) - 2**L  # 余数
    nodes_group = []
    nodes_prob = []
    random.shuffle(nodes)
    for i in range(2**L):
        if i < rem:
            group = [nodes[i], nodes[2**L+i]]
        else:
            group = [nodes[i]]
        nodes_group.append(group)
        prob = []
        for node in group:
            prob.append(nodes_num[str(node)])
        nodes_prob.append(prob)
    time3 = time.time()
    print('group nodes done, cost ', time3 - time2, 'sec\n')
    print('generating network......\n')
    # gen_data = []  # 生成数据
    with open(gen_file, 'w') as f:
        for i in range(len(data)):
            gen_edge = []
            for j in range(len(data[i])):
                l = random.randint(1, L)
                index = random.randint(0, 2**l-1)
                gen_nodes_list = []  # 当前分组节点列表
                gen_nodes_prob = []  # 当前分组节点概率
                for k in range(2**(L-l)):
                    gen_nodes_list += nodes_group[index+k]
                    gen_nodes_prob += nodes_prob[index+k]
                gen_node = randomChoice(gen_nodes_list, gen_nodes_prob)
                gen_edge.append(gen_node)
            # gen_data.append(gen_edge)
            f.writelines(','.join(gen_edge) + '\n')
    time4 = time.time()
    print('generate network done, cost ', time4-time3, 'sec\n')
    print('save network: ', dataname, '_hyperlap.txt done\n')
    print('Done ', dataname, ' cost ', time4-time0, 'sec\n')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-source', '--source_data', type=str, required=True, help='Select the dataset')
    args = parser.parse_args()
    HyperLAP(args.source_data)
    # HyperLAP('email-Enron-full')