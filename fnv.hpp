/* This file is part of hdd_serial_spoofer by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#pragma once
#include <cstdint>
#include <type_traits>

// Implements FNV-1a hash algorithm
namespace detail
{
	template <typename Type, Type OffsetBasis, Type Prime>
	struct SizeDependantData
	{
		using type = Type;

		constexpr static auto k_offset_basis = OffsetBasis;
		constexpr static auto k_prime = Prime;
	};

	template <std::size_t Bits>
	struct SizeSelector : std::false_type {};

	template <>
	struct SizeSelector<32> : SizeDependantData<std::uint32_t, 0x811c9dc5ul, 16777619ul> {};

	template <>
	struct SizeSelector<64> : SizeDependantData<std::uint64_t, 0xcbf29ce484222325ull, 1099511628211ull> {};

	template <std::size_t Size>
	class FnvHash
	{
	private:
		using data_t = SizeSelector<Size>;

	public:
		using hash = typename data_t::type;

	private:
		constexpr static auto k_offset_basis = data_t::k_offset_basis;
		constexpr static auto k_prime = data_t::k_prime;

	public:
		static __forceinline constexpr auto hash_init(
		) -> hash
		{
			return k_offset_basis;
		}

		static __forceinline constexpr auto hash_byte(
			hash current,
			std::uint8_t byte
		) -> hash
		{
			return (current ^ byte) * k_prime;
		}

		template <std::size_t N>
		static __forceinline constexpr auto hash_constexpr(
			const char (&str)[N],
			const std::size_t size = N - 1 /* do not hash the null */
		) -> hash
		{
			const auto prev_hash = size == 1 ? hash_init() : hash_constexpr(str, size - 1);
			const auto cur_hash = hash_byte(prev_hash, str[size - 1]);
			return cur_hash;
		}

		static auto __forceinline hash_runtime_data(
			const void* data,
			const std::size_t sz
		) -> hash
		{
			const auto bytes = static_cast<const uint8_t*>(data);
			const auto end = bytes + sz;
			auto result = hash_init();
			for (auto it = bytes; it < end; ++it)
				result = hash_byte(result, *it);

			return result;
		}

		static auto __forceinline hash_runtime(
			const char* str
		) -> hash
		{
			auto result = hash_init();
			do
				result = hash_byte(result, *str++);
			while (*str != '\0');

			return result;
		}
	};
}

using fnv32 = ::detail::FnvHash<32>;
using fnv64 = ::detail::FnvHash<64>;
using fnv = ::detail::FnvHash<sizeof(void*) * 8>;

#define FNV(str) (std::integral_constant<fnv::hash, fnv::hash_constexpr(str)>::value)
#define FNV32(str) (std::integral_constant<fnv32::hash, fnv32::hash_constexpr(str)>::value)
#define FNV64(str) (std::integral_constant<fnv64::hash, fnv64::hash_constexpr(str)>::value)
