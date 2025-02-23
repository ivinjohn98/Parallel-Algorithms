import matplotlib.pyplot as plt
import numpy as np



execution_data = [
    {
        'k pivot sorting':[3.41647, 2.52034, 1.97549, 1.52258, 1.57253, 1.4088, 1.55706, 1.57971, 1.43436, 1.38525,  
            1.33609, 1.32108, 1.25865, 1.26771, 1.27079, 1.24681, 1.33984, 1.21347, 1.24493, 1.21579,  
            1.23341, 1.23359, 1.20062, 1.19749, 1.17736, 1.27103, 1.21733, 1.17351, 1.14481, 1.21135,  
            1.14755, 1.15963, 1.15298, 1.10621, 1.12684, 1.1, 1.20097, 1.12868, 1.21423, 1.12553,  
            1.1343, 1.11194, 1.15114, 1.17674, 1.15918, 1.10312, 1.16087, 1.12817, 1.15977, 1.17176,  
            1.14492, 1.15386, 1.12689, 1.13945, 1.14928, 1.1355, 1.10172, 1.14303, 1.07184, 1.08931,  
            1.09325, 1.17689, 1.16996, 1.11233, 1.13097, 1.08343, 1.09991, 1.10177, 1.0901, 1.11829,  
            1.09736, 1.10132, 1.06874, 1.08736, 1.08306, 1.07078, 1.09757, 1.08781, 1.05706, 1.04355,  
            1.12988, 1.09624, 1.08049, 1.1153, 1.07543, 1.0975, 1.11564, 1.11024, 1.10971, 1.02944,  
            1.06828, 1.0881, 1.05221, 1.05874, 1.08456, 1.12202, 1.048, 1.02721, 1.05222, 1.05309]
    }
]

k_value = range(1, 101)

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(k_value, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Elapsed Time vs k", fontsize=16)
    plt.xlabel("k value", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"execution_plot_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
