#ifndef TIMER_TASK_H__
#define TIMER_TASK_H__

#include <functional>
#include <stdint.h>
#include <boost\timer.hpp>
#include <list>


class timer_task
{
public:

  void poll();

	// время в секундах через какое время выполнить задачу.
	// количеств раз. 0 - бесконечно.
  void call(std::function<void()> func, double time = 0, uint32_t counts = 1);

private:
	struct task
	{
		std::function<void()> func;
		double time;
		uint32_t counts;
		boost::timer timer;
	};

	std::list<task> m_tasks;
};

#endif // TIMER_TASK_H__
