#include "timer_task.h"

void timer_task::poll()
{
  for (auto it = m_tasks.begin(); it != m_tasks.end();)
  {
    task &t = *it;
    if (t.timer.elapsed() > t.time)
    {
			t.timer.restart();
			t.func();
			if (t.counts == 0)
			{
        continue;
			}
      --t.counts;
      if (t.counts == 0)
      {
        it = m_tasks.erase(it);
      }
    }
    ++it;
  }
}


void timer_task::call(std::function<void()> func, double time, uint32_t counts)
{
  task t;
	t.func = func;
	t.time = time;
	t.counts = counts;
  m_tasks.push_back(t);
}

