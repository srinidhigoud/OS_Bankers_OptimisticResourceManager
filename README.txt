Code is commented properly
	main.cpp
		-Tasks (.h,.cpp)
		-Bank (.h,.cpp)
		-OptimRes (.h,.cpp)
	Sample output for input13 called output13.txt
	.o files are compiled files

To run the code: Change the input.txt file name accordingly

module load gcc-6.2.0
g++ -std=c++11 -c main.cpp Tasks.cpp Bank.cpp OptimRes.cpp
g++ main.o Tasks.o Bank.o OptimRes.o
./a.out input1.txt