import matplotlib
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression
import argparse

plt.rcParams["font.family"] = 'sans-serif'
matplotlib.rcParams.update({'font.size': 15})
color_list = ['#56A768', '#C34F52', '#8272B1', '#4A71AF', '#CCBB79', '#74BDD2']

def plot_degree(data_name, fname, save_name, x_name, y_name):
    
    plt.figure(figsize=(6,4), dpi=120)
    
    dir_path = './' + data_name + '/'
    
    print(data_name)
    datapath = dir_path + fname + '_graph.txt'
    data = pd.read_csv(datapath)
    data = data[data[y_name]!=0]
    datax = data[x_name]
    datay = data[y_name]
    plt.scatter(datax, datay, s=40, alpha=0.8, label='data', c=color_list[0])
    
    gen_path = dir_path + fname + '_gen.txt'
    gen_data = pd.read_csv(gen_path)
    gen_data = gen_data[gen_data[y_name]!=0]
    genx = gen_data[x_name]
    geny = gen_data[y_name]
    plt.scatter(genx, geny, s=40, alpha=0.8, label='hyperlap+', c=color_list[1])

    hypercl_path = dir_path + fname + '_improve.txt'
    hypercl_data = pd.read_csv(hypercl_path)
    hypercl_data = hypercl_data[hypercl_data[y_name] != 0]
    hyperclx = hypercl_data[x_name]
    hypercly = hypercl_data[y_name]
    plt.scatter(hyperclx, hypercly, s=40, alpha=0.8, label='improve', c=color_list[3])
    
    max_xlim = max(max(datax), max(genx), max(hyperclx))
    
    plt.xlabel(x_name)
    plt.ylabel(y_name)
    
    plt.xscale('log')
    plt.yscale('log')
    plt.xlim([0.8, 10*max_xlim])

    plt.legend(bbox_to_anchor=(0., 1.02, 1., .2), loc=3,
               ncol=3, mode="expand", borderaxespad=0.)  # 图例，位于图片上方

    ax = plt.gca()
    ax.tick_params(labelcolor='#4B4B4B', labelsize=11)
    postfix = '.jpg'

    plt.tight_layout()
    plt.savefig(dir_path + 'new_' + save_name + '_' + data_name + '.jpg', bbox_inches='tight')
    # plt.show()  
    
def plot_density(data_name, fname, save_name):
    dir_path = './' + data_name + '/'
    
    # gen
    gen_path = dir_path + fname + '_gen.txt'
    gen_data = pd.read_csv(gen_path)
      
    # Data
    path = dir_path + fname + '_graph.txt'
    data = pd.read_csv(path)

    # hypercl
    hypercl_path = dir_path + fname + '_improve.txt'
    hypercl_data = pd.read_csv(hypercl_path)
    
    print(data_name)    
    plt.figure(figsize=(6,4), dpi=120)
     
    data_number = data.groupby(['num_hes']).mean()
    data_x = np.array(data_number['num_nodes'])
    data_y = np.array(data_number.index)
    
    gen_number = gen_data.groupby(['num_hes']).mean()
    gen_x = np.array(gen_number['num_nodes'])
    gen_y = np.array(gen_number.index)

    hypercl_number = hypercl_data.groupby(['num_hes']).mean()
    hypercl_x = np.array(hypercl_number['num_nodes'])
    hypercl_y = np.array(hypercl_number.index)

    min_x = min(min(data_x), min(gen_x), min(hypercl_x))
    max_x = max(max(data_x), max(gen_x), min(hypercl_y))
    
    # linear regression : slope ~ average density
    reg_data = LinearRegression().fit(data_x.reshape(-1, 1), data_y.reshape(-1, 1))
    reg_datax = np.linspace(min_x, max_x)
    reg_datay = reg_datax * reg_data.coef_[0] + reg_data.intercept_
    plt.plot(reg_datax, reg_datay,c=color_list[0], alpha=0.55, linestyle='--')
    
    reg_gen = LinearRegression().fit(gen_x.reshape(-1, 1),gen_y.reshape(-1, 1))
    reg_genx = np.linspace(min_x, max_x)
    reg_geny = reg_genx * reg_gen.coef_[0] + reg_gen.intercept_
    plt.plot(reg_genx, reg_geny, c=color_list[1], alpha=0.55, linestyle='--')

    reg_hypercl = LinearRegression().fit(hypercl_x.reshape(-1, 1), hypercl_y.reshape(-1, 1))
    reg_hyperclx = np.linspace(min_x, max_x)
    reg_hypercly = reg_hyperclx * reg_hypercl.coef_[0] + reg_hypercl.intercept_
    plt.plot(reg_hyperclx, reg_hypercly, c=color_list[3], alpha=0.55, linestyle='--')
    
    # scatter plot
    plt.scatter(data_x, data_y, label='data', c=color_list[0], alpha=0.8, s=40)
    plt.scatter(gen_x, gen_y,label='hyperlap+', c=color_list[1], alpha=0.8, s=40)
    plt.scatter(hypercl_x, hypercl_y, label='improve', c=color_list[3], alpha=0.8, s=40)
    
    plt.ylabel('# hyperedges')
    plt.xlabel('avg. # nodes')

    plt.legend(bbox_to_anchor=(0., 1.02, 1., .2), loc=3,
               ncol=3, mode="expand", borderaxespad=0.)  # 图例，位于图片上方

    ax = plt.gca()
    ax.tick_params(labelcolor='#4B4B4B', labelsize=10.5)
    
    plt.tight_layout()
    plt.savefig(dir_path + 'new_' + save_name + '_' + data_name + '.jpg', bbox_inches='tight')
    # plt.show()

def plot_overlapness(data_name, fname, save_name):
    dir_path = './' + data_name + '/'
    
    # gen
    gen_path = dir_path + fname + '_gen.txt'
    gen_data = pd.read_csv(gen_path)
      
    # Data
    path = dir_path + fname + '_graph.txt'
    data = pd.read_csv(path)

    # hypercl
    hypercl_path = dir_path + fname + '_improve.txt'
    hypercl_data = pd.read_csv(hypercl_path)
    
    print(data_name)

    plt.figure(figsize=(6,4), dpi=120)

    data_number = data.groupby(['sum_hyperedge_size']).mean()
    data_x = np.array(data_number['num_nodes'])
    data_y = np.array(data_number.index)
    
    gen_number = gen_data.groupby(['sum_hyperedge_size']).mean()
    gen_x = np.array(gen_number['num_nodes'])
    gen_y = np.array(gen_number.index)

    hypercl_number = hypercl_data.groupby(['sum_hyperedge_size']).mean()
    hypercl_x = np.array(hypercl_number['num_nodes'])
    hypercl_y = np.array(hypercl_number.index)
    
    min_x = min(min(data_x), min(gen_x), min(hypercl_x))
    max_x = max(max(data_x), max(gen_x), min(hypercl_y))
    
    # linear regression : slope ~ average density
    reg_data = LinearRegression().fit(data_x.reshape(-1, 1), data_y.reshape(-1, 1))
    reg_datax = np.linspace(min_x, max_x)
    reg_datay = reg_datax * reg_data.coef_[0] + reg_data.intercept_
    plt.plot(reg_datax, reg_datay,c=color_list[0], alpha=0.55, linestyle='--')
    
    reg_gen = LinearRegression().fit(gen_x.reshape(-1, 1),gen_y.reshape(-1, 1))
    reg_genx = np.linspace(min_x, max_x)
    reg_geny = reg_genx * reg_gen.coef_[0] + reg_gen.intercept_
    plt.plot(reg_genx, reg_geny, c=color_list[1], alpha=0.55, linestyle='--')

    reg_hypercl = LinearRegression().fit(hypercl_x.reshape(-1, 1), hypercl_y.reshape(-1, 1))
    reg_hyperclx = np.linspace(min_x, max_x)
    reg_hypercly = reg_hyperclx * reg_hypercl.coef_[0] + reg_hypercl.intercept_
    plt.plot(reg_hyperclx, reg_hypercly, c=color_list[3], alpha=0.55, linestyle='--')
    
    # scatter plot
    plt.scatter(data_x, data_y, label='data', c=color_list[0], alpha=0.8, s=40)
    plt.scatter(gen_x, gen_y,label='hyperlap+', c=color_list[1], alpha=0.8, s=40)
    plt.scatter(hypercl_x, hypercl_y, label='improve', c=color_list[3], alpha=0.8, s=40)
    
    plt.xlabel('avg. # nodes')
    plt.ylabel('$\sum$ hyperedge size')

    plt.legend(bbox_to_anchor=(0., 1.02, 1., .2), loc=3,
               ncol=3, mode="expand", borderaxespad=0.)  # 图例，位于图片上方

    ax = plt.gca()
    ax.tick_params(labelcolor='#4B4B4B', labelsize=10.5)
            
    plt.tight_layout()
    plt.savefig(dir_path + 'new_' + save_name + '_' + data_name + '.jpg', bbox_inches='tight')
    # plt.show()


def plot_homogeneity(data_name, fname, x_name, y_name, save_name):
    round_num = 0

    dir_path = './' + data_name + '/'
    plt.figure(figsize=(6, 4), dpi=120)
    # plt.figure()

    # data
    data = pd.read_csv(dir_path + fname + '_graph.txt')
    data = data[data[x_name] != 0.0]
    data[x_name] = data[x_name].round(round_num)
    count_x = data[x_name].value_counts().sort_index()

    data_x = count_x.index
    data_y = count_x.values

    plt.scatter(data_x, data_y, s=40, alpha=0.8, c=color_list[0], label='data')

    # gen
    gendata = pd.read_csv(dir_path + fname + '_gen.txt')
    gendata = gendata[gendata[x_name] != 0.0]
    gendata[x_name] = gendata[x_name].round(round_num)
    gen_count_x = gendata[x_name].value_counts().sort_index()

    gen_x = gen_count_x.index
    gen_y = gen_count_x.values

    plt.scatter(gen_x, gen_y, s=40, alpha=0.8, c=color_list[1], label='hyperlap+')

    # hypercl
    hypercldata = pd.read_csv(dir_path + fname + '_improve.txt')
    hypercldata = hypercldata[hypercldata[x_name] != 0.0]
    hypercldata[x_name] = hypercldata[x_name].round(round_num)
    hypercl_count_x = hypercldata[x_name].value_counts().sort_index()

    hypercl_x = hypercl_count_x.index
    hypercl_y = hypercl_count_x.values

    plt.scatter(hypercl_x, hypercl_y, s=40, alpha=0.8, c=color_list[3], label='improve')

    plt.xlabel(x_name)
    plt.ylabel(y_name)
    plt.xscale('log')
    plt.yscale('log')

    plt.legend(bbox_to_anchor=(0., 1.02, 1., .2), loc=3,
               ncol=3, mode="expand", borderaxespad=0.) # 图例，位于图片上方

    ax = plt.gca()
    ax.tick_params(labelcolor='#4B4B4B', labelsize=11)

    plt.tight_layout()
    plt.savefig(dir_path + 'new_' + save_name + '_' + data_name + '.jpg', bbox_inches='tight')
    # plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-source', '--dataset', type=str, required=True, help='Select the dataset')
    args = parser.parse_args()
    plot_degree(data_name=args.dataset, fname='pair_degree', save_name='pair_deg', x_name='# overlapping hyperedges',
                y_name='# node pairs')
    plot_degree(data_name=args.dataset, fname='triple_degree', save_name='triple_deg',
                x_name='# overlapping hyperedges', y_name='# node triples')
    plot_density(data_name=args.dataset, fname='egonet_density', save_name='egonet_density')
    plot_overlapness(data_name=args.dataset, fname='egonet_overlapness', save_name='egonet_overlapness')
    plot_homogeneity(data_name=args.dataset, fname='hyperedge_homogeneity', x_name='homogeneity', y_name='# hyperedges',
                     save_name='hyperedge_homogeneity')