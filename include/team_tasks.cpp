#include <algorithm>
#include <map>
#include <string>
#include <tuple>
//#include "task_status.cpp"

class TeamTasks {
public:
  // Получить статистику по статусам задач конкретного разработчика
  const TasksInfo& GetPersonTasksInfo(const std::string& person) const {
    return tasks_of_persons.at(person);
  }

  // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
  void AddNewTask(const std::string& person) {
    tasks_of_persons[person][TaskStatus::NEW]++;
  }

  // Обновить статусы по данному количеству задач конкретного разработчика,
  // подробности см. ниже
  std::tuple<TasksInfo, TasksInfo>
  PerformPersonTasks(const std::string& person, int task_count) {
    if (tasks_of_persons.count(person) == 0)
      return {{}, {}};
    TasksInfo& tasks_info = tasks_of_persons[person];

    TasksInfo add, remove;
    for (const auto& [task_status, count] : tasks_info) {
      if (task_status == TaskStatus::DONE || task_count == 0)
        break;
      const int changing_tasks_count = std::min(task_count, count);
      remove[task_status] = changing_tasks_count;
      add[static_cast<TaskStatus>(static_cast<int>(task_status) + 1)] =
          changing_tasks_count;
      task_count -= changing_tasks_count;
    }

    for (const auto& [task_status, count] : remove) {
      int& current_count = tasks_info[task_status];
      current_count -= count;
      if (current_count == 0)
        tasks_info.erase(task_status);
    }

    TasksInfo old_tasks = tasks_info;
    old_tasks.erase(TaskStatus::DONE);

    for (const auto& [task_status, count] : add)
      tasks_info[task_status] += count;

    return {add, old_tasks};
  }
private:
  std::map<std::string, TasksInfo> tasks_of_persons;
};
