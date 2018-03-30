#!/bin/bash

#SBATCH --job-name=lab3
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem=10GB
#SBATCH --time=00:20:00
#SBATCH --partition=c32_41


#Uncomment to execute C code
module load gcc-6.2.0
g++ -std=c++11 -c main.cpp Tasks.cpp Bank.cpp OptimRes.cpp
g++ main.o Tasks.o Bank.o OptimRes.o
./a.out input.txt