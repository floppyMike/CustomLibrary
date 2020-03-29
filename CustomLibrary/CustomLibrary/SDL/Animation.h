#pragma once

#include "../Timer.h"
#include "Texture.h"
#include "../BasicTypes.h"

#include <vector>

namespace ctl::sdl
{
	struct AniFrame
	{
		mth::Rect<int, int> shape;
		std::chrono::milliseconds step;
	};

	template<typename Impl, typename... T>
	class EAnimation : public crtp<Impl, EAnimation, T...>
	{
		//static_assert(tag::has_tag_v<tag::isTexture, T...>, "Parent must be a texture.");

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

		void draw_animated()
		{
			const Impl* const cpthis = this->underlying();

			if (SDL_RenderCopy(cpthis->renderer()->get(), cpthis->texture(), _blit_ani_().rect_ptr(), cpthis->shape().rect_ptr()) < 0)
				throw std::runtime_error(SDL_GetError());
		}

		void draw_animated(double angle, const mth::Point<int>& center, SDL_RendererFlip flip)
		{
			const Impl* const cpthis = this->underlying();

			if (SDL_RenderCopyEx(cpthis->renderer()->get(), cpthis->texture(), _blit_ani_().rect_ptr(), cpthis->shape().rect_ptr(), angle, center.point_ptr(), flip) < 0)
				throw std::runtime_error(SDL_GetError());
		}

		constexpr auto frames_size() const noexcept { return m_frames.size(); }

	private:
		Timer m_time;
		std::chrono::milliseconds m_till_next = std::chrono::milliseconds(0);

		std::vector<AniFrame> m_frames;
		std::vector<AniFrame>::iterator m_curr_frame = m_frames.begin();


		const auto& _blit_ani_() noexcept
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
	};
}