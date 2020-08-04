#ifndef _CTL_SDL2_ANIMATION_
#define _CTL_SDL2_ANIMATION_

#include <SDL2/SDL.h>

#include "../BasicTypes.h"
#include "../Timer.h"

#include "Drawable.h"
#include "Texture.h"
#include "TypeTraits.h"

#include <vector>

// DEPRECATED

namespace ctl::sdl
{
	struct AniFrame
	{
		AniFrame(const mth::Rect<int, int> &r, const std::chrono::milliseconds &t)
			: shape(r)
			, step(t)
		{
		}

		/**
		 * @brief Blit of the main texture
		 */
		mth::Rect<int, int> shape;
		/**
		 * @brief Time interval till next frame
		 */
		std::chrono::milliseconds step;
	};

	template<typename T>
	concept animation_iter = std::same_as<typename std::iterator_traits<T>::value_type, AniFrame>;

	/**
	 * @brief Enables animating
	 */
	class Animation : public Texture
	{
	public:
		using base = Texture;

		using Texture::Texture;

		Animation() = default;

		/**
		 * @brief Emplace back a frame
		 * @param param Parameters of construction
		 */
		template<typename... P>
		void emplace_back_frame(P &&... param)
		{
			m_frames.emplace_back(std::forward<P>(param)...);
			m_curr_frame = m_frames.begin();
		}
		/**
		 * @brief Push back a frame
		 * @param f Frame to use
		 */
		void push_back_frame(const AniFrame &f)
		{
			m_frames.push_back(f);
			m_curr_frame = m_frames.begin();
		}
		/**
		 * @brief Assignes frames
		 *
		 * @tparam Iter A iterator pointing to AniFrame
		 * @param begin_frame Begin iterator
		 * @param end_frame End iterator
		 */
		template<animation_iter Iter>
		void assign_frames(Iter begin_frame, Iter end_frame)
		{
			m_frames.assign(begin_frame, end_frame);
			m_curr_frame = m_frames.begin();
		}

		/**
		 * @brief Start animation timer
		 */
		void start_ani() noexcept { m_time.start(); }
		/**
		 * @brief Pause animation timer
		 */
		void pause_ani() noexcept { m_time.pause(); }
		/**
		 * @brief Unpause animation timer
		 */
		void unpause_ani() noexcept { m_time.unpause(); }
		/**
		 * @brief Check if animation timer is paused
		 * @return bool
		 */
		[[nodiscard]] auto is_ani_paused() const noexcept -> bool { return m_time.is_paused(); }

		/**
		 * @brief Return current blit of the texture to draw
		 * The function automatically goes to the next animation frame when the animation timer exceedes the given
		 * interval time.
		 * @return const Rect&
		 */
		auto blit_ani() noexcept -> const auto &
		{
			if (!m_time.is_paused() && !m_frames.empty())
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

		/**
		 * @brief Get the current size of stored frames
		 * @return size_t
		 */
		[[nodiscard]] auto frames_size() const noexcept { return m_frames.size(); }

	private:
		Timer					  m_time;
		std::chrono::milliseconds m_till_next = std::chrono::milliseconds(0);

		std::vector<AniFrame>			m_frames;
		std::vector<AniFrame>::iterator m_curr_frame = m_frames.begin();
	};

	template<template<typename> class... Ex>
	using EAnimation = typename MixBuilder<Animation, Ex...>::type;

} // namespace ctl::sdl
#endif // !_CTL_SDL2_ANIMATION_
