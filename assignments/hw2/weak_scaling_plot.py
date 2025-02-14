import matplotlib.pyplot as plt
import numpy as np

execution_data = [
    {'Impl reduce': [2.548e-06, 0.000567919, 0.000273058, 0.00010007, 0.000151693, 4.1479e-05, 0.00017329, 0.000219935], 
     'Std reduce': [1.6158e-05, 4.4912e-05, 9.5503e-05, 0.00056587, 9.6469e-05, 0.000126214, 0.000109791, 0.000403203]},
    
    {'Impl gather': [1.8565e-05, 4.6181e-05, 0.000119566, 0.000166296, 0.000182917, 0.000237041, 0.000227277, 0.000297873], 
     'Std gather': [1.7953e-05, 4.3919e-05, 0.000580413, 0.000657649, 0.00011679, 0.000147994, 0.00011808, 0.000246649]},
    
    {'Impl broadcast O(P)': [2.551e-06, 2.8324e-05, 3.2668e-05, 1.6498e-05, 4.4546e-05, 7.5091e-05, 0.000105415, 0.000104787], 
     'Std broadcast': [6.648e-06, 4.2083e-05, 4.5131e-05, 5.0432e-05, 4.7057e-05, 5.97e-05, 7.5071e-05, 7.6802e-05], 
     'Impl broadcast O(log(P))': [2.484e-06, 2.4067e-05, 3.5411e-05, 2.3093e-05, 5.0612e-05, 2.9505e-05, 7.1818e-05, 0.000105624]},
    
    {'Impl all gather': [8.302e-06, 4.8935e-05, 0.00034124, 0.000200905, 0.000190441, 0.000117668, 0.000219614, 0.000384394], 
     'Std all gather': [1.4693e-05, 0.00812732, 0.000431983, 0.00026979, 0.000310872, 0.00020278, 0.000282859, 0.000272685]}
]

thread_counts = [1, 2, 3, 4, 5, 6, 7, 8]
log_thread_counts = np.log2(thread_counts) 

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(log_thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Elapsed Time vs. log(Number of Threads) and Element count", fontsize=16)
    plt.xlabel("log(Number of Threads, P)", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"execution_plot_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
