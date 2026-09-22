# Build the cpp-httplib web server, then run it in a small production image.
FROM debian:bookworm-slim AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --config Release -j2

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=build /app/MatrixStudioServer /app/MatrixStudioServer
COPY index.html app.js styles.css /app/

EXPOSE 8080
CMD ["/app/MatrixStudioServer"]
