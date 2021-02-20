#if not defined _CTL_SDL2_FPS_
#define _CTL_SDL2_FPS_

#include <chrono>
#include "../Timer.h"

namespace ctl::sdl
{
	/**
	 * @brief Used for getting the fps and delta
	 */
	class FpsCounter
	{
	public:
		/**
		 * @brief Start the counter timer
		 */
		FpsCounter() { m_timer.start(); }

		/**
		 * @brief Mark frame begin and update values
		 */
		void update()
		{
			const auto time = m_timer.ticks<std::chrono::duration<float>>();
			m_timer.start();
			m_elapsed	= time - m_prev_time;
			m_prev_time = time;
		}

		/**
		 * @brief Calculate the delta
		 * @return delta
		 */
		auto delta() -> float { return m_elapsed.count(); }
		/**
		 * @brief Calculate the fps
		 * @return fps
		 */
		auto fps() -> float { return 1 / m_prev_time.count(); }

	private:
		Timer						 m_timer;
		std::chrono::duration<float> m_prev_time;
		std::chrono::duration<float> m_elapsed;
	};
} // namespace ctl::sdl

#endif