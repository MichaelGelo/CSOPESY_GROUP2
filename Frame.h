#pragma once

#ifndef FRAME_H
#define FRAME_H
#include <iostream>
#include "Page.h"

#include "AttachedProcess.h" // I think need to replace into pages but for now since di ko alam paano pages imma do this muna

class Frame {
private:
	int frameNum;
	int memPerFrame;
	bool allocatable;
	std::shared_ptr<Page> currentPage;

public:
	Frame(int frameNum, int memPerFrame, bool isAllocatable)
		: frameNum(frameNum), memPerFrame(memPerFrame), allocatable(isAllocatable), currentPage(nullptr) {}

	int getFrameNum() const { return frameNum; }
	int getMemPerFrame() const { return memPerFrame; }
	bool isAllocatable() const { return allocatable; }

	void setIsAllocatable(bool allocatable) { allocatable = allocatable; }
	void setCurrentPage(std::shared_ptr<Page> page) { currentPage = page; }

	std::shared_ptr<Page> getCurrentPage() const { return currentPage; }

};

//
#endif

