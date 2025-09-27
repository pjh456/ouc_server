#ifndef INCLUDE_OUC_SERVER_HTTP_METHOD_TYPE
#define INCLUDE_OUC_SERVER_HTTP_METHOD_TYPE

#include <string>

namespace ouc_server
{
    namespace http
    {
        enum class HttpMethodType
        {
            Get,
            Head,
            Post,
            Put,
            Delete,
            Connect,
            Options,
            Trace
        };

        HttpMethodType str2type(const std::string &);

        std::string type2str(HttpMethodType);
    }
}

#endif // INCLUDE_OUC_SERVER_HTTP_METHOD_TYPE