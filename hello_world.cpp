#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char** argv) {
	
	int rank;
	int size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	std::string greeting = "Hello world from process: " + std::to_string(rank) + " of " + std::to_string(size) + "\n"; 
	std::cout << greeting;


	MPI_Finalize();
	return EXIT_SUCCESS;
}
