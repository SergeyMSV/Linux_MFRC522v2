#include "Arduino.h"
#include <iomanip>
#include <iostream>
#include <chrono>
#include <thread>

std::chrono::steady_clock::time_point g_time_start;

void Print::print(const std::string& val)
{
	std::cout << val;
}

void Print::print(byte val)
{
	std::cout << static_cast<int>(val);
}

void Print::print(byte val, DigitType stat)
{
	if (stat == HEX)
	{
		std::ios_base::fmtflags f(std::cout.flags());
		std::cout << std::hex << static_cast<int>(val);
		//std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(val);
		std::cout.flags(f);
		return;
	}

	std::cout << static_cast<int>(val);
}

void Print::println(const std::string& val)
{
	print(val);
	println();
}

void Print::println(byte val)
{
	print(val);
	println();
}

void Print::println(byte val, DigitType stat)
{
	print(val, stat);
	println();
}

void Print::println()
{
	std::cout << '\n';
}

void delay(int val)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(val));
}

void delayMicroseconds(int val)
{
	std::this_thread::sleep_for(std::chrono::microseconds(val));
}

void pinMode(byte pin, GPIOMode mode)
{
}

GPIOState g_GPIO_Rst = GPIOState::LOW;
GPIOState g_GPIO_CS = GPIOState::LOW;

void digitalWrite(byte pin, GPIOState state)
{
	if (pin == SS_PIN)
		g_GPIO_CS = state;

	if (pin == RST_PIN)
		g_GPIO_Rst = state;
}

GPIOState digitalRead(byte pin)
{
	if (pin == SS_PIN)
		return g_GPIO_CS;

	if (pin == RST_PIN)
		return g_GPIO_Rst;

	return GPIOState::LOW;
}

uint32_t millis()
{
	auto time_now = std::chrono::steady_clock::now();
	auto diff = time_now - g_time_start;
	return (uint32_t)std::chrono::duration<double, std::milli>(diff).count();
}

void yield()
{
	std::this_thread::yield();
}

byte pgm_read_byte(const void* val)
{
	return *((byte*)val);
}
