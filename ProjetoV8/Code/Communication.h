#include "Project.h"

int com_poolInit(Pool *pool, int size);
int com_inDetectionRange(Robot *robotA, int *a_center, int *b_center);
int com_computeLinkHush(Robot *robot);

int com_setDetectionRadius(Robot *robot, int detectionRadius);
int com_setIdRobotLink(Robot *robot, int id);

int com_WriteOnPool(Robot *robot, Pool *pool);
int com_ReadPool(Robot *robot, Pool *pool);
int com_poolUpdateCommunication(Robot *robot, Pool *pool);

int com_poolVerifyCommunication(Robot *robot, Pool *pool);
int com_poolNewCommunication(Robot *robot, Pool *pool);
int com_verifyRobotLink(Robot *robot, int id, long int time_stamp);


void init_queue(CellQueue *q, int capacity);
void enqueue_cell(CellQueue *q, Cell cell);
void dequeue_cell(CellQueue *q);
void print_queue(CellQueue *q);