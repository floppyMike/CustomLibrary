#pragma once

#include <chrono>

namespace ctl
{
	class Timer
	{
	public:
		Timer()				 = default;
		Timer(const Timer &) = default;
		Timer(Timer &&)		 = default;

		auto operator=(const Timer &) -> Timer & = default;
		auto operator=(Timer &&) -> Timer & = default;

		void start() noexcept
		{
			m_started = true;
			m_paused  = false;

			m_start = std::chrono::steady_clock::now();
		}

		void stop() noexcept
		{
			m_started = false;
			m_paused  = false;
		}

		void pause() noexcept
		{
			if (m_started && !m_paused)
				m_paused = true, m_pause = std::chrono::steady_clock::now() - m_start;
		}

		void unpause() noexcept
		{
			if (m_started && m_paused)
				m_paused = false, m_start = std::chrono::steady_clock::now() - m_pause;
		}

		template<typename Unit = std::chrono::milliseconds>
		auto ticks() noexcept -> Unit
		{
			if (m_started)
				return std::chrono::duration_cast<Unit>(m_paused ? m_pause
																 : std::chrono::steady_clock::now() - m_start);

			return Unit(0);
		}

		[[nodiscard]] constexpr auto is_started() const noexcept { return m_started; }
		[[nodiscard]] constexpr auto is_paused() const noexcept { return m_paused; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_start;
		std::chrono::steady_clock::duration				   m_pause;

		bool m_paused  = false;
		bool m_started = false;
	};
} // namespace ctl