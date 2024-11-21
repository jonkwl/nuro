#pragma once
class MotionBlurPass
{
public:
	static void setup();
	static unsigned int render(unsigned int hdrInput, unsigned int depthInput);
};

