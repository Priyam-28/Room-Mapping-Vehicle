import cv2
import numpy as np
from collections import deque

# Load the image
image = cv2.imread('TestImages/test1.jpg')

# Convert the image to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Apply Canny edge detection
edges = cv2.Canny(gray, 50, 150)

# Find contours in the edged image
contours, _ = cv2.findContours(edges.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Create a binary image representing the maze
maze = np.zeros_like(gray)
cv2.drawContours(maze, contours, -1, 255, 1)

# Identify source and destination points (you may need to customize this)
source = (10, 10)  # Example source point
destination = (200, 200)  # Example destination point

# Define BFS algorithm for maze solving
def bfs_longest_path(maze, source, destination):
    queue = deque([(source, [source])])  # Initialize queue with the source point and path
    longest_path = []  # Initialize longest path
    visited = set([source])  # Initialize set of visited points

    while queue:
        current, path = queue.popleft()  # Get the current point and its path from the queue
        if current == destination:
            # If the current point is the destination, update the longest path if necessary
            if len(path) > len(longest_path):
                longest_path = path
            continue  # Continue exploring other paths
        x, y = current
        # Explore neighboring points
        for dx, dy in [(1, 0), (-1, 0), (0, 1), (0, -1)]:
            next_x, next_y = x + dx, y + dy
            # Check if the next point is within the maze boundaries and is not a wall and is not visited
            if 0 <= next_x < maze.shape[0] and 0 <= next_y < maze.shape[1] and maze[next_x, next_y] != 255 and (next_x, next_y) not in visited:
                # Add the next point and its path to the queue, mark it as visited
                queue.append(((next_x, next_y), path + [(next_x, next_y)]))
                visited.add((next_x, next_y))

    return longest_path

# Apply BFS algorithm to find longest path
longest_path = bfs_longest_path(maze, source, destination)

if longest_path:
    # Draw the longest path on the maze image
    for point in longest_path:
        cv2.circle(image, point, 2, (0, 0, 255), -1)

    # Draw source and destination points
    cv2.circle(image, source, 5, (0, 255, 0), -1)
    cv2.circle(image, destination, 5, (0, 0, 255), -1)

    # Save the image with the longest path
    cv2.imwrite('room_with_longest_path.jpg', image)

    # Display the maze image with the longest path
    cv2.imshow('Longest Path Found', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
else:
    print("No path found from source to destination.")
