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
		if (dT < 0) dT *= -1;
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
		if (newTime < 0) return 0;
		calculateDeltaTime(newTime);
		return Instance().time = newTime;
	}

	static double Get()
	{
		return Instance().time;
	}
};

