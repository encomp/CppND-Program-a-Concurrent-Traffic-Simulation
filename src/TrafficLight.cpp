#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_messages.empty(); });
    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutex);
    _messages.push_back(std::move(msg));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::RED;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase trafficLightPhase = messageQueue.receive();
        if (TrafficLightPhase::GREEN == trafficLightPhase)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void TrafficLight::setCurrentPhase(TrafficLightPhase trafficLightPhase)
{
    _currentPhase = trafficLightPhase;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(4, 6);
    int sleepFor = distribution(generator);
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto end = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (sleepFor * 1000 <= elapsedTime)
        {
            TrafficLightPhase currentPhase = getCurrentPhase();
            TrafficLightPhase newPhase;
            switch (currentPhase)
            {
            case TrafficLightPhase::GREEN:
                newPhase = TrafficLightPhase::RED;
                break;

            case TrafficLightPhase::RED:
                newPhase = TrafficLightPhase::GREEN;
                break;
            }

            setCurrentPhase(newPhase);
            messageQueue.send(std::move(newPhase));
            sleepFor = distribution(generator);
            start = std::chrono::steady_clock::now();
        }
    }
}