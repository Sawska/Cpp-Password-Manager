FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    libssl-dev \
    libargon2-dev \
    libwxbase3.0-dev \
    libwxgtk3.0-gtk3-dev \
    libasio-dev \
    nlohmann-json3-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN rm -rf build

# Clone Crow repo shallowly to avoid big download
RUN git clone --depth 1 https://github.com/CrowCpp/Crow.git /tmp/crow

# Copy only the 'include/crow' directory to system include path
RUN mkdir -p /usr/local/include && \
    cp -r /tmp/crow/include/crow /usr/local/include/

# Clean up temporary clone
RUN rm -rf /tmp/crow

# Clone jwt-cpp repo shallowly
RUN git clone --depth 1 https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp

# Copy the 'include/jwt-cpp' directory to system include path
RUN mkdir -p /usr/local/include && \
    cp -r /tmp/jwt-cpp/include/jwt-cpp /usr/local/include/

# Clean up
RUN rm -rf /tmp/jwt-cpp

# Download cpp-httplib single header file to /usr/local/include
RUN mkdir -p /usr/local/include/httplib
RUN curl -L https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o /usr/local/include/httplib.h

RUN mkdir -p /usr/local/include/picojson
RUN curl -L https://raw.githubusercontent.com/kazuho/picojson/master/picojson.h -o /usr/local/include/picojson/picojson.h



# Configure the build
RUN cmake -B build -S .

# Build the project
RUN cmake --build build

CMD ["ctest", "--output-on-failure", "--test-dir", "build"]
