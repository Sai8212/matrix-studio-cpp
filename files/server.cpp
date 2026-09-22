// Matrix Studio web host. The browser UI remains in index.html/app.js/styles.css.
#include "httplib.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // Cloud hosts such as Render provide PORT; a command-line port still wins locally.
    const char* hosted_port = std::getenv("PORT");
    const int port = argc > 1 ? std::stoi(argv[1])
                              : (hosted_port ? std::stoi(hosted_port) : 8080);
    const fs::path site_root = fs::current_path();

    httplib::Server server;

    // A C++ endpoint that makes it easy to confirm the site is hosted by this server.
    server.Get("/api/health", [](const httplib::Request&, httplib::Response& response) {
        response.set_content(
            R"({"status":"ok","server":"C++ cpp-httplib","application":"Matrix Studio"})",
            "application/json");
    });

    // Serve index.html, app.js, and styles.css without modifying the supplied UI.
    if (!server.set_mount_point("/", site_root.string().c_str())) {
        std::cerr << "Could not serve folder: " << site_root << '\n';
        return 1;
    }

    server.set_error_handler([](const httplib::Request&, httplib::Response& response) {
        if (response.status == 404) {
            response.set_content("Page not found.", "text/plain");
        }
    });

    std::cout << "Matrix Studio is running at http://localhost:" << port << "\n";
    std::cout << "C++ health check: http://localhost:" << port << "/api/health\n";
    std::cout << "Press Ctrl+C to stop the server.\n";

    // 0.0.0.0 accepts requests from a cloud platform as well as local browsers.
    if (!server.listen("0.0.0.0", port)) {
        std::cerr << "Could not start the server on port " << port
                  << ". Choose another port, for example: MatrixStudioServer 8081\n";
        return 1;
    }
}
