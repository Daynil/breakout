#include <random>

int random_int(int from, int to)
{
	std::random_device rd;  // Seed generator
	std::mt19937 gen(rd()); // Mersenne Twister engine
	std::uniform_int_distribution<> dis(from, to);
	return dis(gen);
}

float random_float(float from, float to)
{
	std::random_device rd;  // Seed generator
	std::mt19937 gen(rd()); // Mersenne Twister engine
	std::uniform_real_distribution<float> dis(from, to);
	return dis(gen);
}