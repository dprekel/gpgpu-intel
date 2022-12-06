#include<stdio.h>
#include"execution_environment.h"
#include"wait_util.h"

namespace NEO {

ExecutionEnvironment::ExecutionEnvironment() {
	WaitUtils::Init();
}

}
