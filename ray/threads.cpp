#include "threads.hpp"	
#include "math.hpp"
#include "scene.hpp"

#include <iostream>
#define USE_BGR

using namespace Math;
#define FLOAT2RGB(x) std::round((x) * 255);

std::atomic<bool> ThreadManager::mEndFlag = false;
std::vector<std::atomic<bool>*> ThreadManager::mStartFlags;
std::vector<std::atomic<bool>*> ThreadManager::mDoneFlags;

void TracerThread::TraceMain(ThreadContext context) {
	// switch to windows API
	std::atomic<bool>* start = ThreadManager::GetStartFlag(context.id);
	while (!ThreadManager::ShouldStop()) {
		{
			if (!*start) {
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}

			u8* data = context.imageData;
			u32 startY = context.startY;
			u32 endY = context.endY;
			u32 width = context.width;
			u32 height = context.height;
			Scene* scene = context.scene;
			
			v3 origin(0, 0, 0);

			int samplesPerPixel = 1;
			for (int y = startY; y < endY; ++y) {
				for (int x = 0; x < width; ++x) {
					v3 color;
					v3 sampleColor;
					for (int i = 0; i < samplesPerPixel; ++i) {
						v3 rd = v3::RandUnitCircle(-0.001f, 0.001f);
						sampleColor += scene->ProcessPixel(x, y, width, height, origin, rd);
					}
					color = sampleColor / samplesPerPixel;

					scene->mPaths[x + y * width] += color;

					color = scene->mPaths[x + y * width] / scene->mIterations;

					color = RGB::RGB2SRGB(color);

					//color.Clamp(0, 1);
#ifdef USE_BGR
					data[x * 4 + 0 + y * width * 4] = FLOAT2RGB(color.z); // b
					data[x * 4 + 1 + y * width * 4] = FLOAT2RGB(color.y); // g
					data[x * 4 + 2 + y * width * 4] = FLOAT2RGB(color.x); // r
					data[x * 4 + 3 + y * width * 4] = 255; // a
#else
					data[x * 4 + 0 + y * width * 4] = FLOAT2RGB(color.x); // r
					data[x * 4 + 1 + y * width * 4] = FLOAT2RGB(color.y); // g
					data[x * 4 + 2 + y * width * 4] = FLOAT2RGB(color.z); // b
					data[x * 4 + 3 + y * width * 4] = 255; // a
#endif
				}
			}

			ThreadManager::SetDoneFlag(context.id);
			*start = false;
		}
	}
}

void ThreadManager::SetDoneFlag(int i) {
	*mDoneFlags[i] = true;
}

void ThreadManager::StartThread(int i) {
	*mStartFlags[i] = true;
}

std::atomic<bool>* ThreadManager::GetStartFlag(int i) {
	return mStartFlags[i];
}

void ThreadManager::WaitForThreads() {
	while (true) {
		bool done = true;
		for (auto& f : mDoneFlags) {
			done = done && *f;
		}

		if (done) {
			break;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void ThreadManager::ResumeThreads() {
	for (auto& f : mDoneFlags) {
		*f = false;
	}

	for (auto& f : mStartFlags) {
		*f = true;
	}
}

bool ThreadManager::ShouldStop() {
	return mEndFlag;
}

void ThreadManager::StopThreads() {
	mEndFlag = true;
}

void ThreadManager::CreateThreadPool(const std::vector<ThreadContext>& contextes) {
	int threadCount = std::thread::hardware_concurrency();
	if (contextes.size() < threadCount) {
		std::cout << "INFO not using the full amount of cores" << std::endl;
	}
	threadCount = contextes.size();
	for (int i = 0; i < threadCount; ++i) {
		std::thread* t = new std::thread(TracerThread::TraceMain, contextes[i]);

		mStartFlags.push_back(new std::atomic<bool>(false));
		mDoneFlags.push_back(new std::atomic<bool>(false));
		mEndFlag = false;
	}
}
