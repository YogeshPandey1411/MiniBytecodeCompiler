#ifndef VM_H
#define VM_H

#include "common.h"

/* Execute the bytecode in `obj`.
   Returns 0 on success, -1 on runtime error. */
int vm_execute(const CodeObject *obj);

#endif
