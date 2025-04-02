import cv2

img = cv2.imread('assets/example.jpg', cv2.IMREAD_COLOR)
img = cv2.resize(img, (0, 0), fx=0.5, fy=0.5)
cv2.imwrite('assets/small_example.jpg', img)
cv2.imshow('example image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()