#ifndef INCLUDE_OUC_SERVER_HTTP_RESPONSE
#define INCLUDE_OUC_SERVER_HTTP_RESPONSE

#include <string>
#include <unordered_map>

#include <http/http_method_type.hpp>

namespace ouc_server
{
    namespace http
    {
        class HttpResponseBuilder;

        struct HttpResponse
        {
            std::string version = "HTTP/1.1";
            int stus_code = 200;
            std::string stus_msg = "OK";

            std::unordered_map<std::string, std::string> headers;
            std::string body;

            std::string to_string() const;

            static HttpResponseBuilder create();
        };

        class HttpResponseBuilder
        {
        private:
            HttpResponse res;

        public:
            HttpResponseBuilder() = default;
            ~HttpResponseBuilder() = default;

            HttpResponse build() { return res; }

        public:
            HttpResponseBuilder &version(const std::string &p_version)
            {
                res.version = p_version;
                return *this;
            }

            HttpResponseBuilder &stus_code(int p_stus_code)
            {
                res.stus_code = p_stus_code;
                return *this;
            }

            HttpResponseBuilder &stus_msg(const std::string &p_stus_msg)
            {
                res.stus_msg = p_stus_msg;
                return *this;
            }

            HttpResponseBuilder &header(const std::pair<const std::string &, const std::string &> p_header)
            {
                res.headers.insert(p_header);
                return *this;
            }

            HttpResponseBuilder &header(const std::string &p_key, const std::string &p_value)
            {
                return header({p_key, p_value});
            }

            HttpResponseBuilder &body(const std::string &p_body)
            {
                res.body = p_body;
                return *this;
            }
        };
    }
}

#endif // INCLUDE_OUC_SERVER_HTTP_RESPONSE