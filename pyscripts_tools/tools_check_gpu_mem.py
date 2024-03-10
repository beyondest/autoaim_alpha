import torch

def get_gpu_memory():
    available_memory = []
    for i in range(torch.cuda.device_count()):
        gpu_memory = torch.cuda.get_device_properties(i).total_memory
        available_memory.append(gpu_memory)
    return available_memory

if __name__ == "__main__":
    available_memory = get_gpu_memory()
    print("Available GPU memory:")
    for i, memory in enumerate(available_memory):
        print(f"GPU {i}: {memory / (1024 ** 3)} GB")
        
        
        
        
        
        