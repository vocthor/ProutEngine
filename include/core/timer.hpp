#pragma once

class Timer
{
public:
    void update();

    float deltaTime() const { return deltaTime_; }
    float elapsed() const { return lastFrame_; }

private:
    float deltaTime_ = 0.0f;
    float lastFrame_ = 0.0f;
};
