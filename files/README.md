# Matrix Studio — C++ Web Server

This project presents the existing Matrix Studio website through a C++ HTTP server built with **cpp-httplib**. The original `index.html`, `styles.css`, `app.js`, `Matrix.h`, `Matrix.cpp`, and desktop `main.cpp` are unchanged.

## Run it (Windows)

1. Install a C++ compiler with CMake (Visual Studio Community with the **Desktop development with C++** workload is a straightforward option).
2. Open a terminal in this folder and run:

   ```powershell
   cmake -S . -B build
   cmake --build build --config Release
   .\build\Release\MatrixStudioServer.exe
   ```

   On single-configuration generators, the executable may instead be at `build\MatrixStudioServer.exe`.

3. Open **http://localhost:8080** in a browser.

To use another port, append it when starting the server, for example `MatrixStudioServer.exe 8081`.

## Presentation points

- `server.cpp` is the C++ entry point and uses `httplib::Server`.
- It serves the unmodified HTML, CSS, and JavaScript as static files.
- Visit **http://localhost:8080/api/health** to demonstrate a response created by the C++ server: it returns JSON identifying `C++ cpp-httplib`.
- CMake downloads the pinned `cpp-httplib` v0.18.3 dependency during the first configuration, so no separate library setup is needed.

## Publish permanently with Render

The included `Dockerfile` and `render.yaml` make this project ready for Render.

1. Create a new GitHub repository and upload this entire folder.
2. Sign in at [Render](https://render.com), choose **New +** > **Web Service**, and connect the GitHub repository.
3. Render detects the Docker setup. Keep the service type as **Web Service** and create the service.
4. When the deployment completes, copy the `https://...onrender.com` address that Render displays. That is the public website link to share.

The free tier can sleep after inactivity. For an always-on permanent service, use a paid Render plan or another always-on Docker host.
