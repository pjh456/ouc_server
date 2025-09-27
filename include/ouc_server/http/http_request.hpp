#ifndef INCLUDE_OUC_SERVER_HTTP_REQUEST
#define INCLUDE_OUC_SERVER_HTTP_REQUEST

#include <string>
#include <unordered_map>
#include <sstream>

#include <http/http_method_type.hpp>

namespace ouc_server
{
    namespace http
    {

        struct HttpRequest
        {
            HttpMethodType method = HttpMethodType::Get;
            std::string path;
            std::string version;

            std::unordered_map<std::string, std::string> headers;
            std::string body;

            static HttpRequest from_string(const std::string &);

        private:
            static void parse_top_line(HttpRequest &, std::istringstream &);

            static void parse_get_request(HttpRequest &, std::istringstream &);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_HTTP_REQUEST