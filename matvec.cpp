#include <mpi.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
        // dimensions of a
        int rows = 100;
        int cols = 100;
        
	double **a;
        double *b, *c;
        
	int master = 0;                 // rank of master
        int myid;                       // rank of this process
        int numprocs;                   // number of processes
        
	// allocate memory for a, b and c
        a = (double**)malloc(rows * sizeof(double*));
        for (int i = 0; i < rows; i++) {
        	a[i] = (double*)malloc(cols * sizeof(double));
		for (int j = 0; j < cols; j++) {
			a[i][j] = 1.0;
		}
        }
        b = (double*)malloc(cols * sizeof(double));
        c = (double*)malloc(rows * sizeof(double));
        
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &myid );
	MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
	MPI_Status status;

	int MPI_END_TAG_CUSTOM = -1;

	// Master-slave code
	if ( myid == master ) {	
		// Fill in values for b and broadcast to every slave
		for (int i = 0; i < cols; i++) {
			b[i] = 1.0;
		}
		MPI_Bcast(b, cols, MPI_DOUBLE, master, MPI_COMM_WORLD);
		std::cout << "Broadcast b from master." << std::endl;	

		// Initially, send one row of A to each slave
		int num_sent = 0;  // number of rows that are sent out to the slaves
		for (int p = 0; p < numprocs-1; p++) {
			int process = p+1;
			int row_number = p;
			MPI_Send(a[row_number], cols, MPI_DOUBLE, process, row_number, MPI_COMM_WORLD);
			num_sent++;
		}

		// Receive 'rows' computed matvec entries
		for (int i = 0; i < rows; i++) {
			double result;
			MPI_Status status;

			MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			c[status.MPI_TAG] = result;
			int process = c[status.MPI_SOURCE];

			if (num_sent < rows) {
				// Send a new row to the process that gave a result
				int row_number = num_sent+1;
				MPI_Send(a[row_number], cols, MPI_DOUBLE, process, row_number, MPI_COMM_WORLD); 
			}

			else {
				MPI_Send(&result, 1, MPI_DOUBLE, process, MPI_END_TAG_CUSTOM, MPI_COMM_WORLD);
			} 
		}

		// Output the result
		std::cout << "Result c: " << std::endl;
		for (int i = 0; i < rows; i++) {
			std::cout << c[i] << std::endl;
		} 
	}

	// Slave
	else {
		double *row;
		row = (double*)malloc(cols * sizeof(double));

		while (true) {
			MPI_Status status;
			MPI_Recv(row, cols, MPI_DOUBLE, master, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			if ( status.MPI_TAG != MPI_END_TAG_CUSTOM ) {
				int row_number = status.MPI_TAG;

				// Calculate the inner product of a row of A with b
				double result = 0.0;
				for (int i = 0; i < cols; i++) {
					result += row[i] * b[i];
				}

				MPI_Send(&result, 1, MPI_DOUBLE, master, row_number, MPI_COMM_WORLD);
			} else {
				break;
			}
		}

		delete row;
	}

	delete a, b, c;
	MPI_Finalize();
	return EXIT_SUCCESS;
}
