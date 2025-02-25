#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

const uint8_t MAX_TASKS = 10;

class TaskScheduler
{
public:
    typedef void (*TaskCallback)();

    void executeDelayed(TaskCallback callback, unsigned long delayMs)
    {
        noInterrupts();
        if (taskCount < MAX_TASKS)
        {
            taskQueue[taskCount].runAt = millis() + delayMs;
            taskQueue[taskCount].callback = callback;
            taskCount++;
        }
        interrupts();
    }

    void process()
    {
        noInterrupts();
        uint8_t i = 0;
        while (i < taskCount)
        {
            if (millis() >= taskQueue[i].runAt)
            {
                TaskCallback cb = taskQueue[i].callback;
                // Remove this task from the queue by shifting remaining tasks
                for (uint8_t j = i; j < taskCount - 1; j++)
                {
                    taskQueue[j] = taskQueue[j + 1];
                }
                taskCount--;
                interrupts();
                cb(); // Execute the task
                noInterrupts();
            }
            else
            {
                i++;
            }
        }
        interrupts();
    }

private:
    struct ScheduledTask
    {
        unsigned long runAt;
        TaskCallback callback;
    };

    ScheduledTask taskQueue[MAX_TASKS];
    uint8_t taskCount = 0;
};

#endif