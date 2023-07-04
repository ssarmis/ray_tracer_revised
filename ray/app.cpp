#define USING_UI

#define _CRT_SECURE_NO_WARNINGS

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "threads.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "global.hpp"
#include "math.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "object.hpp"

#define FLOAT2RGB(x) std::round((x) * 255);

constexpr int width = 800;
constexpr int height = 800;

using namespace Math;

v3 lightPosition = v3(0, 0, 0);

u32 Random::state;

int main() {
    Random::state = time(NULL);
#ifdef USING_UI
    SDL_Init(SDL_INIT_EVENTS);

    SDL_Window* window = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);

    bool done = false;
    SDL_Event event;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
#endif
    u8* data = new u8[width* height* 4];

    for (int i = 0; i < width * height * 4; ++i) {
        data[i] = 0;
    }

    Scene scene;
    scene.mPaths = new v3[width * height];
    scene.mIterations = 0;

    Texture kittyTexture("chess.png");

    Sphere* s0 = new Sphere(v3(0, -0.7f, 5), 1);
    s0->SetColor(v3(1, 0, 1));
    s0->SetRoughness(0.3f);
    s0->SetAlbedoTexture(&kittyTexture);
    scene.AddObject(s0);
    
    Sphere* s1 = new Sphere(v3(2, 0, 12), 6);
    s1->SetColor(v3(1, 1, 1));
    s1->SetRoughness(1);
    s1->SetEmission(v3(1, 1, 1));
    scene.AddObject(s1);

    Sphere* s2 = new Sphere(v3(-3, -1, 5), 0.8f);
    s2->SetColor(v3(1, 1, 1));
    s2->SetEmission(v3(1, 1, 1));
    s2->SetRoughness(1);
    scene.AddObject(s2);

    Sphere* s3 = new Sphere(v3(-3, 0.5f, 5), 0.9f);
    s3->SetColor(v3(1, 0.3f, 0.4f));
    s3->SetRoughness(0.03f);
    scene.AddObject(s3);

    Sphere* s4 = new Sphere(v3(1.4f, -1, 4), 1.1f);
    s4->SetColor(v3(1, 0.5f, 0.1f));
    s4->SetRoughness(0.7f);
    scene.AddObject(s4);

    Plane* p0 = new Plane(v3(0, -1, 0), v3(0, 1, 0).Normalized());
    p0->SetColor(v3(1, 0.5f, 0.7f));
    p0->SetRoughness(0.2f);
    scene.AddObject(p0);

    Plane* p1 = new Plane(v3(-3, 0, 0), v3(1, 0, 0).Normalized());
    p1->SetColor(v3(0, 1, 0));
    p1->SetRoughness(0.9f);
    scene.AddObject(p1);

    Plane* p2 = new Plane(v3(3, 0, 0), v3(-1, 0, 0).Normalized());
    p2->SetColor(v3(0.5f, 0.2f, 0.8f));
    p2->SetRoughness(0.9f);
    scene.AddObject(p2);

    Plane* p3 = new Plane(v3(0, 0, 8), v3(0, 0, -1).Normalized());
    p3->SetColor(v3(1, 0, 1));
    p3->SetRoughness(0.7f);
    scene.AddObject(p3);

    Plane* p4 = new Plane(v3(0, 10, 8), v3(0, -1, 0).Normalized());
    p4->SetColor(v3(1, 1, 1));
    p4->SetRoughness(0.7f);
    scene.AddObject(p4);

    Plane* p5 = new Plane(v3(0, 0, 0), v3(0, 0, 1).Normalized());
    p5->SetColor(v3(1, 1, 1));
    p5->SetRoughness(0.7f);
    scene.AddObject(p5);


    int threadCount = std::thread::hardware_concurrency();
    //int threadCount = 1;

    std::vector<ThreadContext> contextes;
    int chunkSize = height / threadCount;
    for (int i = 0; i < threadCount; ++i) {
        ThreadContext c = {};
        c.id = i;
        c.startY = chunkSize * i;
        c.endY = c.startY + chunkSize;
        c.width = width;
        c.height = height;
        c.scene = &scene;
        c.imageData = data;
        contextes.push_back(c);
    }
    ThreadManager::CreateThreadPool(contextes);

    //r32 time = 0;
#ifdef USING_UI
    while (!done) {
        srand(time(NULL));
        SDL_PollEvent(&event);
        
        switch (event.type) {
            case SDL_QUIT: {
                done = true;
                break;
            }
        }
#endif
#ifndef USING_UI
        int iterations = 1000;
        for (int i = 0; i < iterations; ++i) {
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
#endif
            ++scene.mIterations;
            ThreadManager::ResumeThreads();
            ThreadManager::WaitForThreads();
#ifndef USING_UI
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::cout << "\rProgress " << (i / 1000.0) * 100.0 << "%          ";
        }
        //std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "[ms]" << std::endl;
#endif

#ifdef USING_UI

        SDL_UpdateTexture(texture, NULL, data, width * 4);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
#else
    stbi_write_png("splash_art.png", width, height, 4, data, width * 4);
#endif

    ThreadManager::StopThreads();

#ifdef USING_UI
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();
#endif
    return 0;
}


