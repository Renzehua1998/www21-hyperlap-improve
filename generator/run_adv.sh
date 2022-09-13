datanameList=("email-Enron-full" "email-Eu-full" "contact-high" "NDC-classes-full" "tags-ubuntu" "tags-math" "threads-ubuntu")
#datanameList="email-Eu-full"

g++ -O3 -std=c++11 main_adv.cpp -o run_adv;
IFS=,
for dataname in ${datanameList[@]};
do
	./run_adv $dataname;
done
