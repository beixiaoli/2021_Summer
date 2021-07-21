## Git：一种分布式版本控制系统，跟踪文本文件的改动
分布式版本控制：允许软件开发者可以共同参与一个软件开发项目  
最初目的：更好地管理Linux内核开发


### 教程
>[廖雪峰Git教程](https://www.liaoxuefeng.com/wiki/896043488029600)  
>[菜鸟Git教程](https://www.runoob.com/git/git-tutorial.html)  



作用：自动记录版本改动，多人协作编辑...  
由来：手工管理→收费版本控制系统（免费：集中式）→Linus用C开发了分布式版本控制系统  
GitHub：为开源项目免费提供Git存储  

>用VSCode代替记事本


### Git简介
1. 创建版本库
```
$ mkdir learngit  //创建一个空目录
$ cd learngit
$ pwd
```

### 时光机穿梭
2. 添加文件到Git仓库
```
$ git init  //把这个目录变成Git可以管理的仓库

$ git add readme.txt  //把文件添加到仓库
$ git commit -m "wrote a readme file" //把文件提交到仓库
```
3. 查看状态
```
$ git status  //查看当前仓库状态：告诉有文件被修改过
$ git diff readme.txt //查看具体状态变化：查看修改内容
```
4. 版本回退
```
$ git log //查看历史记录
//HEAD指向的版本就是当前版本
$ git reset --hard HEAD^  //回退到上一个版本
$ cat readme.txt  //查看内容
$ git reset --hard 1094a  //指定版本号回退
$ git reflog  //确定要回退的版本号
```
5. 工作区和版本库  
工作区→暂存区→版本库  
![工作区和版本库](https://www.liaoxuefeng.com/files/attachments/919020037470528/0)

6. 跟踪修改  
每次修改，如果不用git add到暂存区，那就不会加入到commit中。  

7. 撤销修改
```
$ git checkout -- readme.txt  //丢弃工作区的修改
$ git reset HEAD readme.txt //把暂存区的修改撤销掉（unstage），重新放回工作区（再丢弃工作区的修改）
```
8. 删除文件 `git rm`

### 远程仓库
本地Git仓库和GitHub仓库之间的传输通过SSH加密  
创建SSH key：`$ ssh-keygen -t rsa -C "youremail@example.com"`  
私钥`id_rsa`  
公钥`id_rsa.pub`
私人仓库：收费GitHub、搭建私人Git服务器

9. 添加远程库  
关联远程库：`git remote add origin git@server-name:path/repo-name.git`  
关联一个远程库时必须给远程库指定一个名字，`origin`是默认习惯命名  
推送master分支的所有内容：`git push -u origin master`  
每次本地提交后，推送最新修改：`git push origin master`  
删除远程库（解除本地和远程的绑定关系）：  
```$ git remote -v
$ git remote rm origin
```
10. 从远程库克隆 
创建新仓库   
克隆本地库：`$ git clone git@github.com:michaelliao/gitskills.git`  

### 分支管理：创建属于自己的分支

11. 创建与合并分支  
查看分支：`git branch`  
创建分支：`git branch <name>`  
切换分支：`git checkout <name>`或者`git switch <name>`  
创建+切换分支：`git checkout -b <name>`或者`git switch -c <name>`  
合并某分支到当前分支：`git merge <name>`  
删除分支：`git branch -d <name>`  
分之合并情况：`git log`
普通模式合并：`$ git merge --no-ff -m "merge with no-ff" dev`

12. bug分支  
储藏工作区： `git stash`  
切换到需要修复的分支，创建临时分支  
修复bug  
切换到需要修复的分支，完成合并  
删除临时分支    
恢复工作区：`git stash apply`+`git stash drop`或者`git stash pop`  
查看stash：`git stash list`  
复制特定提交到当前分支：`$ git cherry-pick 4c805e2`  

13. feature分支：添加新功能  
强行删除：`git branch -D <name>`  

14. 多人协作工作模式：  
查看远程库信息：`git remote -v`
首先，可以试图用`git push origin <branch-name>`推送自己的修改；  
如果推送失败，则因为远程分支比你的本地更新，需要先用`git pull`试图合并；  
如果合并有冲突，则解决冲突，并在本地提交；  
没有冲突或者解决掉冲突后，再用`git push origin <branch-name>`推送就能成功！  
  如果`git pull`提示no tracking information，则说明本地分支和远程分支的链接关系没有创建，用命令`git branch --set-upstream-to <branch-name> origin/<branch-name>`。  
  
15. rebase：把本地未push的分叉提交历史整理成直线
