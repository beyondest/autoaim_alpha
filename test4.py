import numpy as np

a = np.array([[2,1],
              [3,4],
              [5,6],
              [7,8]])
print(a.shape)
print(np.mean(a, axis=0))