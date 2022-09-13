dataset=("email-Enron-full" "email-Eu-full" "contact-high" "NDC-classes-full" "tags-ubuntu" "tags-math" "threads-ubuntu")
for data in ${dataset[@]}
do
  python main_hypercl.py -source $data
  python main_hyperlap.py -source $data
done