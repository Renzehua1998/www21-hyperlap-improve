g++ -g -O3 -std=c++11 egonet_density.cpp -o run_density;
g++ -g -O3 -std=c++11 egonet_overlapness.cpp -o run_overlapness;
g++ -g -O3 -std=c++11 pair_degree.cpp -o run_pair;
g++ -g -O3 -std=c++11 triple_degree.cpp -o run_triple;
g++ -g -O3 -std=c++11 hyperedge_homogeneity.cpp -o run_homogeneity;

dataset=("email-Enron-full" "email-Eu-full" "contact-high" "NDC-classes-full" "tags-ubuntu" "tags-math" "threads-ubuntu")
#dataset=("email-Eu-full")
types=("graph" "uniform" "improve" "gen" "hypercl")
for data in ${dataset[@]}
do
    for type in ${types[@]}
    do
        ./run_density $data --graph_type $type
        ./run_overlapness $data --graph_type $type
        ./run_pair $data --graph_type $type
        ./run_triple $data --graph_type $type
        ./run_homogeneity $data --graph_type $type    
    done
done