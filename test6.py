from autoaim_alpha.autoaim_alpha.img.tools import *
import cv2
import numpy as np

img = canvas((1000, 1000,3))

cv2.imshow("canvas", img.img)
cv2.waitKey(0)
cv2.destroyAllWindows()