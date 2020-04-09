#include <sanitizer/dfsan_interface.h>
#include "tracing.hpp"

static const char * __PROLOGUE = "prologue";
static const char * __EPILOGUE = "epilogue";

extern const char * __name[];
extern bool __visited[];

void __begin_tracing() {
  dfsan_trace_region(__PROLOGUE);
}

void __begin_loop(int loop_id) {
  if (TRACE_REGION == loop_id && !__visited[loop_id])
    dfsan_trace_instructions();
}

void __begin_iteration(int loop_id) {
  if (TRACE_REGION == loop_id && !__visited[loop_id])
    dfsan_trace_region(__name[loop_id]);
}

void __end_iteration(int loop_id) {
  if (TRACE_REGION == loop_id && !__visited[loop_id])
    dfsan_trace_instructions();
}

void __end_loop(int loop_id) {
  if (TRACE_REGION == loop_id&& !__visited[loop_id] ) {
    dfsan_trace_region(__EPILOGUE);
    __visited[loop_id] = true;
  }
}

void __begin_region(int loop_id, int region_id) {
  if (TRACE_REGION == loop_id && !__visited[loop_id])
    dfsan_trace_region(__name[region_id]);
}

void __end_region(int loop_id) {
  if (TRACE_REGION == loop_id && !__visited[loop_id])
    dfsan_trace_instructions();
}
