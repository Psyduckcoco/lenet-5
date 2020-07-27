
# # 读取模型 并把权重参数保存下来
# import torch
# import numpy as np
#
# the_model = torch.load("Lenet.pth")
#
#
# for key, value in the_model.items(): # 由于最邻近差值没有参数，只需要将其他参数赋予给新模型即可
#     # the_model[key] = value
#     # f = open('./parameter/'+key+".txt", "w")
#
#     value_cpu = value.cpu()
#     data_arr=value_cpu.numpy()
#     data_arr = data_arr.flatten()
#     np.savetxt('./parameter/'+key+'.txt', data_arr, fmt='%f', delimiter='\r\n')
#     # f.write(data_arr)

# # 把mnist保存成图片

import os
from skimage import io
import torchvision.datasets.mnist as mnist
import numpy as np
import torch
root = "../dataset/"

# train_set = (
#     mnist.read_image_file(os.path.join(root, 'train-images-idx3-ubyte')),
#     mnist.read_label_file(os.path.join(root, 'train-labels-idx1-ubyte'))
# )

test_set = (
    mnist.read_image_file(os.path.join(root, 'images')),
    mnist.read_label_file(os.path.join(root, 'labels'))
)

# print("train set:", train_set[0].size())
print("test set:", test_set[0].size())

def convert_to_img(train=True):
    if (train):
        f = open(root + 'train.txt', 'w')
        data_path = root + '/train/'
        if (not os.path.exists(data_path)):
            os.makedirs(data_path)
        for i, (img, label) in enumerate(zip(train_set[0], train_set[1])):
            img_path = data_path + str(i) + '.jpg'
            io.imsave(img_path, img.numpy())
            f.write(img_path + ' ' + str(label) + '\n')
        f.close()
    else:
        f = open(root + 'test.txt', 'w')
        data_path = root + '/test/'
        if (not os.path.exists(data_path)):
            os.makedirs(data_path)
        for i, (img, label) in enumerate(zip(test_set[0], test_set[1])):
            img_path = data_path + str(i) + '.jpg'
            io.imsave(img_path, img.numpy())
            # f.write(img_path + ' ' + str(label) + '\n')
            num = label.numpy()
            f.write( str(num) + '\n')
        f.close()


# convert_to_img(True)
convert_to_img(False)