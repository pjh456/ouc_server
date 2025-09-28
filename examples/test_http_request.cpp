#include <http/http_response.hpp>

#include <string>
#include <iostream>

int main()
{
    std::string body = "Hello HTTP!";

    auto builder = ouc_server::http::HttpResponse::create();
    auto res =
        builder.body(body)
            .header("Content-Length", std::to_string(body.size()))
            .header("Content-Type", "text/plain")
            .build();

    printf("%s\n", res.to_string().c_str());

    return 0;
}