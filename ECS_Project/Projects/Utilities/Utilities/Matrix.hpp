#pragma once
#include <type_traits>
#include <iostream>

template<size_t R, size_t C, typename T = float>
struct Mat final
{
	static_assert(R && C, "Dimensions are zero");

	Mat() {
		std::cout << "ctor\n";
	}
	Mat(const Mat& other) {
		std::cout << "cctor\n"; std::memcpy(&m[0][0], &other.m[0][0], R*C*sizeof(T));
	}
	Mat(Mat&& other) {
		std::cout << "mctor\n"; std::memcpy(&m[0][0], &other.m[0][0], R * C * sizeof(T));
	}
	~Mat() {
		std::cout << "dtor\n";
	}
	Mat& operator=(const Mat& other) {
		std::cout << "cass\n"; std::memcpy(&m[0][0], &other.m[0][0], R * C * sizeof(T)); return *this;
	}
	Mat& operator=(Mat&& other) {
		std::cout << "mass\n"; std::memcpy(&m[0][0], &other.m[0][0], R * C * sizeof(T)); return *this;
	}

	static constexpr size_t Rows = R;
	static constexpr size_t Cols = C;

	T m[Rows][Cols] = { 0 };
};

template<size_t R1, size_t C1, size_t R2, size_t C2, typename T = float>
Mat<R1, C2> mul(const Mat<R1, C1>& left, const Mat<R2, C2>& right, std::enable_if_t<(C1 == R2) && (R1 && C1 && R2 && C2), int> = 0)
{
	Mat<R1, C2> mat;

	for (size_t r3 = 0; r3 < R1; r3++)
	{
		for (size_t c3 = 0; c3 < C2; c3++)
		{
			T sum = 0;
			for (size_t i = 0; i < C1; i++)
			{
				sum += left.m[r3][i] * right.m[i][c3];
			}
			mat.m[r3][c3] = sum;
		}
	}

	return mat;
}