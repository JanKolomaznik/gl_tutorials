import numpy as np
import matplotlib.pyplot as plt

# Camera settings
near = 0.1  # Near plane
far = 100.0  # Far plane

# Generate a range of world-space depths (evenly spaced in real space)
world_depths = np.linspace(near, far, 1000)

# Compute the corresponding depth buffer values (simulating OpenGL depth storage)
clip_depths = ( (world_depths - near) / (far - near) )  # Approximate OpenGL depth storage
clip_depths = (clip_depths * (far + near) / (far - near)) / (1 + (clip_depths * (2 * near / (far - near)))) # Perspective divide

# Compute the correct gl_FragCoord.z depth buffer value
depth_buffer_values = ( (world_depths * (far + near)) / (2 * far * near + world_depths * (far - near)) + 1 ) / 2

# Linearized depth calculation (should be a straight line)
linear_depths = world_depths  # Since it's already in world space, this is naturally linear

# Normalize for visualization (to fit [0,1] range)
depth_buffer_values_norm = (depth_buffer_values - depth_buffer_values.min()) / (depth_buffer_values.max() - depth_buffer_values.min())
linear_depths_norm = (linear_depths - linear_depths.min()) / (linear_depths.max() - linear_depths.min())

# Create the plot
plt.figure(figsize=(8, 5))
plt.plot(world_depths, depth_buffer_values_norm, label="Depth Buffer Value (gl_FragCoord.z)", color="red", linestyle="--")
plt.plot(world_depths, linear_depths_norm, label="Linearized Depth", color="blue", linestyle="-")

plt.xlabel("World-Space Depth")
plt.ylabel("Stored Depth Value (Normalized)")
plt.title("Comparison of Depth Buffer vs. Linearized Depth")
plt.legend()
plt.grid()

# Save the figure as a PNG file
plt.savefig("depth_buffer_explanation.png", dpi=300)
plt.show()

