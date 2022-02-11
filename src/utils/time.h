#pragma once

#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>


namespace Avarice {
using namespace std::chrono_literals;

/**
 * @brief Represents a time value stored in microseconds.
 */
class Time {
public:
	Time() = default;
	template<typename Rep, typename Period>
	constexpr Time(const std::chrono::duration<Rep, Period> &duration) :
		value(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) {
	}

	template<typename Rep = float>
	static constexpr Time Seconds(const Rep &seconds) {
		return Time(std::chrono::duration<Rep>(seconds));
	}

	template<typename Rep = int32_t>
	static constexpr Time Milliseconds(const Rep &milliseconds) {
		return Time(std::chrono::duration<Rep, std::micro>(milliseconds));
	}

	template<typename Rep = int64_t>
	static constexpr Time Microseconds(const Rep &microseconds) {
		return Time(std::chrono::duration<Rep, std::micro>(microseconds));
	}

	template<typename T = float>
	constexpr auto AsSeconds() const {
		return static_cast<T>(value.count()) / static_cast<T>(1000000);
	}

	template<typename T = int32_t>
	constexpr auto AsMilliseconds() const {
		return static_cast<T>(value.count()) / static_cast<T>(1000);
	}

	template<typename T = int64_t>
	constexpr auto AsMicroseconds() const {
		return static_cast<T>(value.count());
	}

	//template<typename Period = std::ratio<1, 1>>
	/*float Mod(const Time &other) {
		return std::modf(std::chrono::duration_cast<std::chrono::duration<double, Period>>(value),
			std::chrono::duration_cast<std::chrono::duration<double, Period>>(other.value));
	}*/

	static Time Now() {
		static const auto LocalEpoch = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - LocalEpoch);

		//auto now = std::chrono::system_clock::now();
		//return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
	}

	static std::string GetDateTime(const std::string &format = "%Y-%m-%d %H:%M:%S") {
		auto now = std::chrono::system_clock::now();
		auto timeT = std::chrono::system_clock::to_time_t(now);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&timeT), format.c_str());
		return ss.str();
	}

	template<typename Rep, typename Period>
	constexpr operator std::chrono::duration<Rep, Period>() const {
		return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(value);
	}

	constexpr bool operator==(const Time &rhs) const;
	constexpr bool operator!=(const Time &rhs) const;
	constexpr bool operator<(const Time &rhs) const;
	constexpr bool operator<=(const Time &rhs) const;
	constexpr bool operator>(const Time &rhs) const;
	constexpr bool operator>=(const Time &rhs) const;

	constexpr Time operator-() const;

	friend constexpr Time operator+(const Time &lhs, const Time &rhs);
	friend constexpr Time operator-(const Time &lhs, const Time &rhs);
	friend constexpr Time operator*(const Time &lhs, float rhs);
	friend constexpr Time operator*(const Time &lhs, int64_t rhs);
	friend constexpr Time operator*(float lhs, const Time &rhs);
	friend constexpr Time operator*(int64_t lhs, const Time &rhs);
	friend constexpr Time operator/(const Time &lhs, float rhs);
	friend constexpr Time operator/(const Time &lhs, int64_t rhs);
	friend constexpr double operator/(const Time &lhs, const Time &rhs);

	constexpr Time &operator+=(const Time &rhs);
	constexpr Time &operator-=(const Time &rhs);
	constexpr Time &operator*=(float rhs);
	constexpr Time &operator*=(int64_t rhs);
	constexpr Time &operator/=(float rhs);
	constexpr Time &operator/=(int64_t rhs);

private:
	std::chrono::microseconds value{};
};
}

#include "time.inl"
