# create image
```
# syntax=docker/dockerfile:1
FROM node:12-alpine
RUN apk add --no-cache python python g++ make
WORKDIR /app
COPY . .
RUN yarn install --production
CMD ["node", "src/index.js"]
```
# build
write a docker file
docker build -t name . 
docker run -dp name

# publish 
docker tag imagename reponame
docker push reponame

# volume
although we can create, update, delete files in container, those changes will be lost when the container removed. Those changes is isolated from all other container even if they are created by same image.  
- make a volume using -v command
```
docker volume todo-db
docker run -dp 3000:3000 -v todo-db:/etc/todos getting-started
```

# network
```
docker network create todo app  
 docker run -d \
     --network todo-app --network-alias mysql \
     -v todo-mysql-data:/var/lib/mysql \
     -e MYSQL_ROOT_PASSWORD=secret \
     -e MYSQL_DATABASE=todos \
     mysql:5.7
```

- a  simple docker compose
```
version: "3.7"

services:
  app:
    image: node:12-alpine
    command: sh -c "yarn install && yarn run dev"
    ports:
      - 3000:3000
    working_dir: /app
    volumes:
      - ./:/app
    environment:
      MYSQL_HOST: mysql
      MYSQL_USER: root
      MYSQL_PASSWORD: secret
      MYSQL_DB: todos

  mysql:
    image: mysql:5.7
    volumes:
      - todo-mysql-data:/var/lib/mysql
    environment:
      ``````````````````````````MYSQL_ROOT_PASSWORD: secret``````````````````````````
      MYSQL_DATABASE: todos

volumes:
  todo-mysql-data:
```

- with comand 
```
docker compose up -d
```