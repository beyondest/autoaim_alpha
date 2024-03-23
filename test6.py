import h5py
import time
import numpy as np
# 创建或打开 HDF5 文件
output_file = 'dataset.hdf5'

def read_hdf5_file(file_path):
    with h5py.File(file_path, 'r') as f:
        x_data = f['x'][:]  # 读取 x 数据集中的所有数据
        y_data = f['y'][:]  # 读取 y 数据集中的所有数据

    return x_data, y_data
with h5py.File(output_file, 'w') as f:
    # 创建数据集
    x_dset = f.create_dataset('x', shape=(0,10, 3), maxshape=(None, 10, 3), dtype='float32')  # 创建 x 数据集，初始大小为 (0, 3)，最大可扩展到任意行，3 列
    y_dset = f.create_dataset('y', shape=(0, 2), maxshape=(None, 2), dtype='int32')  # 创建 y 数据集，初始大小为 (0,)，最大可扩展到任意行
    # 主循环，模拟获取 x 和 y 数据
    for i in range(10):  # 假设获取 10 组数据
        # 假设每次获取 x 和 y 数据的过程
        t1 = time.perf_counter()
        
        x = np.array([np.ones(3)*i for j in range(10)])  # 假设 x 是一个列表
        y = np.array([i, i+1])  # 假设 y 是一个整数

        # 将 x 和 y 数据写入 HDF5 文件
        x_dset.resize((x_dset.shape[0] + 1,10, 3))  # 扩展 x 数据集的行数
        y_dset.resize((y_dset.shape[0] + 1,2 ))  # 扩展 y 数据集的行数

        x_dset[-1] = x  # 将新的 x 数据写入最后一行
        y_dset[-1] = y  # 将新的 y 数据写入最后一行

        print(f"Recorded data: x={x}, y={y}")
        t2 = time.perf_counter()
        print(f"Time elapsed: {t2 - t1:.4f} seconds")
        


# 读取 HDF5 文件
x_data, y_data = read_hdf5_file(output_file)

# 打印读取的数据
print("Read x data:", x_data)
print("Read y data:", y_data)

print(type(x_data))
print(type(y_data))
print(x_data.shape)
print(y_data.shape)

