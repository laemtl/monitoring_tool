FROM rust:1.31

# Set the directory we want to run the next commands for
WORKDIR /home/lfesse/www/rust-backend

COPY . .

RUN cargo install --path .

# Expose ports
EXPOSE 80

CMD ["rust-backend"]
