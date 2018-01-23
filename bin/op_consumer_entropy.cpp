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
#include <fstream>

using namespace std;

bm_type_t bm_type = BM_NONE;
int BM_MPSC_OQ_CAP = -1;

bm_process_op_t bm_process_op_type = BM_PROCESS_DUMMY;
int SPIN_TIME = -1;
int random_accum = 0;


int main (int argc, char** argv)
{
    int size = strtol(argv[1], NULL, 10);
    int count = 0;

    void* context = zmq_ctx_new ();
    void* consumer = zmq_socket (context, ZMQ_SUB);
    zmq_connect (consumer, "tcp://localhost:5555");
    zmq_setsockopt(consumer, ZMQ_SUBSCRIBE, NULL, 0);
    cout << "Connected consumer..." << endl;
    
    
    int rc = initEntropy(200);
    if (rc < 0) return 2;

    ofstream file;
    file.open ("entropy.txt");
    

    while (count < size) {
        count++;
        char buffer[sizeof(bm_op_t)];
        int nbytes = zmq_recv(consumer, buffer, sizeof(bm_op_t), 0);
        if (sizeof(bm_op_t) == nbytes) {
            bm_op_t* op_ptr = (bm_op_t*) buffer;
            op_t* optr = (op_t*)malloc(sizeof(op_t));
            optr->type = (type_t)op_ptr->type;
            optr->key_hv = op_ptr->key_hv;

            processEntropy(*optr);
        }




        file << getEntropyValue() << endl;
        file.close();

    }
    //cout << "Presione cualquier tecla para continuar..." << endl;
    //cin.get();
    
    return 0;
}


