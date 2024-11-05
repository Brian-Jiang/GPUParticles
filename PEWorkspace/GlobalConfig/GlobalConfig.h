#ifndef PE_GLOBAL_CONFIG_H
#define PE_GLOBAL_CONFIG_H

#define PE_SERVER_PORT 1660
#define PE_SERVER_MAX_CONNECTIONS 32
#define PE_CLIENT_LUA_COMMAND_SERVER_PORT 1417
#define PE_SERVER_LUA_COMMAND_SERVER_PORT 1500


#define PE_CLIENT_TO_SERVER_CONNECT_TIMEOUT 1000

#define PE_SOCKET_SEND_TIMEOUT 0

#define PE_SOCKET_RECEIVE_TIMEOUT 0

#define PE_SOCKET_SEND_STEPSIZE 8192
#define PE_SOCKET_RECEIVE_BUFFER_SIZE 8192



// in general is a good idea. if we have mroe than one method in same event processing queue for a component, it is likely
// that user forgot a parent registered a handler for the event and re-registered a new function in a child
#define PE_DONT_ALLOW_MULTIPLE_METHODS_IN_EVENT_PROCESSING_QUEUE 1

#define CLASS_REGISTRATION_CHECKS 1

#define DEFAULT_SKIN_WEIGHTS_PER_VERTEX 8

#define PE_MAX_INSTANCE_COUNT (1280)
#define PE_MAX_INSTANCE_COUNT_IN_DRAW_CALL (1280)
#define PE_MAX_SKINED_INSTANCE_COUNT_IN_DRAW_CALL (1280) // should be <= PE_MAX_INSTANCE_COUNT_IN_DRAW_CALL
#define PE_MAX_SKINED_INSTANCE_COUNT_IN_COMPUTE_CALL (1280) // should be <= PE_MAX_INSTANCE_COUNT_IN_DRAW_CALL

#if PE_API_IS_D3D11
#define PE_MAX_BONE_COUNT_IN_DRAW_CALL 128
#else
#define PE_MAX_BONE_COUNT_IN_DRAW_CALL 16
#endif
#define PE_MAX_FRAMES_IN_ANIMATION (30)
#define PE_MAX_ANIMATIONS_IN_BUFFER (16)


#define PE_MAX_NUM_OF_BUFFER_STEPS (64)

//code genration control

#define PE_PERFORM_REDUNDANCY_MEMORY_CHECKS 0

//animation compute 
#define PE_USE_COMPUTE_FOR_INSTANCED_ANIMS 1 // do we use compute at all to help with anims?

#if PE_USE_COMPUTE_FOR_INSTANCED_ANIMS
	#define PE_USE_COMPUTE_ANIMATION_REDUCE 1 // 0 or 1: 1 = read results from compute output, not cpu fed buffer
	#if PE_USE_COMPUTE_ANIMATION_REDUCE
		#define PE_STORE_CS_MAP_RESULT_AS_MATRIX 1
	#endif
#endif

#ifndef PE_STORE_CS_MAP_RESULT_AS_MATRIX
#define PE_STORE_CS_MAP_RESULT_AS_MATRIX 0
#endif

#ifndef PE_USE_COMPUTE_ANIMATION_REDUCE
#define PE_USE_COMPUTE_ANIMATION_REDUCE 0
#endif


#endif
