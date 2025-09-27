#include <http/http_response.hpp>

#include <sstream>

namespace ouc_server
{
    namespace http
    {
        std::string HttpResponse::to_string() const
        {
            std::ostringstream oss;
            oss << version << " "
                << stus_code << " "
                << stus_msg << "\r\n";

            for (auto &[k, v] : headers)
                oss << k << ": " << v << "\r\n";

            oss << "\r\n"
                << body;

            return oss.str();
        }

        HttpResponseBuilder HttpResponse::create() { return {}; }
    }
}
