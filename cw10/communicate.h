#ifndef COMMUNICATION_H 
#define COMMUNICATION_H

typedef enum op_t { ADD = 1, SUB = 2, MUL = 3, DIV = 4 };
typedef struct task_t {
   op_t op; 
   double arg1;
   double arg2;
} task_t;

#endif
