datanameList=("email-Enron-full" "email-Eu-full" "contact-high" "NDC-classes-full" "tags-ubuntu" "tags-math" "threads-ubuntu")
#datanameList="email-Eu-full"

g++ -O3 -std=c++11 main_improve.cpp -o run_improve;
IFS=,
for dataname in ${datanameList[@]};
do
	./run_improve $dataname;
done
