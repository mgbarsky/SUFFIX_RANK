INPUT_FOLDER=$1
SA_FOLDER=$2
gcc -o test_sa utils.c test_sa.c
cat ${SA_FOLDER}/suffixarray_* > final_sa
./test_sa ${INPUT_FOLDER} final_sa
