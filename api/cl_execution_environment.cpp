#include"cl_execution_environment.h"
#include"execution_environment.h"

namespace NEO {

ClExecutionEnvironment::ClExecutionEnvironment() : ExecutionEnvironment() {
	printf("Hello there!\n");
	//asyncEventsHandler.reset(new AsyncEventsHandler());
}

}
