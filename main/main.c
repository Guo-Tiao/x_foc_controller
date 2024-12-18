#include <stdio.h>
#include "core/x_foc_core.h"

#define X_RUN_DEMO      1


void app_main(void)
{
#if X_RUN_DEMO
    extern void demo_main();
    demo_main();
    return;
#endif
    //初始化
    x_foc_init();
    //运行
    x_foc_run();
}
