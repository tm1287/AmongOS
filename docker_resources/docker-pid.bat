@ECHO OFF
docker inspect --format "{{ .State.Pid }}" %*