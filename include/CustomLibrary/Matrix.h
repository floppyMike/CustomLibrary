#ifndef _CTL_MATRIX_
#define _CTL_MATRIX_

#include <vector>
#include <functional>

#include "Traits.h"
#include "Dim.h"

namespace ctl::mth
{
	// -----------------------------------------------------------------------------
	// Matrix Implementation
	// -----------------------------------------------------------------------------

	template<arithmetic Type, typename Allocator = std::allocator<Type>>
	class Matrix
	{
	public:
		/**
		 * @brief Construct a empty Matrix
		 */
		constexpr Matrix() = default;
		/**
		 * @brief Copy a matrix
		 */
		constexpr Matrix(const Matrix &) = default;
		/**
		 * @brief Move a matrix
		 */
		constexpr Matrix(Matrix &&) = default;
		/**
		 * @brief Create Matrix from dimensions (reserved).
		 *
		 * @param r Rows
		 * @param c Columns
		 */
		constexpr Matrix(size_t r, size_t c)
			: m_dim{ c, r }
		{
			m_data.reserve(r * c);
		}
		/**
		 * @brief Create and initialize matrix from dimension
		 *
		 * @param r Rows
		 * @param c Columns
		 * @param val Value to init
		 */
		constexpr Matrix(size_t r, size_t c, Type val)
			: m_dim{ c, r }
			, m_data(r * c, val)
		{
		}
		/**
		 * @brief Create a single matrix with only rows and 1 column
		 * @param init Data to fill it with
		 */
		constexpr Matrix(size_t r, size_t c, std::initializer_list<Type> &&init)
			: m_dim{ c, r }
			, m_data(std::move(init))
		{
			assert(c * r == m_data.size() && "Dimensions must match the given initializer list.");
		}
		/**
		 * @brief Construct a matrix from iterators
		 * @param begin from iterator
		 * @param end till iterator
		 */
		template<typename _Iter>
		constexpr Matrix(_Iter begin, _Iter end) requires iter_matches<Type>
			: m_dim{ 1, std::distance(begin, end) }
			, m_data(begin, end)
		{
		}

		/**
		 * @brief Construct a matrix using a generator function
		 *
		 * @param r Rows
		 * @param c Columns
		 * @param generator function to use for filling out the matrix
		 */
		template<std::invocable _Gen>
		constexpr Matrix(size_t r, size_t c, _Gen generator)
			: m_dim{ c, r }
		{
			m_data.reserve(m_dim.area());
			while (m_data.size() < m_data.capacity()) m_data.emplace_back(generator());
		}

		//~Matrix()
		//{
		//	std::clog << "Destructor\n";
		//}

		/**
		 * @brief Matrix copy operator
		 */
		constexpr auto operator=(const Matrix &m) -> Matrix & = default;
		/**
		 * @brief Matrix move operator
		 */
		constexpr auto operator=(Matrix &&) noexcept -> Matrix & = default;
		/**
		 * @brief Set matrix to a value
		 *
		 * @param t
		 * @return auto&
		 */
		constexpr auto operator=(Type t) noexcept -> auto &
		{
			return apply([t](Type) constexpr { return t; });
		}

		/**
		 * @brief Get a constant value from the matrix
		 * @param c column
		 * @param r row
		 */
		constexpr auto operator()(size_t c, size_t r) const noexcept -> Type
		{
			assert(c < m_dim.w && r < m_dim.h);
			return m_data[c + m_dim.w * r];
		}
		/**
		 * @brief Get a value the matrix
		 * @param c column
		 * @param r row
		 */
		constexpr auto operator()(size_t c, size_t r) noexcept -> Type &
		{
			assert(c < m_dim.w && r < m_dim.h);
			return m_data[c + m_dim.w * r];
		}

		/**
		 * @brief Get the matrix data as a const vals*
		 * @return data in form of ptr to const data
		 */
		constexpr auto data() const noexcept -> const Type * { return m_data.data(); }

		/**
		 * @brief Get the direct matrix location without column or row. Used primarily for iteration.
		 * @param loc Direct location
		 * @return Value
		 */
		constexpr auto operator[](size_t loc) noexcept -> Type &
		{
			assert(loc < m_data.size() && "Direct location of matrix is too large.");
			return m_data[loc];
		}
		/**
		 * @brief Get the direct matrix location without column or row. Used primarily for iteration.
		 * @param loc Direct location
		 * @return Value
		 */
		constexpr auto operator[](size_t loc) const noexcept -> Type
		{
			assert(loc < m_data.size() && "Direct location of matrix is too large.");
			return m_data[loc];
		}

		/**
		 * @brief Get the dimensions of the matrix
		 * @return Dim
		 */
		[[nodiscard]] constexpr auto dim() const noexcept -> const Dim<size_t> & { return m_dim; }

		/**
		 * @brief Perform a dot product with a other matrix
		 * @param mat2 Other matrix
		 * @return Result
		 */
		template<typename T, typename Alloc>
		constexpr auto dot_product(const Matrix<T, Alloc> &mat2) const noexcept
		{
			assert(m_dim.w == mat2.dim().h && "Width must be same a height for dot product.");

			Matrix<std::common_type_t<T, Type>, Allocator> mat(m_dim.h, mat2.dim().w, 0);

			for (size_t y1 = 0; y1 < m_dim.h; ++y1)
				for (size_t x2 = 0; x2 < mat2.dim().w; ++x2)
					for (size_t x1 = 0; x1 < m_dim.w; ++x1) mat(x2, y1) += (*this)(x1, y1) * mat2(x2, x1);

			return mat;
		}

		/**
		 * @brief Transpose the matrix to a new matrix
		 * @return Transposed matrix
		 */
		constexpr auto transpose() const noexcept
		{
			Matrix mat(m_dim.w, m_dim.h);

			for (size_t x = 0; x < m_dim.w; ++x)
				for (size_t y = 0; y < m_dim.h; ++y) mat.emplace_back((*this)(x, y));

			return mat;
		}

		/**
		 * @brief Apply a function with a seperate iterator onto the matrix
		 * @param func Binary function
		 * @param i2 Other container iterator
		 */
		template<typename _F, typename _Iter>
		constexpr auto apply(_F func, _Iter i2) noexcept -> auto &
		{
			std::transform(m_data.begin(), m_data.end(), i2, m_data.begin(), func);
			return *this;
		}
		/**
		 * @brief Apply a function onto the matrix
		 * @param func Unary function
		 */
		template<typename F>
		constexpr auto apply(F func) noexcept -> auto &
		{
			std::transform(m_data.begin(), m_data.end(), m_data.begin(), std::move(func));
			return *this;
		}

		/**
		 * @brief Emplace_back a reserved container (must have capacity left)
		 * @param ele Value to push
		 */
		constexpr auto emplace_back(Type ele) -> auto &
		{
			assert(m_data.size() < m_data.capacity() && "Matrix emplace back overflowed the capacity.");
			m_data.emplace_back(ele);
			return *this;
		}

		/**
		 * @brief Gets the total size of the matrix
		 * @return size
		 */
		[[nodiscard]] constexpr auto size() const noexcept -> size_t { return m_dim.area(); }

		/**
		 * @brief Iterator const begin
		 * @return begin
		 */
		constexpr auto begin() const noexcept { return m_data.begin(); }
		/**
		 * @brief Iterator begin
		 * @return begin
		 */
		constexpr auto begin() noexcept { return m_data.begin(); }

		/**
		 * @brief Iterator const end
		 * @return end
		 */
		constexpr auto end() const noexcept { return m_data.end(); }
		/**
		 * @brief Iterator end
		 * @return end
		 */
		constexpr auto end() noexcept { return m_data.end(); }

	private:
		Dim<size_t>					 m_dim;
		std::vector<Type, Allocator> m_data;
	};

	// -----------------------------------------------------------------------------
	// Arithmitic Overloads
	// -----------------------------------------------------------------------------

	template<arithmetic T, typename Alloc>
	constexpr auto operator-(Matrix<T, Alloc> m) noexcept
	{
		return m.apply([](T t) constexpr { return -t; });
	}

	// --------------------------------- lv -----------------------------------------

	template<arithmetic T, typename Alloc>
	constexpr auto operator+=(Matrix<T, Alloc> &mat, T x) noexcept -> auto &
	{
		return mat.apply([x](T y) constexpr { return y + x; });
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator-=(Matrix<T, Alloc> &mat, T x) noexcept -> auto &
	{
		return mat.apply([x](T y) constexpr { return y - x; });
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator*=(Matrix<T, Alloc> &mat, T x) noexcept -> auto &
	{
		return mat.apply([x](T y) constexpr { return y * x; });
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator/=(Matrix<T, Alloc> &mat, T x) noexcept -> auto &
	{
		return mat.apply([x](T y) constexpr { return y / x; });
	}

#define _ELEMENT_WISE_CHECK_(dim1, dim2) \
	assert(dim1 == dim2 && "Elementwise applications need both matricies to be the same in dimension.");

	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator+=(Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &
	{
		_ELEMENT_WISE_CHECK_(mat1.dim(), mat2.dim());
		return mat1.apply(std::plus{}, mat2.begin());
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator-=(Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &
	{
		_ELEMENT_WISE_CHECK_(mat1.dim(), mat2.dim());
		return mat1.apply(std::minus{}, mat2.begin());
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator*=(Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &
	{
		_ELEMENT_WISE_CHECK_(mat1.dim(), mat2.dim());
		return mat1.apply(std::multiplies{}, mat2.begin());
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator/=(Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &
	{
		_ELEMENT_WISE_CHECK_(mat1.dim(), mat2.dim());
		return mat1.apply(std::divides{}, mat2.begin());
	}

	// --------------------------------- rv (&&) -----------------------------------------

	template<arithmetic T, typename Alloc>
	constexpr auto operator+(Matrix<T, Alloc> &&mat, T x) noexcept -> auto &&
	{
		return mat += x;
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator-(Matrix<T, Alloc> &&mat, T x) noexcept -> auto &&
	{
		return mat -= x;
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator*(Matrix<T, Alloc> &&mat, T x) noexcept -> auto &&
	{
		return mat *= x;
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator/(Matrix<T, Alloc> &&mat, T x) noexcept -> auto &&
	{
		return mat /= x;
	}

	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator+(Matrix<T1, Alloc1> &&mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &&
	{
		return mat1 += mat2;
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator-(Matrix<T1, Alloc1> &&mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &&
	{
		return mat1 -= mat2;
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator*(Matrix<T1, Alloc1> &&mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &&
	{
		return mat1 *= mat2;
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator/(Matrix<T1, Alloc1> &&mat1, const Matrix<T2, Alloc2> &mat2) noexcept -> auto &&
	{
		return mat1 /= mat2;
	}

	// forwarding right

	template<typename Type, typename Allocator>
	constexpr auto operator+(const Matrix<Type, Allocator> &m1, Matrix<Type, Allocator> &&m2) -> auto &&
	{
		return m2 += m1;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator-(const Matrix<Type, Allocator> &m1, Matrix<Type, Allocator> &&m2) -> auto &&
	{
		return -m2 + m1;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator*(const Matrix<Type, Allocator> &m1, Matrix<Type, Allocator> &&m2) -> auto &&
	{
		return m2 *= m1;
	}

	// forwarding both

	template<typename Type, typename Allocator>
	constexpr auto operator+(Matrix<Type, Allocator> &&m1, Matrix<Type, Allocator> &&m2) -> auto &&
	{
		return m1 += m2;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator-(Matrix<Type, Allocator> &&m1, Matrix<Type, Allocator> &&m2) -> auto &&
	{
		return m1 -= m2;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator*(Matrix<Type, Allocator> &&m1, Matrix<Type, Allocator> &&m2) -> auto &&
	{
		return m1 *= m2;
	}

	// --------------------------------- rv (const) -----------------------------------------

	template<arithmetic T, typename Alloc>
	constexpr auto operator+(const Matrix<T, Alloc> &mat, T x) noexcept
	{
		return Matrix<T, Alloc>(mat) + x;
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator-(const Matrix<T, Alloc> &mat, T x) noexcept
	{
		return Matrix<T, Alloc>(mat) - x;
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator*(const Matrix<T, Alloc> &mat, T x) noexcept
	{
		return Matrix<T, Alloc>(mat) * x;
	}
	template<arithmetic T, typename Alloc>
	constexpr auto operator/(const Matrix<T, Alloc> &mat, T x) noexcept
	{
		return Matrix<T, Alloc>(mat) / x;
	}

	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator+(const Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept
	{
		return Matrix<T1, Alloc1>(mat1) + mat2;
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator-(const Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept
	{
		return Matrix<T1, Alloc1>(mat1) - mat2;
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator*(const Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept
	{
		return Matrix<T1, Alloc1>(mat1) * mat2;
	}
	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator/(const Matrix<T1, Alloc1> &mat1, const Matrix<T2, Alloc2> &mat2) noexcept
	{
		return Matrix<T1, Alloc1>(mat1) / mat2;
	}

	// forwarding

	template<typename Type, typename Allocator>
	constexpr auto operator+(Type x, const Matrix<Type, Allocator> &m) noexcept
	{
		return m + x;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator-(Type x, const Matrix<Type, Allocator> &m) noexcept
	{
		return -m + x;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator*(Type x, const Matrix<Type, Allocator> &m) noexcept
	{
		return m * x;
	}
	template<typename Type, typename Allocator>
	constexpr auto operator/(Type x, Matrix<Type, Allocator> m) noexcept
	{
		return m.apply([x](Type y) constexpr { return x / y; });
	}

	// -----------------------------------------------------------------------------
	// Boolean Overloads
	// -----------------------------------------------------------------------------

	template<typename T1, typename Alloc1, typename T2, typename Alloc2>
	constexpr auto operator==(const Matrix<T1, Alloc1> &m1, const Matrix<T2, Alloc2> &m2) -> bool
	{
		_ELEMENT_WISE_CHECK_(m1.dim(), m2.dim());
		for (auto [i1, i2] = { m1.begin(), m2.begin() }; i1 != m1.end(); ++i1, ++i2)
			if (*i1 != *i2)
				return false;
		return true;
	}

} // namespace ctl::mth

#endif
