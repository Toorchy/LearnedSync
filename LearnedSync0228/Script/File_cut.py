import os  

def visitDir(path):
    global ordial
    filesize_dict= {}
    li = os.listdir(path)
    for p in li:  
        pathname = os.path.join(path, p)  
        if not os.path.isfile(pathname):   #is a dir
            visitDir(pathname)  
        else:
            byte = os.path.getsize(pathname)
            with open(dest + format(ordial, "04"), "wb+") as fo:  # 新文件路径
                f = open(pathname, "rb")
                fo.write(f.read(int(byte * (0.5 - delta))))
                f.seek(int(byte * (0.5 + delta)))
                fo.write(f.read())
                f.close()
            ordial += 1
	    
# 增量
delta = 0.
# 原文件路径
src = "/home/zhouyu/Dropbox-1/"
# 新文件路径
dest = "/home/zhouyu/test-old/"

ordial = 0
rm = "rm -r " + dest + "*"
os.system(rm)
visitDir(src) 
print(src, dest, delta)