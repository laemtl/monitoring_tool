FROM rust:1.35

# Set the directory we want to run the next commands for
WORKDIR /home/lfesse/www/monitoring_tool/rust-backend

# install protobuf from source
RUN apt-get update && \
    apt-get -y install git unzip build-essential autoconf libtool nano

COPY . .

# Expose ports
EXPOSE 80

CMD /usr/local/bin/shell.sh ; sleep infinity
