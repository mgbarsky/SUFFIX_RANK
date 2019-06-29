$INPUT_FOLDER=$1
$SA_FOLDER=$2
gcc -o test_sa utils.c test_sa.c
cat output/suffixarray_* > final_sa
./test_sa input final_sa
