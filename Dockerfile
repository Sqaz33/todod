FROM alpine:latest AS builder

RUN apk add --no-cache \
    gcc \
    g++ \
    make \
    cmake \
    boost-dev \
    boost-system \
    boost-thread \
    nlohmann-json \
    sqlite-dev \
    flex-dev \
    bison 

WORKDIR /app 
COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=ReleaseWithLTO && \
    cmake --build build --config ReleaseWithLTO

FROM alpine:latest

RUN apk add --no-cache \
    boost-system \
    boost-thread \
    sqlite \
    sqlite-dev 

WORKDIR /app 

COPY --from=builder /app/build /app/build

RUN mkdir -p /data
VOLUME ["/data"]

EXPOSE 6666
ENV DB_PATH=/data/todos.db

CMD ["./build/todod", "0.0.0.0", "6666"]
