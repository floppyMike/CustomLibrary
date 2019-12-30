#pragma once

#include "../Timer.h"
#include "Texture.h"
#include "BasicTypes.h"

#include <vector>

namespace ctl::sdl
{
	struct AniFrame
	{
		Rect<int, int> shape;
		std::chrono::milliseconds step;
	};

	template<typename Impl, typename... T>
	class EAnimation
	{
		static_assert(has_tag_v<Tags::isRect, T...>, "Parent must be a rectangle.");
		Impl* const pthis = static_cast<Impl*>(this);

	public:
		EAnimation() = default;

		void push_frame(AniFrame&& f)
		{
			m_frames.emplace_back(f);
			m_curr_frame = m_frames.begin();
		}
		void assign_frames(std::initializer_list<AniFrame>&& frames)
		{
			m_frames.assign(frames);
			m_curr_frame = m_frames.begin();
		}

		void start_ani() noexcept
		{
			m_time.start();
		}
		void pause_ani() noexcept
		{
			m_time.pause();
		}
		void unpause_ani() noexcept
		{
			m_time.unpause();
		}
		bool is_ani_paused() noexcept
		{
			m_time.isPaused();
		}

		const auto& blit_ani() noexcept
		{
			if (!m_time.isPaused() && !m_frames.empty())
			{
				m_till_next += m_time.ticks();

				while (m_till_next >= m_curr_frame->step)
				{
					m_till_next -= m_curr_frame->step;
					++m_curr_frame;

					if (m_curr_frame == m_frames.end())
						m_curr_frame = m_frames.begin();
				}

				m_time.start();
			}

			return m_curr_frame->shape;
		}

		constexpr auto frames_size() const noexcept { return m_frames.size(); }

	private:
		Timer m_time;
		std::chrono::milliseconds m_till_next = std::chrono::milliseconds(0);

		std::vector<AniFrame> m_frames;
		std::vector<AniFrame>::iterator m_curr_frame = m_frames.begin();
	};
}