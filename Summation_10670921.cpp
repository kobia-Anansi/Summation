#include <iostream>
#include <mpi.h>
#include <time.h>
#include <ctime>
#include <chrono>
using namespace std;
using namespace std::chrono;
int main(int argc, char *argv[])
{
	int rank, sum, size, dest = 0, tag = 0, A[1000];
	int elapsed_time[5]; int totaTime = 0;

	for(int i=0;i<1001;i++)
		A[i] = i;
	
	MPI_Status status;
	MPI_Init (&argc, &argv);
	rank=MPI::COMM_WORLD.Get_rank();
	size = MPI::COMM_WORLD.Get_size();

	if(rank != 0)
	{
		auto start = high_resolution_clock::now();
		int interval = 1000/(size),begin,end;
		begin = interval*(rank-1);
		end = interval*(rank)-1;

		sum = 0;
		for(int i=begin;i<=end;i++)
			sum+=i;
		//cout <<sum <<"at beginning of rank " << rank <<endl;

		MPI_Send(&sum, sizeof(sum), MPI_INT, dest, tag, MPI_COMM_WORLD);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		elapsed_time[rank] = duration.count();
		MPI_Send(&elapsed_time[rank], sizeof(elapsed_time[rank]), MPI_INT, dest, 1, MPI_COMM_WORLD);
	}
	else
	{
		auto start = high_resolution_clock::now();
		int interval = 1000/(size),begin,end;
		begin = interval*(size-1);
		end = interval*(size)-1;

		int calc = 0;
		for(int i=begin;i<=end+1;i++)
			calc+=i;
		
		int total = 0; int add = 0;
	    
		for(int source= 1; source< size; source++)
		{
			//cout << source <<endl;
			MPI_Recv(&sum, sizeof(sum), MPI_INT, source, 0, MPI_COMM_WORLD, &status);
			cout << "The sum recieved from the process " << source << " is " << sum <<endl;
			cout <<sum <<endl;
			total += sum;
		}

		for(int source= 1; source< size; source++)
		{
			//cout << source <<endl;
			MPI_Recv(&elapsed_time[source], sizeof(elapsed_time[source]), MPI_INT, source, 1, MPI_COMM_WORLD, &status);
			cout << "Time recieved from the process " << source << " is " << elapsed_time[source] <<endl;
			totaTime += elapsed_time[source];
		}

	 	cout << "the total sum is " << total + calc<<endl;
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		elapsed_time[rank] = duration.count();

		cout << "Time taken by Summation Program for rank: "
        << elapsed_time[rank] + totaTime << " microseconds and "<< "Speedup is " <<size <<" times that of single node execution" << endl;
	}
	MPI_Finalize();
}	