#pragma once

#include <chrono>

namespace ctl
{
	class Timer
	{
	public:
		Timer() = default;
		Timer(const Timer&) = default;
		Timer(Timer&&) = default;

		Timer& operator=(const Timer&) = default;
		Timer& operator=(Timer&&) = default;

		void start() noexcept
		{
			m_started = true;
			m_paused = false;

			m_start = std::chrono::steady_clock::now();
		}

		void stop() noexcept
		{
			m_started = false;
			m_paused = false;
		}

		void pause() noexcept
		{
			if (m_started && !m_paused)
				m_paused = true,
				m_pause = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start);
		}

		void unpause() noexcept
		{
			if (m_started && m_paused)
				m_paused = false,
				m_start = std::chrono::steady_clock::now() - m_pause;
		}

		template<typename Unit = std::chrono::milliseconds>
		Unit ticks() noexcept
		{
			if (m_started)
			{
				if (m_paused)
					return std::chrono::duration_cast<Unit>(m_pause);
				else
					return std::chrono::duration_cast<Unit>(std::chrono::steady_clock::now() - m_start);
			}
			else
				return Unit(0);
		}

		constexpr const bool& isStarted() const noexcept { return m_started; }
		constexpr const bool& isPaused() const noexcept { return m_paused; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_start;
		std::chrono::milliseconds m_pause;

		bool m_paused = false;
		bool m_started = false;
	};
}