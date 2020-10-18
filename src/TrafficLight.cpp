#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lck(_mtx);
    _cond.wait(lck, [this](){ return !this->_queue.empty(); });

    T t = std::move(*_queue.begin());
    _queue.pop_front();
    return t; // Expecting RVO to occur here
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lck(_mtx);
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase newPhase = _messageQueue.receive();
        if (newPhase == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    std::lock_guard<std::mutex> lck(_mutex);
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
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int> uniformDist(4000, 6000);
    std::chrono::time_point<std::chrono::system_clock> lastPhaseChange;

    // Initialise duration of first phase (red)
    int phaseDurationInMs = uniformDist(eng);

    // init stop watch
    lastPhaseChange = std::chrono::system_clock::now();
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastPhaseChange).count();
        if (timeSinceLastUpdate >= phaseDurationInMs)
        {
            std::unique_lock<std::mutex> lck(_mutex, std::defer_lock);

            TrafficLightPhase newPhase =
                (_currentPhase == TrafficLightPhase::red) ?
                    TrafficLightPhase::green : TrafficLightPhase::red;

            lck.lock();
            _currentPhase = newPhase; // Toggle phase
            lck.unlock();

            _messageQueue.send(std::move(newPhase));

            // Pick new phase duration
            phaseDurationInMs = uniformDist(eng);

            // reset stop watch for next cycle
            lastPhaseChange = std::chrono::system_clock::now();
        }
    }
}
