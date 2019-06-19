FROM rust:1.35

# Set the directory we want to run the next commands for
WORKDIR /home/lfesse/www/rust-backend

COPY . .

# build for release
RUN cargo build --release

# Expose ports
EXPOSE 80
CMD ["./target/release/rust-backend"]
