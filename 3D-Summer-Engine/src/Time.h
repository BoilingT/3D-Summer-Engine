#pragma once
class Time
{
private:
	double deltaTime = 0;
	double fixedDeltaTime = 0;
	double time = 0;

	static Time& Instance()
	{
		static Time instance;
		return instance;
	}

	static double calculateDeltaTime(double newTime)
	{
		double lastPcTime = Instance().time;
		double dT = newTime - lastPcTime;
		Instance().deltaTime = dT;
		return dT;
	}

public:
	static double DeltaTime()
	{
		return Instance().deltaTime;
	}

	static double FixedDeltaTime(double value)
	{
		return Instance().fixedDeltaTime = value;
	}

	static double FixedDeltaTime()
	{
		return Instance().fixedDeltaTime;
	}

	static double Set(double newTime)
	{
		calculateDeltaTime(newTime);
		return Instance().time = newTime;
	}

	static double Get()
	{
		return Instance().time;
	}
};

