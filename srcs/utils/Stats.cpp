#include "utils/Stats.hpp"
#include <algorithm>
#include <iomanip>

std::unordered_map<std::string, struct sStat> Stats::stats = {};

Stats::Stats() {
}

Stats::~Stats() {
}

void    Stats::printStats() {
    for (auto const &stat : stats) {
        std::cout << stat.first << ":" << std::endl;

        std::cout << "\tfunction called " << stat.second.nbCalls << " times" << std::endl;
        std::cout << "\ttotal exec time " << std::fixed << std::setprecision(8) \
        << stat.second.totalExecTime.count() << "s" << std::endl;
        std::cout << "\taverage " << std::fixed << std::setprecision(8) << \
        stat.second.avgExecTime.count() << "s" << std::endl;

        if (stat.second.minExecTime != std::chrono::duration<double>::max()) {
            std::cout << "\tmin " << std::fixed << std::setprecision(8) << \
            stat.second.minExecTime.count() << "s" << std::endl;
        }
        if (stat.second.maxExecTime != std::chrono::duration<double>::min()) {
            std::cout << "\tmax " << std::fixed << std::setprecision(8) << \
            stat.second.maxExecTime.count() << "s" << std::endl;
        }
    }
}

std::chrono::high_resolution_clock::time_point  Stats::startStats(std::string name) {
    if (Stats::stats.find(name) == Stats::stats.end()) {
        struct sStat stats;
        stats.nbCalls = 0;
        stats.totalExecTime = std::chrono::duration<double>(0);
        stats.avgExecTime = std::chrono::duration<double>(0);
        stats.minExecTime = std::chrono::duration<double>::max();
        stats.maxExecTime = std::chrono::duration<double>::min();
        Stats::stats[name] = stats;
    }
    ++(Stats::stats[name].nbCalls);
    return std::chrono::high_resolution_clock::now();
}

void	Stats::endStats(std::string name, std::chrono::high_resolution_clock::time_point \
startExecTime) {
    if (Stats::stats.find(name) == Stats::stats.end()
    || startExecTime == std::chrono::high_resolution_clock::time_point::min())
        return;
    std::chrono::duration<double> execTime = std::chrono::duration_cast<std::chrono::duration<double> >(
        std::chrono::high_resolution_clock::now() - startExecTime);
    Stats::stats[name].totalExecTime += execTime;
    Stats::stats[name].avgExecTime = Stats::stats[name].totalExecTime / Stats::stats[name].nbCalls;
    Stats::stats[name].minExecTime = std::min(Stats::stats[name].minExecTime, execTime);
    Stats::stats[name].maxExecTime = std::max(Stats::stats[name].maxExecTime, execTime);
}
