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