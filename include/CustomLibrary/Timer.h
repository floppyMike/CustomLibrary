#pragma once

#include <chrono>

namespace ctl
{
	/**
	 * @brief Stopwatch like timer
	 */
	class Timer
	{
	public:
		/**
		 * @brief Create a timer
		 */
		Timer() = default;
		/**
		 * @brief Copy a timer
		 */
		Timer(const Timer &) = default;

		/**
		 * @brief Copy a timer
		 */
		auto operator=(const Timer &) -> Timer & = default;

		/**
		 * @brief Start and reset the timer
		 */
		void start() noexcept
		{
			m_started = true;
			m_paused  = false;

			m_start = std::chrono::steady_clock::now();
		}

		/**
		 * @brief Stop and reset the timer
		 *
		 */
		void stop() noexcept
		{
			m_started = false;
			m_paused  = false;
		}

		/**
		 * @brief Pause the timer
		 */
		void pause() noexcept
		{
			if (m_started && !m_paused)
				m_paused = true, m_pause = std::chrono::steady_clock::now() - m_start;
		}

		/**
		 * @brief Unpause the timer
		 */
		void unpause() noexcept
		{
			if (m_started && m_paused)
				m_paused = false, m_start = std::chrono::steady_clock::now() - m_pause;
		}

		/**
		 * @brief Get the timer duration
		 *
		 * @tparam Unit Type of chrono duration to use
		 * @return Unit ticks per Unit
		 */
		template<typename Unit = std::chrono::milliseconds>
		auto ticks() noexcept -> Unit
		{
			if (m_started)
				return std::chrono::duration_cast<Unit>(m_paused ? m_pause
																 : std::chrono::steady_clock::now() - m_start);

			return Unit(0);
		}

		/**
		 * @brief Checks if the timer has started
		 * @return if started
		 */
		[[nodiscard]] constexpr auto is_started() const noexcept { return m_started; }
		/**
		 * @brief Checks if the timer is paused
		 * @return if paused
		 */
		[[nodiscard]] constexpr auto is_paused() const noexcept { return m_paused; }

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_start;
		std::chrono::steady_clock::duration				   m_pause;

		bool m_paused  = false;
		bool m_started = false;
	};
} // namespace ctl