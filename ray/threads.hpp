#pragma once

#include "global.hpp"

#include <thread>
#include <vector>
#include <mutex>

class Scene;

struct ThreadContext {
	u32 id;

	s32 startY;
	s32 endY;
	s32 width;
	s32 height;
	u8* imageData;

	Scene* scene;
};

class ThreadManager {
private:
	static std::vector<std::atomic<bool>*> mStartFlags;
	static std::vector<std::atomic<bool>*> mDoneFlags;
	static std::atomic<bool> mEndFlag;

public:
	ThreadManager() = delete;
	ThreadManager(const ThreadManager& other) = delete;

	static void CreateThreadPool(const std::vector<ThreadContext>& contextes);
	static void ResumeThreads();
	static void StopThreads();
	static bool ShouldStop();
	static void WaitForThreads();
	static void StartThread(int i);
	static void SetDoneFlag(int i);
	static std::atomic<bool>* GetStartFlag(int i);
};

class TracerThread {
public:
	static void TraceMain(ThreadContext context);
};