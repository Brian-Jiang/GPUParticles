#if 0
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include <spu_timer.h>

#define rf(a,b) (((b-a)*((float)rand()/RAND_MAX))+a)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

typedef struct {
    float x;
    float y;
    float z;
    float dx,dy,dz;
    float mass;
    int index; // Index, padding?
} point_data;

int rank;
int numranks;
uint64_t lastTime;

typedef ppe_funcptr ppe_pointer;

/* Define some shortcuts for buffer alignment. */
#define ALIGNED16  __attribute__((aligned(16)))
#define ALIGNED128 __attribute__((aligned(128)))

ppe_pointer rpcbuffer0 ALIGNED16;
ppe_pointer rpcbuffer1 ALIGNED16;

ppe_funcptr ppe_malloc ALIGNED16;
ppe_funcptr report_result ALIGNED16;

ppe_pointer ppe_buffer;
// i think 16*32 should be good?
#define BUFF_SIZE 16384 
#define PI 3.14159265

int main(int argc, char *argv[]) {
    //printf("Hello\nsizeof: %d\n", sizeof(point_data));
    //exit(0);
    int dmatag = 3;
	int myVar = 55;
    int i=0,j=0;
    uint32_t done=0;
    point_data points[512] ALIGNED16;
    ppe_pointer scratch;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);

    ppe_malloc = cellmsg_accept_rpc();
    report_result = cellmsg_accept_rpc();

    MPI_Barrier(MPI_COMM_WORLD);

    *(uint32_t *)(void *)&rpcbuffer0 = BUFF_SIZE;
    cellmsg_rpc(ppe_malloc, &rpcbuffer0, sizeof(uint32_t), sizeof(uint32_t),
                &rpcbuffer1, sizeof(ppe_pointer), sizeof(ppe_pointer));
    ppe_buffer = rpcbuffer1;
    srand(rank);
    for(i=0;i<512;i++) {
        points[i].index = i+rank*512;
        points[i].x = rf(-100.0, 100.0);
        points[i].y = rf(-100.0, 100.0);
        points[i].z = rf(-100.0, 100.0);
        points[i].mass = rf(5.0, 10.0);
    }
    float dt = 10.0;
    float mag = 0.0;
    float speed = 35.0;
    float force = 0.0;
for(j=0;;j++) {
    //printf("\007Starting iter %d\n",j);
    mfc_write_tag_mask(1<<dmatag);
    for(i=0;i<512;i++) {
        mag = sqrt(pow(points[i].x, 2) + pow(points[i].y, 2));
        points[i].dx = -points[i].y/mag;
        points[i].dy = points[i].x/mag;
        force = speed / (pow(abs(mag-((-points[i].z + 100.0)/2.0)), 1.5) + 1.0);
        force /= (points[i].mass / 10.0);
        points[i].dz = sin(((j+i)%360)*(PI/180.0)) / 100.0;
        points[i].x += points[i].dx * (float)dt * force;
        points[i].y += points[i].dy * (float)dt * force;
        points[i].z += points[i].dz * (float)dt;
    }
    mfc_put(&points, ppe_buffer, BUFF_SIZE, dmatag, 0,0);
    mfc_read_tag_status_all();

    ((uint32_t *)(void *)&rpcbuffer0)[0] = ppe_buffer;
    cellmsg_rpc(report_result,
              &rpcbuffer0, sizeof(uint32_t), sizeof(uint32_t),
              NULL, 0, 1);
    MPI_Barrier(MPI_COMM_WORLD);
}

    printf("Done %d\n", rank);

    MPI_Finalize();

	return 0;
}

#endif
