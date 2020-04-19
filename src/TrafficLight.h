#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    std::deque<T> _messages;
};

enum TrafficLightPhase
{
    RED = 0,
    GREEN = 1
};

class TrafficLight : public TrafficObject
{

public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    void setCurrentPhase(TrafficLightPhase trafficLightPhase);
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    MessageQueue<TrafficLightPhase> messageQueue;
    TrafficLightPhase _currentPhase;
    void cycleThroughPhases();
};

#endif