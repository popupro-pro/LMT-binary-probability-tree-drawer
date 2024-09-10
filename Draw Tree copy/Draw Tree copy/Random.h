#ifndef RANDOM_MT_H
#define RANDOM_MT_H

#include <chrono>
#include <random>
#include <algorithm>

// This header-only Random namespace implements a self-seeding Mersenne Twister.
// Requires C++17 or newer.
// It can be #included into as many code files as needed (The inline keyword avoids ODR violations)
// Freely redistributable, courtesy of learncpp.com (https://www.learncpp.com/cpp-tutorial/global-random-numbers-random-h/)
namespace Random
{
	// Returns a seeded Mersenne Twister
	// Note: we'd prefer to return a std::seed_seq (to initialize a std::mt19937), but std::seed can't be copied, so it can't be returned by value.
	// Instead, we'll create a std::mt19937, seed it, and then return the std::mt19937 (which can be copied).
	inline std::mt19937 generate()
	{
		std::random_device rd{};

		// Create seed_seq with clock and 7 random numbers from std::random_device
		std::seed_seq ss{
			static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
				rd(), rd(), rd(), rd(), rd(), rd(), rd()};

		return std::mt19937{ss};
	}

	// Here's our global std::mt19937 object.
	// The inline keyword means we only have one global instance for our whole program.
	inline std::mt19937 mt{generate()}; // generates a seeded std::mt19937 and copies it into our global object

	// Generate a random int between [min, max] (inclusive)
	inline int get(int min, int max)
	{
		return std::uniform_int_distribution{min, max}(mt);
	}

	inline double get(double min, double max)
	{
		double rand = get(0, 1000000);
		double doubleRand = (max - min) * rand / (double)1000000 + min;
		return doubleRand;
	}

	// The following function templates can be used to generate random numbers
	// when min and/or max are not type int
	// See https://www.learncpp.com/cpp-tutorial/function-template-instantiation/
	// You can ignore these if you don't understand them

	// Generate a random value between [min, max] (inclusive)
	// * min and max have same type
	// * Return value has same type as min and max
	// * Supported types:
	// *    short, int, long, long long
	// *    unsigned short, unsigned int, unsigned long, or unsigned long long
	// Sample call: Random::get(1L, 6L);             // returns long
	// Sample call: Random::get(1u, 6u);             // returns unsigned int
	template <typename T>
	T get(T min, T max)
	{
		return std::uniform_int_distribution<T>{min, max}(mt);
	}

	// Generate a random value between [min, max] (inclusive)
	// * min and max can have different types
	// * Must explicitly specify return type as template type argument
	// * min and max will be converted to the return type
	// Sample call: Random::get<std::size_t>(0, 6);  // returns std::size_t
	// Sample call: Random::get<std::size_t>(0, 6u); // returns std::size_t
	// Sample call: Random::get<std::int>(0, 6u);    // returns int
	template <typename R, typename S, typename T>
	R get(S min, T max)
	{
		return get<R>(static_cast<R>(min), static_cast<R>(max));
	}
	std::vector<double> GenerateValley(double min, double max, int num)
	{
		int offset = get(0, 1);
		std::vector<double> Left;
		for(int i = 0; i < num / 2 + offset; i++)
		{
			double rand = get(min, max);
			Left.push_back(rand);
		}
		std::sort(Left.begin(), Left.end(), std::greater<double>());
		std::vector<double> Right;
		for(int i = num / 2 + offset; i < num; i++)
		{
			double rand = get(min, max);
			Right.push_back(rand);
		}
		std::sort(Right.begin(), Right.end());
		std::vector<double> All(Left);
		for(int i = 0; i < Right.size(); i++)
		{
			All.push_back(Right[i]);
		}
		return All;
	}
	std::vector<double> GenerateDecending(double min, double max, int num)
	{
		std::vector<double> All(num);
		for(int i = 0; i < num; i++)
		{
			All[i] = get(min, max);
		}
		std::sort(All.begin(), All.end(), std::greater<double>());
		return All;
	}
}

#endif
