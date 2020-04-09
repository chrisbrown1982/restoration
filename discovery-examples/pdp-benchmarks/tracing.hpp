#ifndef TRACING_HPP
#define TRACING_HPP

#include <sanitizer/dfsan_interface.h>

void __begin_tracing();
void __begin_loop(int loop_id);
void __begin_iteration(int loop_id);
void __end_iteration(int loop_id);
void __end_loop(int loop_id);
void __begin_region(int loop_id, int region_id);
void __end_region(int loop_id);

#endif /* TRACING_HPP */
