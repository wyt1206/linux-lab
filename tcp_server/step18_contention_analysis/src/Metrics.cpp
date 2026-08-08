#include "Metrics.h"

#include <iostream>

Metrics& Metrics::instance()
{
    static Metrics metrics;

    return metrics;
}

void Metrics::incrementConnections()
{
    connections_++;
}

void Metrics::decrementConnections()
{
    connections_--;
}

void Metrics::incrementRequests()
{
    requests_++;
}

void Metrics::incrementResponses()
{
    responses_++;
}

void Metrics::print()
{
    std::cout << "\n========== Runtime Metrics =========="
              << "\nConnections: " << connections_.load()
              << "\nRequests: " << requests_.load()
              << "\nResponses: " << responses_.load()
              << "\n=====================================\n";
}