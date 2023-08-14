#ifndef OPTICK_DUMMY_H
#define OPTICK_DUMMY_H

#define OPTICK_EVENT(...)
#define OPTICK_CATEGORY(NAME, CATEGORY)
#define OPTICK_FRAME(NAME)
#define OPTICK_THREAD(THREAD_NAME)
#define OPTICK_START_THREAD(THREAD_NAME)
#define OPTICK_STOP_THREAD()
#define OPTICK_TAG(NAME, DATA)
#define OPTICK_EVENT_DYNAMIC(NAME)	
#define OPTICK_PUSH_DYNAMIC(NAME)		
#define OPTICK_PUSH(NAME)				
#define OPTICK_POP()		
#define OPTICK_CUSTOM_EVENT(DESCRIPTION)
#define OPTICK_STORAGE_REGISTER(STORAGE_NAME)
#define OPTICK_STORAGE_EVENT(STORAGE, DESCRIPTION, CPU_TIMESTAMP_START, CPU_TIMESTAMP_FINISH)
#define OPTICK_STORAGE_PUSH(STORAGE, DESCRIPTION, CPU_TIMESTAMP_START)
#define OPTICK_STORAGE_POP(STORAGE, CPU_TIMESTAMP_FINISH)				
#define OPTICK_SET_STATE_CHANGED_CALLBACK(CALLBACK)
#define OPTICK_SET_MEMORY_ALLOCATOR(ALLOCATE_FUNCTION, DEALLOCATE_FUNCTION)	
#define OPTICK_SHUTDOWN()
#define OPTICK_GPU_INIT_D3D12(DEVICE, CMD_QUEUES, NUM_CMD_QUEUS)
#define OPTICK_GPU_INIT_VULKAN(DEVICES, PHYSICAL_DEVICES, CMD_QUEUES, CMD_QUEUES_FAMILY, NUM_CMD_QUEUS)
#define OPTICK_GPU_CONTEXT(...)
#define OPTICK_GPU_EVENT(NAME)
#define OPTICK_GPU_FLIP(SWAP_CHAIN)
#define OPTICK_UPDATE()
#define OPTICK_FRAME_FLIP(...)
#define OPTICK_FRAME_EVENT(FRAME_TYPE, ...)
#define OPTICK_START_CAPTURE(...)
#define OPTICK_STOP_CAPTURE()
#define OPTICK_SAVE_CAPTURE(...)
#define OPTICK_APP(NAME)

#endif