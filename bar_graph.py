import matplotlib.pyplot as plt

# Data bar chat
algorithms = ['FIFO', 'VOQ Optimal', 'iSLIP']
service_time = [10, 6, 9]

plt.figure()
plt.bar(algorithms, service_time)

plt.title("Total Service Time Comparison")
plt.xlabel("Scheduling Algorithm")
plt.ylabel("Time Slots")

plt.show()
