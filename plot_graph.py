import matplotlib.pyplot as plt
import numpy as np

# Data for Distributed Count Sort
datasets = ['small', 'medium', 'large']
std_runtime = [0.13, 0.35, 0.41]  # in seconds
std_memory = [248064, 265216, 403968]  # in KB

# Data for Distributed Merge Sort
dist_runtime = [0, 0, 0.03]  # in seconds
dist_memory = [1664, 4184, 25128]  # in KB

# Plotting configurations
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# Plotting Runtime Comparison as a Line Graph
x = np.arange(len(datasets))

ax1.plot(x, std_runtime, marker='o', label='Distributed Count Sort', color='skyblue', linestyle='-')
ax1.plot(x, dist_runtime, marker='o', label='Distributed Merge Sort', color='salmon', linestyle='-')
ax1.set_xlabel("Dataset Size")
ax1.set_ylabel("Total Runtime (seconds)")
ax1.set_title("Runtime Comparison")
ax1.set_xticks(x)
ax1.set_xticklabels(datasets)
ax1.legend(loc='upper left', bbox_to_anchor=(0, 1), fontsize='x-small')  # Legend at the top left

# Plotting Memory Usage Comparison as a Bar Graph
width = 0.35  # bar width
ax2.bar(x - width/2, std_memory, width, label='Distributed Count Sort', color='skyblue')
ax2.bar(x + width/2, dist_memory, width, label='Distributed Merge Sort', color='salmon')
ax2.set_xlabel("Dataset Size")
ax2.set_ylabel("Max Memory Usage (KB)")
ax2.set_title("Memory Usage Comparison")
ax2.set_xticks(x)
ax2.set_xticklabels(datasets)
ax2.legend(loc='upper left', bbox_to_anchor=(0, 1), fontsize='x-small')  # Legend at the top left

plt.tight_layout()
plt.savefig("analysis.png")
plt.show()
