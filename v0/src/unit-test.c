#include "net_utility.h"

int main(int argc, char *argv[]) {
	
#if defined(TEST_1_FORMAT)
	format(33,'-');
	return 0;
#endif

#if defined(TEST_2_USAGE)
	usage(argv);
	return 0;
#endif

#if defined(TEST_3_ERROR_MSG)
	error_msg("Look like this passed");
	return 0;
#endif

#if defined(TEST_4_PASS_MSG)
	pid_t process_id = getpid();
	pass_msg("Created process:",&process_id);
	pass_msg(NULL,NULL);
	return 0;
#endif

#if defined(TEST_5_PREFORM_TASK)
	perform_task("Testing Test 5 perform task...",NULL);
	return 0;
#endif 
}
