#include <gyros/gyros.h>

gyros_task_t task1;
char task1_stack[128];

gyros_task_t task2;
char task2_stack[128];

static void
task1_main(void *arg)
{
    for (;;)
        ;
}

static void
task2_main(void *arg)
{
    for (;;)
        gyros_sleep_ms(1000);
}

int
main()
{
    gyros_task_create(&task1, "task1", task1_main, 0,
                      task1_stack, sizeof(task1_stack), 1);
    gyros_task_create(&task2, "task2", task2_main, 0,
                      task2_stack, sizeof(task2_stack), 2);

    gyros_start();

    return 0;
}
