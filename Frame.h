#pragma once

#ifndef FRAME_H
#define FRAME_H
#include <iostream>

#include "AttachedProcess.h" // I think need to replace into pages but for now since di ko alam paano pages imma do this muna

class Frame {
private:
	int frameNum;
	int memPerFrame;

public:
	Frame(int frameNum, int memPerFrame)
		: frameNum(frameNum), memPerFrame(memPerFrame) {}

	int getFrameNum() const { return frameNum; }

	int getMemPerFrame() const { return memPerFrame; }

};

#endif

