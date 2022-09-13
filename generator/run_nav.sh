datanameList=("email-Enron-full" "email-Eu-full" "contact-high" "NDC-classes-full" "tags-ubuntu" "tags-math" "threads-ubuntu")
#datanameList="email-Eu-full"
numLevelList="5"

g++ -O3 -std=c++11 main_nav.cpp -o run_nav;
IFS=,
for dataname in ${datanameList[@]};
do
	for numLevel in $numLevelList;
	do
		./run_nav $dataname $numLevel;
	done
done
