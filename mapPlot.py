import matplotlib.pyplot as plt
import numpy as np

# Function to convert polar coordinates to Cartesian coordinates
def polar_to_cartesian(distance, angle):
    x = distance * np.cos(np.radians(angle))
    y = distance * np.sin(np.radians(angle))
    return x, y

# Example distance measurements (replace with actual sensor readings)
distances = [100, 80, 120]  # Example distances in cm
angles = [45, 90, 135]      # Example angles in degrees

# Convert polar coordinates to Cartesian coordinates
obstacles = [polar_to_cartesian(dist, angle) for dist, angle in zip(distances, angles)]

# Plotting
plt.figure(figsize=(8, 6))
plt.scatter(*zip(*obstacles), color='red', marker='o', label='Obstacles')
plt.xlabel('X')
plt.ylabel('Y')
plt.title('2D Map of Environment')
plt.grid(True)
plt.legend()
plt.axis('equal')
plt.show()
