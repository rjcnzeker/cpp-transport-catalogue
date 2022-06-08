#include <istream>
#include <sstream>
#include <map>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

class InputReader {
public:
    explicit InputReader(int number_requests) : request_queue_() {
        for (int i = 0; i < number_requests; ++i) {
            std::string line;
            std::getline(std::cin, line);
            Request request;
            request.request_ = line;
            if (line[0] == 'B') {
                line = line.substr(line.find_first_not_of(' ', 3));

                request_queue_[RequestType::BUS].push_back(line);
            }
            if (line[0] == 'S') {
                line = line.substr(line.find_first_not_of(' ', 4));
                request_queue_[RequestType::STOP].push_back(line);
            }
        }
    }

    void RequestProcessing() {

    }


private:
    enum class RequestType {
        BUS,
        STOP,
    };

    struct Request {
        Request() = default;

        Request(std::string &request) : request_(std::move(request)) {
        }

        std::string request_;
    };

    std::map<RequestType, std::vector<Request>> request_queue_;

};