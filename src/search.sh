rm -rf build/
mkdir build/
cd build/
cmake ..
make
data_dir="/home/xinyan/programs/data"
data_set="audio"
metric="euclid"
# gdb --args \
./search \
-t ${data_dir}/${data_set}/${data_set}_base.fvecs \
-b ${data_dir}/${data_set}/${data_set}_base.fvecs \
-q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
-g ${data_dir}/${data_set}/${data_set}_${metric}_groundtruth.lshbox
