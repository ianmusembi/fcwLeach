import matplotlib.pyplot as plt
import numpy as np
import csv

host_names = []
remaining_energy = []
energy_diff = []

with open('../hostEnergyStats.csv', 'r') as file:
    data = csv.reader(file, delimiter=',')
    for row in data:
        host_names.append(row[0])
        remaining_energy.append(float(row[1]))
        energy_diff.append(float(row[2]))

# First bar graph: energy remaining
plt.figure(1)
plt.bar(host_names, remaining_energy, color='b', width=0.65, label = "Remaining energy")
plt.xlabel('Hosts')
plt.ylabel('Remaining Energy')
plt.title('Remaining Energy of Hosts')
plt.legend()

# Second bar graph: energy drained
plt.figure(2)
plt.bar(host_names, energy_diff, color='b', width=0.65, label = "Energy Lost")
plt.xlabel('Hosts')
plt.ylabel('Energy Lost')
plt.title('Energy Lost From Hosts')
plt.legend()

plt.show()




