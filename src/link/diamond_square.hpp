// Copyright (c) 2018 Martyn Afford
// Licensed under the MIT licence

#ifndef DIAMOND_SQUARE_HPP
#define DIAMOND_SQUARE_HPP

#include <cassert>

namespace heightfield {

    // Generate a heightfield using random midpoint displacement and the
    // diamond-square algorithm.
    //
    // \param size
    //   Size of the desired heightfield. Valid values are powers of two plus one
    //   (i.e., 2^n + 1), such as 129, 257 and 513. Must be at least five. Invalid
    //   sizes will trigger an assertion failure.
    // \param random
    //   A callback of the form `float(float limit)` that returns a random value
    //   ranging from 0 (inclusive) to limit (exclusive). This function is called
    //   once for each heightfield location excluding the corner points (i.e.,
    //   size^2 - 4 times).
    // \param variance
    //   A callback of the form `float(int level)` that returns the maximum
    //   variance in one direction for the specified level. The level starts from
    //   zero and increases for each recursion. For example, a heightfield of size
    //   129 has seven levels, zero through six. Generally speaking, the variance
    //   halves each iteration. A good start is `constant * pow(0.5f, level)`. This
    //   function is called once for each level.
    // \param at
    //   A callback of the form `U(int x, int y)` that returns a mutable reference
    //   to the heightfield location specified by the coordinates. A coordinate will
    //   range from zero (inclusive) to size (exclusive). The return type U can be
    //   any numeric type but it must be, or act as, a non-const reference to allow
    //   the algorithm to output its results. This allows the user to decide how
    //   the heightfield is stored in memory.
    //
    // The corner points of the heightfield should be initialised to suitable
    // starting values before calling this function.
    //
    // The resulting heightfield is not tileable. Points along the edges are
    // averaged over three neighbouring points instead of the usual four.
    //
    // Variance is defined in one direction but is applied in both. If we assume an
    // integer type for the heightfield and an initial value of 128, then a variance
    // of 100 will result in a range of 28 to 228.
    //
    // No clamping is performed by this algorithm so if the randomness, variance and
    // initial values combine to generate a value smaller or larger than the data
    // type referenced by `at`, underflow or overflow can occur.
    //
    // The time complexity of this algorithm is O(n^2) where n is `size`. The space
    // complexity is constant (and small) as this algorithm makes no allocations
    // internally, relying on the user to provide the data structure and accessing
    // it via `at`.
    template <typename T, typename T2, typename T3>
    void
        diamond_square_no_wrap(int size, T&& random, T2&& variance, T3&& at)
    {
        assert(size >= 5 && ((size - 1) & (size - 2)) == 0 && "valid size");

        auto level = 0;
        auto stride = size - 1;
        auto end = size - 1;

        while (stride > 1) {
            auto range = variance(level);
            auto half = stride / 2;

            // Diamond step:
            for (auto y = half; y < end; y += stride) {
                for (auto x = half; x < end; x += stride) {
                    auto tl = at(x - half, y - half);
                    auto bl = at(x - half, y + half);
                    auto tr = at(x + half, y - half);
                    auto br = at(x + half, y + half);

                    auto average = (tl + tr + br + bl) / 4.0f;
                    auto displacement = random(range * 2.0f) - range;

                    at(x, y) = average + displacement;
                }
            }

            // Square step:
            // Firstly there are four special cases to handle the rows and columns
            // with only three neighbours.

            // Top row - no up
            for (auto x = half; x < end; x += stride) {
                auto down = at(x, half);
                auto left = at(x - half, 0);
                auto right = at(x + half, 0);

                auto average = (down + left + right) / 3.0f;
                auto displacement = random(range * 2.0f) - range;

                at(x, 0) = average + displacement;
            }

            // Bottom row - no down
            for (auto x = half; x < end; x += stride) {
                auto up = at(x, end - half);
                auto left = at(x - half, end);
                auto right = at(x + half, end);

                auto average = (up + left + right) / 3.0f;
                auto displacement = random(range * 2.0f) - range;

                at(x, end) = average + displacement;
            }

            // Left column - no left
            for (auto y = half; y < end; y += stride) {
                auto up = at(0, y - half);
                auto down = at(0, y + half);
                auto right = at(half, y);

                auto average = (up + down + right) / 3.0f;
                auto displacement = random(range * 2.0f) - range;

                at(0, y) = average + displacement;
            }

            // Right column - no right
            for (auto y = half; y < end; y += stride) {
                auto up = at(end, y - half);
                auto down = at(end, y + half);
                auto left = at(end - half, y);

                auto average = (up + down + left) / 3.0f;
                auto displacement = random(range * 2.0f) - range;

                at(end, y) = average + displacement;
            }

            // The remaining points -- the edges all processed as special cases
            // above -- that have four neighbours.
            auto offset = true;

            for (auto y = half; y < end; y += half) {
                for (auto x = offset ? stride : half; x < end; x += stride) {
                    auto up = at(x, y - half);
                    auto down = at(x, y + half);
                    auto left = at(x - half, y);
                    auto right = at(x + half, y);

                    auto average = (up + down + left + right) / 4.0f;
                    auto displacement = random(range * 2.0f) - range;

                    at(x, y) = average + displacement;
                }

                offset = !offset;
            }

            stride /= 2;
            ++level;
        }
    }

    // Generate a tileable heightfield using random midpoint displacement and the
    // diamond-square algorithm.
    //
    // \param size
    //   Size of the desired heightfield. Valid values are powers of two (i.e.,
    //   2^n), such as 128, 256 and 512. Must be at least four. Invalid sizes will
    //   trigger an assertion failure.
    // \param random
    //   A callback of the form `float(float limit)` that returns a random value
    //   ranging from 0 (inclusive) to limit (exclusive). This function is called
    //   once for each heightfield location excluding the initial top left point
    //   (i.e., size^2 - 1 times).
    // \param variance
    //   A callback of the form `float(int level)` that returns the maximum
    //   variance in one direction for the specified level. The level starts from
    //   zero and increases for each recursion. For example, a heightfield of size
    //   128 has seven levels, zero through six. Generally speaking, the variance
    //   halves each iteration. A good start is `constant * pow(0.5f, level)`. This
    //   function is called once for each level.
    // \param at
    //   A callback of the form `U(int x, int y)` that returns a mutable reference
    //   to the heightfield location specified by the coordinates. A coordinate will
    //   range from zero (inclusive) to size (exclusive). The return type U can be
    //   any numeric type but it must be, or act as, a non-const reference to allow
    //   the algorithm to output its results. This allows the user to decide how
    //   the heightfield is stored in memory.
    //
    // The top left point of the heightfield should be initialised to a suitable
    // starting value before calling this function.
    //
    // The resulting heightfield is tileable.
    //
    // Variance is defined in one direction but is applied in both. If we assume an
    // integer type for the heightfield and an initial value of 128, then a variance
    // of 100 will result in a range of 28 to 228.
    //
    // No clamping is performed by this algorithm so if the randomness, variance
    // and initial values combine to generate a value smaller or larger than the
    // data type referenced by `at`, underflow or overflow can occur.
    //
    // The time complexity of this algorithm is O(n^2) where n is `size`. The space
    // complexity is constant (and small) as this algorithm makes no allocations
    // internally, relying on the user to provide the data structure and accessing
    // it via `at`.
    template <typename T, typename T2, typename T3>
    void
        diamond_square_wrap(int size, T&& random, T2&& variance, T3&& at)
    {
        assert(size >= 4 && (size & (size - 1)) == 0 && "valid size");

        auto level = 0;
        auto stride = size;
        auto end = size;
        auto mask = size - 1;

        // The same as 'at' but handles x or y exceeding their maximum, size - 1.
        // As the size is a power of two, simple bit masking can be used. Note that
        // this does not handle negative indices. This is why there are two special
        // cases in the square step code later.
        auto at_wrap = [&at, mask](int x, int y) { return at(x & mask, y & mask); };

        while (stride > 1) {
            auto range = variance(level);
            auto half = stride / 2;

            // Diamond step:
            for (auto y = half; y < end; y += stride) {
                for (auto x = half; x < end; x += stride) {
                    auto tl = at(x - half, y - half);
                    auto bl = at_wrap(x - half, y + half);
                    auto tr = at_wrap(x + half, y - half);
                    auto br = at_wrap(x + half, y + half);

                    auto average = (tl + tr + br + bl) / 4.0f;
                    auto displacement = random(range * 2.0f) - range;

                    at(x, y) = average + displacement;
                }
            }

            // Square step:
            // Special cases for the top row and left column where the x or y index
            // would be negative in the main square step algorithm (e.g., x - half,
            // when x is zero, would be -half). Not adding conditionals to handle
            // negative indices in the main loop improves performance by 3-8%.
            for (auto x = half; x < end; x += stride) {
                auto up = at(x, end - half);
                auto down = at(x, half);
                auto left = at(x - half, 0);
                auto right = at_wrap(x + half, 0);

                auto average = (up + down + left + right) / 4.0f;
                auto displacement = random(range * 2.0f) - range;

                at(x, 0) = average + displacement;
            }

            // As above; special case for the left column.
            for (auto y = half; y < end; y += stride) {
                auto up = at(0, y - half);
                auto down = at_wrap(0, y + half);
                auto left = at(end - half, y);
                auto right = at(half, y);

                auto average = (up + down + left + right) / 4.0f;
                auto displacement = random(range * 2.0f) - range;

                at(0, y) = average + displacement;
            }

            // The remaining points, not including the leftmost and topmost; that
            // is, all coordinates, x or y, with a value of at least half. Thus,
            // x - half and y - half are assured to be at least zero.
            bool offset = true;

            for (auto y = half; y < end; y += half) {
                for (auto x = offset ? stride : half; x < end; x += stride) {
                    auto up = at(x, y - half);
                    auto down = at_wrap(x, y + half);
                    auto left = at(x - half, y);
                    auto right = at_wrap(x + half, y);

                    auto average = (up + down + left + right) / 4.0f;
                    auto displacement = random(range * 2.0f) - range;

                    at(x, y) = average + displacement;
                }

                offset = !offset;
            }

            stride /= 2;
            ++level;
        }
    }

} // namespace heightfield

#endif /* DIAMOND_SQUARE_HPP */



//#pragma once
//
//#include <vector>
//#include <array>
//
//#include <fmt/ostream.h>
//
//#include "types.hpp"
//#include "random.hpp"
//
//namespace link
//{
//	
//	struct DiamondSquare
//	{
//		std::vector<f32> data;
//		i64 side_lenght;
//
//		f32& get(i64 x, i64 y)
//		{
//			return data[x + y * side_lenght];
//		}
//
//		void generate(i64 n, f32 range, f32 roughness, f32 falloff)
//		{
//			i64 size = pow(2, n);
//			fmt::print("Creating terrain data ranging from 0 to {}", size);
//
//			side_lenght = size + 1;
//			data.resize((side_lenght) * (side_lenght), 0.0f);
//
//			get(0, 0) = CRandom::range_f(-range, range);
//			get(0, size) = CRandom::range_f(-range, range);
//			get(size, 0) = CRandom::range_f(-range, range);
//			get(size, size) = CRandom::range_f(-range, range);
//
//			step(0, 0, size, roughness, falloff);
//
//			//i32 step_size = size;
//
//			//while (step_size > 1)
//			//{
//			//	diamond_step
//			//}
//
//		}
//
//		std::vector<u64> indices;
//
//
//
//		// represents the top left corner of an already filled square
//		void step(i64 x, i64 y, i64 reach, f32 roughness, f32 falloff)
//		{
//			if (x < side_lenght && y < side_lenght)
//			{
//				i64 new_reach = reach / 2;
//
//				if (new_reach < 1)
//				{
//					return;
//				}
//
//				diamond_step(x + new_reach, y + new_reach, new_reach, roughness);
//
//				square_step(x + new_reach, y			, new_reach, roughness);
//				square_step(x			 , y + new_reach, new_reach, roughness);
//				square_step(x + reach	 , y + new_reach, new_reach, roughness);
//				square_step(x + new_reach, y + reach	, new_reach, roughness);
//
//				roughness *= falloff;
//
//				step(x, y, new_reach, roughness, falloff);
//				step(x + new_reach, y, new_reach, roughness, falloff);
//				step(x, y + new_reach, new_reach, roughness, falloff);
//				step(x + new_reach, y + new_reach, new_reach, roughness, falloff);
//			}
//		}
//		
//		// x & y represent the coordinates of the value we're filling in
//		void square_step(i64 x, i64 y, i64 reach, f32 roughness)
//		{
//			if (x < side_lenght && y < side_lenght)
//			{
//				f32 avg = 0.0f;
//				u32 count = 0;
//
//				if (x + reach < side_lenght)
//				{
//					avg += get(x + reach, y);
//					count++;
//				}
//				if (x - reach >= 0)
//				{
//					avg += get(x - reach, y);
//					count++;
//				}
//				if (y + reach < side_lenght)
//				{
//					avg += get(x, y + reach);
//					count++;
//				}
//				if (y - reach >= 0)
//				{
//					avg += get(x, y - reach);
//					count++;
//				}
//
//				avg /= count;
//				if (get(x, y) == 0.0f)
//				{
//					get(x, y) = avg + CRandom::range_f(-roughness, roughness);
//					indices.emplace_back(x + y * side_lenght);
//				}
//			}
//		}
//
//		// x & y represent the coordinates of the value we're filling in
//		void diamond_step(i64 x, i64 y, i64 reach, f32 roughness)
//		{
//			if (x < side_lenght && y < side_lenght)
//			{
//				f32 avg = 0.0f;
//				u32 count = 0;
//
//				if (x - reach >= 0 && y - reach >= 0)
//				{
//					avg += get(x - reach, y - reach);
//					count++;
//				}
//				if (x - reach >= 0 && y + reach < side_lenght)
//				{
//					avg += get(x - reach, y + reach);
//					count++;
//				}
//				if (x + reach < side_lenght && y - reach >= 0)
//				{
//					avg += get(x + reach, y - reach);
//					count++;
//				}
//				if (x + reach < side_lenght && y + reach < side_lenght)
//				{
//					avg += get(x + reach, y + reach);
//					count++;
//				}
//
//				avg /= count;
//				if (get(x, y) == 0.0f)
//				{
//					get(x, y) = avg + CRandom::range_f(-roughness, roughness);
//					indices.emplace_back(x + y * side_lenght);
//				}
//			}
//		}
//
//		void square_step_v2(i64 half, i64 size, f32 roughness)
//		{
//			for (u64 x = 0; x < side_lenght - 1; x += size)
//			{
//				for (u64 y = 0; y < side_lenght - 1; y += size)
//				{
//					f32 avg = get(x, y);
//					i64 count = 1;
//
//					if (x + size < side_lenght)
//					{
//						avg += get(x + size, y);
//						count++;
//					}
//					if (y + size < side_lenght)
//					{
//						avg += get(x, y + size);
//						count++;
//					}
//					if (x + size < side_lenght && y + size < side_lenght)
//					{
//						avg += get(x + size, y + size);
//						count++;
//					}
//
//					get(x + half, y + half) = avg / (f32)count + CRandom::range_f(-roughness, roughness);
//					indices.emplace_back(get(x, y));
//				}
//			}
//		}
//
//		void diamond_step_v2(i64 half, i64 size, f32 roughness)
//		{
//			for (u64 x = 0; x < side_lenght - 1; x += half)
//			{
//				for (u64 y = x + half % size; y < side_lenght - 1; y += size)
//				{
//					f32 avg = 0.0f;
//					i64 count = 0;
//
//					if (x + half < side_lenght)
//					{
//						avg += get(x + half, y);
//						count++;
//					}
//					if (x - half >= 0)
//					{
//						avg += get(x - half, y);
//						count++;
//					}
//					if (y + half < side_lenght)
//					{
//						avg += get(x, y + half);
//						count++;
//					}
//					if (y - half >= 0)
//					{
//						avg += get(x, y - half);
//						count++;
//					}
//
//					get(x, y) = avg / (f32)count + CRandom::range_f(-roughness, roughness);
//					indices.emplace_back(get(x, y));
//				}
//			}
//		}
//
//		void generate_v2(i64 n, f32 range, f32 roughness, f32 falloff)
//		{
//			i64 size = pow(2, n);
//			fmt::print("Creating terrain data ranging from 0 to {}", size);
//
//			side_lenght = size + 1;
//			data.resize((side_lenght) * (side_lenght), 0.0f);
//
//			get(0, 0) = CRandom::range_f(-range, range);
//			get(0, size) = CRandom::range_f(-range, range);
//			get(size, 0) = CRandom::range_f(-range, range);
//			get(size, size) = CRandom::range_f(-range, range);
//
//			while (size > 1)
//			{
//				i64 half = size / 2;
//				square_step_v2(half, size, roughness);
//				diamond_step_v2(half, size, roughness);
//				size /= 2;
//				roughness *= falloff;
//			}
//		}
//
//		std::vector<std::vector<f32>> height_map;
//		f32 height_min = 1.0f;
//		f32 height_max = 16.0f;
//		i64 height_map_size = 129;
//		f32 invalid = -99999.0f;
//
//		f32 get_value_v3(i64 x, i64 y)
//		{
//			if (x >= 0 && y >= 0 && x < height_map_size && y < height_map_size)
//			{
//				return height_map[x][y];
//			}
//			return invalid;
//		}
//
//		void generate_v3()
//		{
//			height_map.resize(height_map_size);
//			for (auto& height_vec : height_map)
//			{
//				height_vec.resize(height_map_size);
//			}
//
//			height_map[0][0] = CRandom::range_f(-height_min, height_max);
//			height_map[0][height_map_size - 1] = CRandom::range_f(-height_min, height_max);
//			height_map[height_map_size - 1][0] = CRandom::range_f(-height_min, height_max);
//			height_map[height_map_size - 1][height_map_size - 1] = CRandom::range_f(-height_min, height_max);
//
//			i64 chunk_size = height_map_size - 1;
//
//			while (chunk_size > 1)
//			{
//				i64 chunk_size_half = chunk_size / 2;
//
//				for (i64 y = 0; y <= height_map_size - 2; y += chunk_size)
//				{
//					for (i64 x = 0; x <= height_map_size - 2; x += chunk_size)
//					{
//						f32 sum = 0.0f;
//						sum += height_map[y][x];
//						sum += height_map[y][x + chunk_size];
//						sum += height_map[y + chunk_size][x];
//						sum += height_map[y + chunk_size][x + chunk_size];
//
//						height_map[y + chunk_size_half][x + chunk_size_half] = sum / 4.0f;
//					}
//				}
//
//				for (i64 y = 0; y <= height_map_size - 1; y += chunk_size_half)
//				{
//					for (i64 x = y + chunk_size_half % chunk_size; x <= height_map_size - 1; x += chunk_size)
//					{
//						f32 sum = 0.0f;
//						i64 count = 0;
//
//						if (x - chunk_size_half > 0)
//						{
//							sum += height_map[y][x - chunk_size_half];
//							count++;
//						}
//						if (x + chunk_size_half < height_map_size)
//						{
//							sum = sum + height_map[y][x + chunk_size_half];
//							count++;
//						}
//
//						if (y - chunk_size_half > 0)
//						{
//							sum = sum + height_map[y - chunk_size_half][x];
//							count++;
//						}
//						if (y + chunk_size_half < height_map_size)
//						{
//							sum = sum + height_map[y + chunk_size_half][x];
//							count++;
//						}
//						height_map[y][x] = sum / (f32)count;
//					}
//				}
//
//				chunk_size = chunk_size / 2;
//			}
//		}
//	};
//}