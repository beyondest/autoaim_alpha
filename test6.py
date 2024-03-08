import cv2
path = './../roi_tmp.jpg'
cv2.imshow('image', cv2.imread(path))
cv2.waitKey(0)
cv2.destroyAllWindows()