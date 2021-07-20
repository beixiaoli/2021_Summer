### Git：一种分布式版本控制系统，跟踪文本文件的改动
分布式版本控制：允许软件开发者可以共同参与一个软件开发项目  
最初目的：更好地管理Linux内核开发


#### 教程
>[廖雪峰Git教程](https://www.liaoxuefeng.com/wiki/896043488029600)  
>[菜鸟Git教程](https://www.runoob.com/git/git-tutorial.html)  



作用：自动记录版本改动，多人协作编辑...  
由来：手工管理→收费版本控制系统（免费：集中式）→Linus用C开发了分布式版本控制系统  
GitHub：为开源项目免费提供Git存储  

>用VSCode代替记事本


```
$ mkdir learngit  //创建一个空目录
$ cd learngit
$ pwd

$ git init  //把这个目录变成Git可以管理的仓库

$ git add readme.txt  //把文件添加到仓库
$ git commit -m "wrote a readme file" //把文件提交到仓库

$ git status  //查看当前仓库状态：告诉有文件被修改过
$ git diff readme.txt //查看具体状态变化：查看修改内容

$ git log //查看历史记录
//HEAD指向的版本就是当前版本
$ git reset --hard HEAD^  //回退到上一个版本
$ cat readme.txt  //查看内容
$ git reset --hard 1094a  //指定版本号回退
$ git reflog  //确定要回退的版本号

//工作区→暂存区→版本库

//跟踪修改

$ git checkout -- readme.txt  //丢弃工作区的修改
$ git reset HEAD readme.txt //把暂存区的修改撤销掉（unstage），重新放回工作区




```

