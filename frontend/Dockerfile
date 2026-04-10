# Build stage.
FROM node:20-alpine AS build

WORKDIR /app
COPY frontend/package.json frontend/package-lock.json ./
RUN npm ci

COPY frontend/ ./
RUN npm run build

# Serve stage.
FROM nginx:alpine

COPY --from=build /app/build /usr/share/nginx/html
COPY frontend/nginx.conf /etc/nginx/conf.d/default.conf

# Run nginx as non-root where possible.
RUN chown -R nginx:nginx /usr/share/nginx/html /var/cache/nginx

EXPOSE 80
