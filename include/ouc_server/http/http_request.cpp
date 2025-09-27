#include <http/http_request.hpp>

namespace ouc_server
{
    namespace http
    {
        HttpRequest HttpRequest::from_string(const std::string &raw_str)
        {
            std::istringstream iss(raw_str);
            HttpRequest req;

            parse_top_line(req, iss);
            switch (req.method)
            {
            case HttpMethodType::Get:
            default:
                parse_get_request(req, iss);
                break;

            case HttpMethodType::Head:
                break;
            case HttpMethodType::Post:
                break;
            case HttpMethodType::Put:
                break;
            case HttpMethodType::Delete:
                break;
            case HttpMethodType::Connect:
                break;
            case HttpMethodType::Options:
                break;
            case HttpMethodType::Trace:
                break;
            }

            return req;
        }

        void HttpRequest::parse_top_line(HttpRequest &req, std::istringstream &iss)
        {
            std::string line, type_str;
            std::getline(iss, line);

            while ((!line.empty()) && line.back() != '\r')
                line.pop_back();

            std::istringstream req_iss(line);

            req_iss >> type_str >> req.path >> req.version;
            req.method = str2type(type_str);
        }

        void HttpRequest::parse_get_request(HttpRequest &req, std::istringstream &iss)
        {
            std::string line;

            // parse top line
            std::getline(iss, line);
            while ((!line.empty()) && line.back() != '\r')
                line.pop_back();
            std::istringstream req_iss(line);
            std::string type_str;
            req_iss >> type_str >> req.path >> req.version;
            req.method = str2type(type_str);

            // parse headers
            while (std::getline(iss, line) && line != "\r")
            {
                while ((!line.empty()) && line.back() == '\r')
                    line.pop_back();

                auto pos = line.find(": ");
                if (pos == std::string::npos)
                    pos = line.find(":");

                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string val = line.substr(pos + 2);
                    req.headers[key] = val;
                }
            }

            // parse body (not finish)
            std::string body;
            while (std::getline(iss, line))
                body += line + "\n";
            req.body = body;
        }
    }
}