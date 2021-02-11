#if not defined _CTL_SDL2_LOAD_
#define _CTL_SDL2_LOAD_

#include "../Traits.h"

#include "Renderer.h"
#include "TypeTraits.h"

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// Loader Implementations
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename, typename, typename>
		class _Loader_;

#if defined _CTL_SDL2_TEXTURE_
		/**
		 * @brief Handles Texture Loading
		 */
		template<typename Full, uses_sdl_renderer Impl>
		class _Loader_<Texture, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Loads Texture from Surface
			 */
			auto surface(SDL_Surface *surface) const -> void
			{
				this->obj()->texture(SDL_CreateTextureFromSurface(this->renderer()->get(), surface));
			}

			/**
			 * @brief Loads Texture from a file
			 * @param path Path to File
			 */
			auto file(const char *path) -> void
			{
				this->obj()->texture(IMG_LoadTexture(this->renderer()->get(), path));
			}

			/**
			 * @brief Loads Texture from series of bytes
			 *
			 * @param src Byte start address
			 * @param size Byte array size
			 */
			auto bytes(void *src, int size) -> void
			{
				this->obj()->texture(IMG_LoadTexture_RW(this->renderer()->get(), SDL_RWFromMem(src, size), 1));
			}
		};
#endif

#if defined _CTL_SDL2_Text_
		/**
		 * @brief Handles Text Loading
		 */
		template<typename Full, uses_sdl_renderer Impl>
		class _Loader_<Text, Full, Impl> : public Impl
		{
			auto _load_(SDL_Surface *s, const char *text) -> void
			{
				this->obj()->text(SDL_CreateTextureFromSurface(this->renderer()->get(), s), text);
				SDL_FreeSurface(s);
			}

		public:
			using Impl::Impl;

			/**
			 * @brief Loads Text Fast
			 *
			 * @param f Font ptr
			 * @param text Text to draw
			 * @param color Color of the text -> Default: White
			 */
			auto solid(TTF_Font *f, const char *text, const SDL_Color &colour = { 0, 0, 0, 0xFF }) -> void
			{
				return _load_(TTF_RenderUTF8_Solid(f, text, colour), text);
			}

			/**
			 * @brief Loads Text with highlighting
			 *
			 * @param f Font to use
			 * @param text Text to draw
			 * @param bg Background color
			 * @param colour Foreground color -> Default: White
			 */
			auto shaded(TTF_Font *f, const char *text, const SDL_Color &bg, const SDL_Color &colour = { 0, 0, 0, 0xFF })
				-> void
			{
				return _load_(TTF_RenderUTF8_Shaded(f, text, colour, bg), text);
			}

			/**
			 * @brief Loads quality Text
			 *
			 * @param f Font to use
			 * @param text Text to draw
			 * @param colour Foreground color -> Default: White
			 */
			auto blended(TTF_Font *f, const char *text, const SDL_Color &colour = { 0, 0, 0, 0xFF }) -> void
			{
				return _load_(TTF_RenderUTF8_Blended(f, text, colour), text);
			}

			/**
			 * @brief Loads quality Text with newlines
			 *
			 * @param f Font to use
			 * @param text Text to draw
			 * @param wrapper Character limit till newline
			 * @param colour Foreground color -> Default: White
			 */
			auto wrapped(TTF_Font *f, const char *text, const Uint16 &wrapper,
						 const SDL_Color &colour = { 0, 0, 0, 0xFF }) -> void
			{
				return _load_(TTF_RenderUTF8_Blended_Wrapped(f, text, colour, wrapper), text);
			}
		};

		/**
		 * @brief Handles Font Loading
		 */
		template<typename Full, typename Impl>
		class _Loader_<Font, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Loads font file
			 *
			 * @param path Path to find file
			 * @param pt Font size in pixel
			 */
			auto file(const char *path, int pt) -> void
			{
				auto *temp = TTF_OpenFont(path, pt);
				ASSERT(temp, TTF_GetError());
				this->obj()->font(temp);
			}
		};
#endif

#if defined _CTL_SDL2_MUSIC_
		/**
		 * @brief Handles Music Loading
		 */
		template<typename Full, typename Impl>
		class _Loader_<Music, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Loads music file
			 * @param path Path to File
			 */
			auto file(std::string_view path) -> void
			{
				Mix_Music *temp = Mix_LoadMUS(path.data());
				ASSERT(temp, Mix_GetError());

				this->obj()->music(temp);
			}
		};
#endif

	} // namespace detail

	// -----------------------------------------------------------------------------
	// Loading Extension
	// -----------------------------------------------------------------------------

	/**
	 * @brief Type for loading type construction
	 * @tparam T Object to load for type
	 */
	template<typename T>
	using Load = typename Filter<detail::_Loader_, Functor<T>, T>::type;

	/**
	 * @brief Type for loading type with renderer construction
	 * @tparam T Object to load for type
	 */
	template<typename T, typename Renderer>
	using LoadR = typename Filter<detail::_Loader_, FunctorR<T, Renderer>, T>::type;

	/**
	 * @brief Shows loading options for object
	 *
	 * @param ptr ptr to object
	 * @return Load type for further options
	 */
	template<typename _T>
	auto load(_T *const ptr)
	{
		return Load<_T>(ptr);
	}

	/**
	 * @brief Shows loading options using renderer for object
	 *
	 * @param ptr ptr to object
	 * @param r ptr to renderer
	 * @return Load type for further options
	 */
	template<typename _T, typename _Renderer>
	auto load(_T *const ptr, _Renderer *r)
	{
		return LoadR<_T, _Renderer>(ptr, r);
	}

} // namespace ctl::sdl

#endif