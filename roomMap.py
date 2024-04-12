import cv2
import numpy as np

# Load the image
image = cv2.imread('TestImages/test1.jpg')

# Convert the image to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Apply Canny edge detection
edges = cv2.Canny(gray, 50, 150)

# Find contours in the edged image
contours, _ = cv2.findContours(edges.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Draw contours on the original image
cv2.drawContours(image, contours, -1, (0, 255, 0), 2)

# Save the image with contours
cv2.imwrite('room_map_with_contours.jpg', image)

# Display the original image with contours
cv2.imshow('Room Map', image)
cv2.waitKey(0)
cv2.destroyAllWindows()
