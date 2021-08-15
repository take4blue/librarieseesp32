#include "Task.h"
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

namespace Take4 {
IRAM_ATTR void Task::task(void *p)
{
    Task *task = (Task *)p;
    task->function_(*task, task->callbackData_);
    // タスクが終了したので削除処理を実施する。
    vTaskDelete(nullptr);
    task->taskHandle_ = nullptr;
}

Task::Task()
    : taskHandle_(nullptr)
    , function_(nullptr)
    , callbackData_(nullptr)
{
}
Task::~Task() {}

void Task::startProcess(const char *taskName, uint32_t stackSize, UBaseType_t priority, BaseType_t core, CallbackFunction func, void *data)
{
    if (taskHandle_ == nullptr) {
        function_ = func;
        callbackData_ = data;
        xTaskCreatePinnedToCore(Task::task, taskName,
                                stackSize, (void *)this, priority, &taskHandle_, core);
    }
}

void Task::suspend()
{
    vTaskSuspend(taskHandle_);
}

void Task::resume()
{
    vTaskResume(taskHandle_);
}

eTaskState Task::status() const
{
    return eTaskGetState(taskHandle_);
}

BaseType_t Task::notify(uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue)
{
    return xTaskGenericNotify(taskHandle_, ulValue, eAction, pulPreviousNotificationValue);
}

BaseType_t Task::notifyFromISR(uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken, uint32_t *pulPreviousNotificationValue)
{
    return xTaskGenericNotifyFromISR(taskHandle_, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken);
}

BaseType_t Task::wait(uint32_t &pulNotificationValue, TickType_t xTicksToWait, uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit)
{
    return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit, &pulNotificationValue, xTicksToWait);
}

void Task::suspendAll()
{
    vTaskSuspendAll();
}

void Task::resumeAll()
{
    xTaskResumeAll();
}

void Task::initWatchDogTimer(uint32_t timeout, bool panic)
{
    ESP_ERROR_CHECK(esp_task_wdt_init(timeout, panic));
}

void Task::addWatchDog()
{
    ESP_ERROR_CHECK(esp_task_wdt_add(taskHandle_));
}

void Task::deleteWatchDog()
{
    ESP_ERROR_CHECK(esp_task_wdt_delete(taskHandle_));
}

void Task::resetWatchDog()
{
    ESP_ERROR_CHECK(esp_task_wdt_reset());
}
} // namespace Take4
