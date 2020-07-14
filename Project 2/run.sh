make
./main-serial 1000
./main-serial 10000
./main-serial 100000
./main-serial 1000000
mpiexec -np 4 ./main-parallel 1000
mpiexec -np 4 ./main-parallel 10000
mpiexec -np 4 ./main-parallel 100000
mpiexec -np 4 ./main-parallel 1000000
make clean
