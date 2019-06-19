FROM rust:1.35

# Set the directory we want to run the next commands for
WORKDIR /home/lfesse/www/rust-backend

# install protobuf from source
#RUN apt-get update && \
#    apt-get -y install git unzip build-essential autoconf libtool
#RUN git clone https://github.com/google/protobuf.git && \
#    cd protobuf && \
#    ./autogen.sh && \
#    ./configure && \
#    make && \
#    make install && \
#    ldconfig && \
#    make clean && \
#    cd .. && \
#    rm -r protobuf

RUN apt install protobuf-c-compiler

COPY . .

# build for release
RUN cargo build --release

# Expose ports
EXPOSE 80
CMD ["./target/release/rust-backend"]
