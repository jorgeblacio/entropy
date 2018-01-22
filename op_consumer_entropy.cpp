#include "k_v_benchmark.h"
#include "entropy.h"
#include <zmq.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <assert.h>
#include <inttypes.h>
#include <iostream>
#include <cstdlib>

using namespace std;

bm_type_t bm_type = BM_NONE;
int BM_MPSC_OQ_CAP = -1;

bm_process_op_t bm_process_op_type = BM_PROCESS_DUMMY;
int SPIN_TIME = -1;
int random_accum = 0;

int get_and_set_config_from_file() {
    static const char* filename = "./bm_config.txt";
    FILE* bm_config_fptr = fopen(filename, "r");
    if (bm_config_fptr == NULL) {
        cout << filename << " does NOT exist or it is Wrong." << endl;
        return -1;
    }

    char line[50];
    fgets(line, 50, bm_config_fptr);
    bm_type = (bm_type_t)atoi(line);
    fgets(line, 50, bm_config_fptr);
    BM_MPSC_OQ_CAP = atoi(line);
    fgets(line, 50, bm_config_fptr);
    bm_process_op_type = (bm_process_op_t)atoi(line);
    fgets(line, 50, bm_config_fptr);
    SPIN_TIME = atoi(line);
    fclose(bm_config_fptr);
    return 0;
}

void bm_process_op(bm_op_t op) {
    switch(bm_process_op_type) {
        case BM_PROCESS_DUMMY: {
            ;
        } break;
        case BM_PROCESS_ADD: {
            random_accum += rand();
        } break;
        case BM_PROCESS_SPIN: {
            struct timeval t1, t2;
            gettimeofday(&t1, NULL);
            double elapsed = 0;
            do {
                gettimeofday(&t2, NULL);
                elapsed = t2.tv_usec - t1.tv_usec;
            } while(elapsed < SPIN_TIME);
        } break;
        case BM_PROCESS_PRINT: {
            fprintf(stdout, "type: %d, key: %"PRIu64"\n", op.type, op.key_hv);
        } break;
    }
}

int main (void)
{
	bool running = true;
    int rc = get_and_set_config_from_file();
    if (rc < 0) return 1;

    void* context = zmq_ctx_new ();
    void* consumer = zmq_socket (context, ZMQ_SUB);
    zmq_connect (consumer, "tcp://localhost:5555");
    zmq_setsockopt(consumer, ZMQ_SUBSCRIBE, NULL, 0);
    cout << "Connected consumer..." << endl;
    
    
    rc = initEntropy(200);
    if (rc < 0) return 2;

    

    while (1) {
        char buffer[sizeof(bm_op_t)];
        int nbytes = zmq_recv(consumer, buffer, sizeof(bm_op_t), 0);
        if (sizeof(bm_op_t) == nbytes) {
            bm_op_t* op_ptr = (bm_op_t*) buffer;
            op_t* optr;
            optr->type = (type_t)op_ptr->type;
            optr->key_hv = op_ptr->key_hv;

            processEntropy(*optr);
        }
        cout << getEntropyValue() << endl;
    }
    cout << "Presione cualquier tecla para continuar..." << endl;
    cin.get();
    
    return 0;
}


