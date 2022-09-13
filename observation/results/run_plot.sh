dataset=("email-Enron-full" "email-Eu-full" "contact-high" "NDC-classes-full" "tags-ubuntu" "tags-math" "threads-ubuntu")
for data in ${dataset[@]}
do
  python plot_hyperlap.py -source $data
  python plot_improve.py -source $data
done