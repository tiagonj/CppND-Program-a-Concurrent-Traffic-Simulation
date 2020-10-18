#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}
*/

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // launch cycleThroughPhases function in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    while (true)
    {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<int> distr(4000, 6000);
        int cycleDurationInMs = distr(eng);

        std::this_thread::sleep_for(std::chrono::seconds(cycleDurationInMs));

        TrafficLightPhase newPhase =
            (_currentPhase == TrafficLightPhase::red) ?
                TrafficLightPhase::green : TrafficLightPhase::red;

        _currentPhase = newPhase;
        // FP.4a : MessageQueue<TrafficLightPhase>::send(TrafficLightPhase &&newPhase);

        // Wait for 1ms between two cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
