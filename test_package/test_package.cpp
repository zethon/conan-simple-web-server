#include "client_http.hpp"
#include "server_http.hpp"

// Added for the default_resource example
#include <algorithm>
#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

using namespace std;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

int main() {
    // HTTP-server at port 8080 using 1 thread
    // Unless you do more heavy non-threaded processing in the resources,
    // 1 thread is usually faster than several threads
    HttpServer server;
    server.config.port = 8080;

    // Add resources using path-regex and method-string, and an anonymous function
    // POST-example for the path /string, responds the posted string
    server.resource["^/string$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        // Retrieve string:
        auto content = request->content.string();
        // request->content.string() is a convenience function for:
        // stringstream ss;
        // ss << request->content.rdbuf();
        // auto content=ss.str();

        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.length() << "\r\n\r\n"
                  << content;


        // Alternatively, use one of the convenience functions, for instance:
        // response->write(content);
    };

    // POST-example for the path /json, responds firstName+" "+lastName from the posted json
    // Responds with an appropriate error message if the posted json is not valid, or if firstName or lastName is missing
    // Example posted json:
    // {
    //   "firstName": "John",
    //   "lastName": "Smith",
    //   "age": 25
    // }
    server.resource["^/json$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {



        // Alternatively, using a convenience function:
        // try {
        //     ptree pt;
        //     read_json(request->content, pt);

        //     auto name=pt.get<string>("firstName")+" "+pt.get<string>("lastName");
        //     response->write(name);
        // }
        // catch(const exception &e) {
        //     response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        // }
    };

    // GET-example for the path /info
    // Responds with request-information
    server.resource["^/info$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        stringstream stream;
        stream << "<h1>Request from " << request->remote_endpoint_address() << ":" << request->remote_endpoint_port() << "</h1>";

        stream << request->method << " " << request->path << " HTTP/" << request->http_version;

        stream << "<h2>Query Fields</h2>";
        auto query_fields = request->parse_query_string();
        for(auto &field : query_fields)
            stream << field.first << ": " << field.second << "<br>";

        stream << "<h2>Header Fields</h2>";
        for(auto &field : request->header)
            stream << field.first << ": " << field.second << "<br>";

        response->write(stream);
    };

    // GET-example for the path /match/[number], responds with the matched string in path (number)
    // For instance a request GET /match/123 will receive: 123
    server.resource["^/match/([0-9]+)$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        response->write(request->path_match[1].str());
    };

    // GET-example simulating heavy work in a separate thread
    server.resource["^/work$"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> /*request*/) {
        thread work_thread([response] {
            this_thread::sleep_for(chrono::seconds(5));
            response->write("Work done");
        });
        work_thread.detach();
    };

    // Default GET-example. If no other matches, this anonymous function will be called.
    // Will respond with content in the web/-directory, and its subdirectories.
    // Default file: index.html
    // Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
    server.default_resource["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

    };

    server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
        // Handle errors here
        // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
    };

    thread server_thread([&server]() {
        // Start server
        server.start();
    });

    // Wait for server to start so that the client can connect
    this_thread::sleep_for(chrono::seconds(1));

    // Client examples
    HttpClient client("localhost:8080");

    string json_string = "{\"firstName\": \"John\",\"lastName\": \"Smith\",\"age\": 25}";

    // Synchronous request examples
    try {
        auto r1 = client.request("GET", "/match/123");
        cout << r1->content.rdbuf() << endl; // Alternatively, use the convenience function r1->content.string()

        auto r2 = client.request("POST", "/string", json_string);
        cout << r2->content.rdbuf() << endl;
    }
    catch(const SimpleWeb::system_error &e) {
        cerr << "Client request error: " << e.what() << endl;
    }

    // Asynchronous request example
    client.request("POST", "/json", json_string, [](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
        if(!ec)
            cout << response->content.rdbuf() << endl;
    });
    client.io_service->run();

    server_thread.join();
}
