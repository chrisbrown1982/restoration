/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
  Mehdi Goli: m.goli@rgu.ac.uk*/

#include<OpenCL/opencl.h> 
#include <iostream>

class Ocl_Utilities{
    //protected:
    //    int threshold;
public:
    //int  getThreshold(){
    //   return threshold;
    // }
    //int threshold;
    Ocl_Utilities(){}//threshold=1;}
    virtual bool  device_rules(cl_device_id id)=0;
    //void  setThreshold(int t){
    //   threshold=t;
    // }
    // if (threshold!=1){
    //      #define THRESHOLD threshold
    //}
    void printStatus(std::string label, cl_int status){
  
      std::cout << label << " status: ";
      printErrorString(status);
  
      if (status != CL_SUCCESS) exit(1);
    }

    void printErrorString(cl_int error){
      
      switch (error){
	case CL_SUCCESS: 
	  std::cout << "CL_SUCCESS" << std::endl;
	  break;
	case CL_DEVICE_NOT_FOUND: 
	  std::cout << "CL_DEVICE_NOT_FOUND" << std::endl;
	  break;
	case CL_DEVICE_NOT_AVAILABLE: 
	  std::cout << "CL_DEVICE_NOT_AVAILABLE" << std::endl;
	  break;
	case CL_COMPILER_NOT_AVAILABLE: 
	  std::cout << "CL_COMPILER_NOT_AVAILABLE" << std::endl;
	  break;
	case CL_MEM_OBJECT_ALLOCATION_FAILURE: 
	  std::cout << "CL_MEM_OBJECT_ALLOCATION_FAILURE" << std::endl;
	  break;
	case CL_OUT_OF_RESOURCES: 
	  std::cout << "CL_OUT_OF_RESOURCES" << std::endl;
	  break;
	case CL_OUT_OF_HOST_MEMORY: 
	  std::cout << "CL_OUT_OF_HOST_MEMORY" << std::endl;
	  break;
	case CL_PROFILING_INFO_NOT_AVAILABLE: 
	  std::cout << "CL_PROFILING_INFO_NOT_AVAILABLE" << std::endl;
	  break;
	case CL_MEM_COPY_OVERLAP: 
	  std::cout << "CL_MEM_COPY_OVERLAP" << std::endl;
	  break;
	case CL_IMAGE_FORMAT_MISMATCH: 
	  std::cout << "CL_IMAGE_FORMAT_MISMATCH" << std::endl;
	  break;
	case CL_IMAGE_FORMAT_NOT_SUPPORTED: 
	  std::cout << "CL_IMAGE_FORMAT_NOT_SUPPORTED" << std::endl;
	  break;
	case CL_BUILD_PROGRAM_FAILURE: 
	  std::cout << "CL_BUILD_PROGRAM_FAILURE" << std::endl;
	  break;
	case CL_MAP_FAILURE: 
	  std::cout << "CL_MAP_FAILURE" << std::endl;
	  break;
	case CL_MISALIGNED_SUB_BUFFER_OFFSET: 
	  std::cout << "CL_MISALIGNED_SUB_BUFFER_OFFSET" << std::endl;
	  break;
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: 
	  std::cout << "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST" << std::endl;
	  break;
	case CL_INVALID_VALUE: 
	  std::cout << "CL_INVALID_VALUE" << std::endl;
	  break;
	case CL_INVALID_DEVICE_TYPE: 
	  std::cout << "CL_INVALID_DEVICE_TYPE" << std::endl;
	  break;
	case CL_INVALID_PLATFORM: 
	  std::cout << "CL_INVALID_PLATFORM" << std::endl;
	  break;
	case CL_INVALID_DEVICE: 
	  std::cout << "CL_INVALID_DEVICE" << std::endl;
	  break;
	case CL_INVALID_CONTEXT: 
	  std::cout << "CL_INVALID_CONTEXT" << std::endl;
	  break;
	case CL_INVALID_QUEUE_PROPERTIES: 
	  std::cout << "CL_INVALID_QUEUE_PROPERTIES" << std::endl;
	  break;
	case CL_INVALID_COMMAND_QUEUE: 
	  std::cout << "CL_INVALID_COMMAND_QUEUE" << std::endl;
	  break;
	case CL_INVALID_HOST_PTR: 
	  std::cout << "CL_INVALID_HOST_PTR" << std::endl;
	  break;
	case CL_INVALID_MEM_OBJECT: 
	  std::cout << "CL_INVALID_MEM_OBJECT" << std::endl;
	  break;
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: 
	  std::cout << "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR" << std::endl;
	  break;
	case CL_INVALID_IMAGE_SIZE: 
	  std::cout << "CL_INVALID_IMAGE_SIZE" << std::endl;
	  break;
	case CL_INVALID_SAMPLER: 
	  std::cout << "CL_INVALID_SAMPLER" << std::endl;
	  break;
	case CL_INVALID_BINARY: 
	  std::cout << "CL_INVALID_BINARY" << std::endl;
	  break;
	case CL_INVALID_BUILD_OPTIONS: 
	  std::cout << "CL_INVALID_BUILD_OPTIONS" << std::endl;
	  break;
	case CL_INVALID_PROGRAM: 
	  std::cout << "CL_INVALID_PROGRAM" << std::endl;
	  break;
	case CL_INVALID_PROGRAM_EXECUTABLE: 
	  std::cout << "CL_INVALID_PROGRAM_EXECUTABLE" << std::endl;
	  break;
	case CL_INVALID_KERNEL_NAME: 
	  std::cout << "CL_INVALID_KERNEL_NAME" << std::endl;
	  break;
	case CL_INVALID_KERNEL_DEFINITION: 
	  std::cout << "CL_INVALID_KERNEL_DEFINITION" << std::endl;
	  break;
	case CL_INVALID_KERNEL: 
	  std::cout << "CL_INVALID_KERNEL" << std::endl;
	  break;
	case CL_INVALID_ARG_INDEX: 
	  std::cout << "CL_INVALID_ARG_INDEX" << std::endl;
	  break;
	case CL_INVALID_ARG_VALUE: 
	  std::cout << "CL_INVALID_ARG_VALUE" << std::endl;
	  break;
	case CL_INVALID_ARG_SIZE: 
	  std::cout << "CL_INVALID_ARG_SIZE" << std::endl;
	  break;
	case CL_INVALID_KERNEL_ARGS: 
	  std::cout << "CL_INVALID_KERNEL_ARGS" << std::endl;
	  break;
	case CL_INVALID_WORK_DIMENSION: 
	  std::cout << "CL_INVALID_WORK_DIMENSION" << std::endl;
	  break;
	case CL_INVALID_WORK_GROUP_SIZE: 
	  std::cout << "CL_INVALID_WORK_GROUP_SIZE" << std::endl;
	  break;
	case CL_INVALID_WORK_ITEM_SIZE: 
	  std::cout << "CL_INVALID_WORK_ITEM_SIZE" << std::endl;
	  break;
	case CL_INVALID_GLOBAL_OFFSET: 
	  std::cout << "CL_INVALID_GLOBAL_OFFSET" << std::endl;
	  break;
	case CL_INVALID_EVENT_WAIT_LIST: 
	  std::cout << "CL_INVALID_EVENT_WAIT_LIST" << std::endl;
	  break;
	case CL_INVALID_EVENT: 
	  std::cout << "CL_INVALID_EVENT" << std::endl;
	  break;
	case CL_INVALID_OPERATION: 
	  std::cout << "CL_INVALID_OPERATION" << std::endl;
	  break;
	case CL_INVALID_GL_OBJECT: 
	  std::cout << "CL_INVALID_GL_OBJECT" << std::endl;
	  break;
	case CL_INVALID_BUFFER_SIZE: 
	  std::cout << "CL_INVALID_BUFFER_SIZE" << std::endl;
	  break;
	case CL_INVALID_MIP_LEVEL: 
	  std::cout << "CL_INVALID_MIP_LEVEL" << std::endl;
	  break;
	case CL_INVALID_GLOBAL_WORK_SIZE: 
	  std::cout << "CL_INVALID_GLOBAL_WORK_SIZE" << std::endl;
	  break;
	case CL_INVALID_PROPERTY: 
	  std::cout << "CL_INVALID_PROPERTY" << std::endl;
	  break;
	  
	default:
	  std::cout << "Unknown OpenCL error " << error << std::endl;
	
      }
    }
};
