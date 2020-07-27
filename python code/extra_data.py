
import torch

the_model = torch.load("Lenet.pth")


for key, value in the_model.items(): # 由于最邻近差值没有参数，只需要将其他参数赋予给新模型即可
    # the_model[key] = value
    f = open(key"out.txt", "w")

    c = value.cpu()
    b=c.numpy()
    print(key,b)

print ("1")