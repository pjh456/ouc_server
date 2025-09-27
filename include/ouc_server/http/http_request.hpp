#ifndef INCLUDE_OUC_SERVER_HTTP_REQUEST
#define INCLUDE_OUC_SERVER_HTTP_REQUEST

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

        struct HttpRequest
        {
            HttpMethodType method;
        };
    }
}

#endif