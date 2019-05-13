# Create the container from the alpine linux image
FROM node:12-alpine

#PM2 will be used as PID 1 process
RUN npm install pm2 -g
RUN pm2 install pm2-auto-pull

# Set the directory we want to run the next commands for
WORKDIR /var/www

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
# where available (npm@5+)
COPY package*.json ./
COPY pm2.json .

# Install the dependencies, can be commented out if you're running the same node version
RUN npm install
# If you are building your code for production
# RUN npm ci --only=production

# Copy our source code into the container
COPY . .

# run webpack and the vue-loader
RUN npm run build

# Expose ports
EXPOSE 3000

# Actual script to start can be overridden from `docker run`
CMD ["pm2", "start", "pm2.json", "--no-daemon"]
