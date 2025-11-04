#include "buffered_channel.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <functional>

void sender_func(int id, BufferedChannel<std::string>& channel, int num_messages) {
    try {
        for (int i = 0; i < num_messages; ++i) {
            std::string message = "Message " + std::to_string(i) + " from sender " + std::to_string(id);
            std::cout << "[SENDER " << id << "] Sending: " << message << "\n";
            channel.Send(message);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "[SENDER " << id << "] Finished.\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "[SENDER " << id << "] EXCEPTION: " << e.what() << '\n';
    }
}

void receiver_func(int id, BufferedChannel<std::string>& channel) {
    while (true) {
        std::cout << "[RECEIVER " << id << "] Waiting for data...\n";
        auto [message, ok] = channel.Recv();

        if (!ok) {
            std::cout << "[RECEIVER " << id << "] Channel closed. Exiting.\n";
            break;
        }

        std::cout << "[RECEIVER " << id << "] Received: " << message << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    BufferedChannel<std::string> channel(5);

    std::vector<std::thread> senders;
    std::vector<std::thread> receivers;

    for (int i = 0; i < 2; ++i) {
        senders.emplace_back(sender_func, i, std::ref(channel), 7);
    }

    for (int i = 0; i < 3; ++i) {
        receivers.emplace_back(receiver_func, i, std::ref(channel));
    }

    std::cout << "--- Main: Waiting for senders to finish... ---\n";
    for (auto& t : senders) {
        t.join();
    }

    std::cout << "--- Main: All senders finished. Closing channel. ---\n";
    channel.Close();

    std::cout << "--- Main: Waiting for receivers to finish... ---\n";
    for (auto& t : receivers) {
        t.join();
    }

    std::cout << "--- Main: Test complete. ---\n";
    
    std::cout << "\n--- Main: Testing Send() on closed channel... ---\n";
    try {
        channel.Send("This should fail");
    } catch(const std::runtime_error& e) {
        std::cerr << "Caught expected exception: " << e.what() << '\n';
    }

    return 0;
}