
class T:
    def __init__(self,x):
        self.c = x
        

b = T(10)
c = T(20)
d = T(20)
a = [b,c,d]
mm = max(a,key=lambda x:x.c)
print(mm.c)