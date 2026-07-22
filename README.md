# linux-lab

##### 进入Docker Linux环境：

```Shell
docker run -it linux-lab
docker start -ai linux-lab
env | grep -i proxy
unset http_proxyapt install -y g++ cmake make git
```

##### 在另一个terminal里进入docker Linux环境：

```Shell
docker exec -it linux-lab bash
```
