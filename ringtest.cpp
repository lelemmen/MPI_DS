/*
 *  MPI Doctoral School
 */ 

#include <mpi.h>
#include <iostream>
#include <cstdlib>


/* send message in a ring here */
void sendRing(char *buffer, int msgLen)
{
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	int tag = 148;

	if (rank == 0) {
		MPI_Send(buffer, msgLen, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
		MPI_Recv(buffer, msgLen, MPI_CHAR, size-1, tag, MPI_COMM_WORLD, &status);
	} else if ((1 <= rank) && (rank < size - 1)) {
		MPI_Recv(buffer, msgLen, MPI_CHAR, rank-1, tag, MPI_COMM_WORLD, &status);
		MPI_Send(buffer, msgLen, MPI_CHAR, rank+1, tag, MPI_COMM_WORLD);
	} else {  // rank == size - 1
		MPI_Recv(buffer, msgLen, MPI_CHAR, rank-1, tag, MPI_COMM_WORLD, &status);
		MPI_Send(buffer, msgLen, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
	}
}

using namespace std;
int main(int argc, char* argv[])
{
        int rank, size;
        MPI_Init( &argc, &argv );
        MPI_Comm_rank( MPI_COMM_WORLD, &rank );
        MPI_Comm_size( MPI_COMM_WORLD, &size );

        char *buffer = new char[1048576];

        int msgLen = 8;
        for (int i = 0; i < 18; i++) {
                double startTime = MPI_Wtime();
                sendRing(buffer, msgLen);
                double elapsed = MPI_Wtime() - startTime;

                if (rank == 0) {
		    double bandwidth = msgLen * size / elapsed;
                    std::cout << "Bandwidth for size " << msgLen << ": " << bandwidth <<  " bytes per second." << endl;
		}
                msgLen *= 2;
        }

        delete [] buffer;

        MPI_Finalize();
        return EXIT_SUCCESS;
}

