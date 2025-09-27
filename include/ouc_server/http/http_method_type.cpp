#include <http/http_method_type.hpp>

namespace ouc_server
{
    namespace http
    {
        HttpMethodType str2type(const std::string &str)
        {
            if (str == "GET")
                return HttpMethodType::Get;
            else if (str == "HEAD")
                return HttpMethodType::Head;
            else if (str == "POST")
                return HttpMethodType::Post;
            else if (str == "PUT")
                return HttpMethodType::Put;
            else if (str == "DELETE")
                return HttpMethodType::Delete;
            else if (str == "CONNECT")
                return HttpMethodType::Connect;
            else if (str == "OPTIONS")
                return HttpMethodType::Options;
            else if (str == "TRACE")
                return HttpMethodType::Trace;
            else
                return HttpMethodType::Get;
        }

        std::string type2str(HttpMethodType type)
        {
            switch (type)
            {
            case HttpMethodType::Get:
            default:
                return "GET";
            case HttpMethodType::Head:
                return "HEAD";
            case HttpMethodType::Post:
                return "POST";
            case HttpMethodType::Put:
                return "PUT";
            case HttpMethodType::Delete:
                return "DELETE";
            case HttpMethodType::Connect:
                return "CONNECT";
            case HttpMethodType::Options:
                return "OPTIONS";
            case HttpMethodType::Trace:
                return "TRACE";
            }
        }
    }
}